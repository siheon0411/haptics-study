/********************************************************************************//**
\file      InnoML_Test_main_input.cpp
\brief     Example of real-time motion streaming with the Input interface.
\copyright Copyright (C) 2016-2017 InnoSimulation, Inc. All rights reserved.
************************************************************************************/

#include <stdio.h>		// for printf
#include <windows.h>	// for sleep
#include <math.h>		// for sin
#include <InnoML.h>		// for motion

#pragma comment(lib, "winmm.lib")

#define SAMPLE_CHANNELS	3	// 3-DOF
#define SAMPLE_RATE		200	// sample time (5ms)
#define SAMPLE_COUNT	4	// buffer play time (20ms)
#define SAMPLE_SIZE		(sizeof(short)*SAMPLE_CHANNELS)
#define BUFFER_SIZE		(SAMPLE_COUNT*SAMPLE_SIZE) // 20ms delay 
#define BUFFER_COUNT	2	// double buffer

static int stream_pos = 0;
static int stream_len = SAMPLE_RATE*SAMPLE_SIZE*1;

static int GenMotionStream(short* buffer, int size, int pos, int duration, float frq, float amp) 
{
	memset(buffer, 0, size);
	for(int i=0; i<size; i+=SAMPLE_SIZE) {
		float time = (float)(pos+i)/duration;
		buffer[0] = (amp * MOTION_MAX_16) * sin(2 * IM_PI * frq * time);
		buffer += SAMPLE_CHANNELS;
	}
	return size;
}

static int CustomMotionFilter(void* stream, int len, short offset, float factor)
{
	int samples = len/SAMPLE_SIZE;
	short* buffer = (short*)stream;
	for(int i=0; i<samples; i++) {
		buffer[0] = (buffer[0] + offset) * factor;
		buffer[1] = (buffer[1] + offset) * factor;
		buffer[2] = (buffer[2] + offset) * factor;
		buffer += SAMPLE_CHANNELS;
	}
	return len;
}

struct netstreamer
{
	netstreamer(IMBuffer input_buffer) {
		// Creates a double buffer
		buffer = input_buffer;
		semaphore = CreateSemaphore(NULL, 1, BUFFER_COUNT, NULL);
	}	
	~netstreamer() {
		CloseHandle(semaphore);	
	}	

	int write(void* data, int size) {
		int len = imBufferEnqueue(buffer, data, size);
		// wait to read front buffer
		WaitForSingleObject(semaphore, INFINITE);
		return len;
	}
	int read(void* data, int size) {
		int len = imBufferDequeue(buffer, data, size);
		// wake up for write to next back buffer
		ReleaseSemaphore(semaphore, 1, NULL);
		return len;
	}		
	int record(int pos) 
	{
		unsigned char read_buffer[BUFFER_SIZE];
		int size = GenMotionStream((short*)read_buffer, BUFFER_SIZE, pos, stream_len, 1.0f, 0.5f);
		if(size >= BUFFER_SIZE) {
			write(read_buffer, BUFFER_SIZE);
		}
		return size;
	}
	static int stream_filter(void* netstream, void* stream, int len)
	{		
		netstreamer* streamer = (netstreamer*)netstream;
		int size = len;	
		// get input stream
		size = streamer->read(stream, size);
		// input stream filtering  : (-0.5 ~ 0.5) -> (-1 ~ 1)
		size = CustomMotionFilter(stream, size, 0, 2);
		return size; // mix size
	}
		
	IMBuffer buffer;
	HANDLE semaphore;
};

// Forces Simulation (Telemetry)
typedef struct {
	short	heave, roll, pitch;
} PLATFORM_POSITION_MESSAGE;

typedef struct {
	float	wx, wy, wz;	/**< roll, pitch, yaw (speed, radians/s) */ 
	float	ax, ay, az;	/**< surge, sway, heave (acceleration, m/s^2) */ 
} SIM_TELEMETRY_MESSAGE;

#define TELEMETRY_CHANNELS sizeof(SIM_TELEMETRY_MESSAGE)/sizeof(float)

static int telemetry_filter_callback(void* netstream, void* stream, int len)
{	
	IMBuffer input_buffer = (IMBuffer)netstream;
	SIM_TELEMETRY_MESSAGE message[SAMPLE_COUNT];
	if(imBufferDequeue(input_buffer, message, sizeof(message)) == 0)
		return 0;

	// Here you can use math filters (washout filter, scaler, mapping algorithms, ...)
	/* simple scaler (Telemetry -> DOF) */
	PLATFORM_POSITION_MESSAGE* dof = (PLATFORM_POSITION_MESSAGE*)stream;
	for(int i=0; i<SAMPLE_COUNT; i++) {
		dof[i].heave = message[i].ay * MOTION_MAX_16;
	}
	return len; // mix size
}

int main(int argc, char *argv[])
{
    /* Start up */	
	IMBuffer master_buffer = imCreateBuffer(SAMPLE_RATE, 0, SAMPLE_CHANNELS, SAMPLE_COUNT, 2); // double buffer
	// set default device profile (check "IMotion_#id.ini")
	IMContext context = imCreateContext(master_buffer);
	imSetContext(context);
	imStart();
	
	// The input buffer must be used to store the sampling stream of the input streamer !
	IMBuffer input_buffer = imCreateBuffer(SAMPLE_RATE, 0, SAMPLE_CHANNELS, SAMPLE_COUNT<<1);
	IMInput input = imCreateInput(input_buffer);

	/**** Motion Input Streamer test ****/
	if(1) // Simple Streamer (Buffer Enqueue)
	{	
		stream_pos = 0;
		imInputStart(input);
		while(stream_pos < stream_len) {			
			// by netstream buffer
			short stream[SAMPLE_COUNT][SAMPLE_CHANNELS];
			int size = GenMotionStream((short*)stream, sizeof(stream), stream_pos, stream_len, 1.0f, 1.0f);
			// submit buffer
			stream_pos += imBufferEnqueue(input_buffer, stream, size);
			Sleep(10);	
		}
		imInputStop(input);
		fprintf(stderr, "Input Streamer (Callback) completed ... \n\n");
	}	

	if(1) // Streamer & Filter (Buffer Enqueue/Dequeue)
	{	
		stream_pos = 0;
		netstreamer netstream(input_buffer);
		imInputStart(input, netstream.stream_filter, &netstream);
		while(stream_pos < stream_len) {
			stream_pos += netstream.record(stream_pos);	
		}
		imInputStop(input);
		fprintf(stderr, "Input Streamer (stream object) completed ... \n\n");
	}
	
	/**** Motion Input Sampler test ****/
	if(1) // Direct Positioning Mode (Sample Stream)
	{
		imInputStart(input);
		// motion input sampler by motion table transform
		short sample[SAMPLE_CHANNELS];
		unsigned int start_ticks = timeGetTime();
		for(int time=0; time < 1000*1; ) {
			int size = GenMotionStream(sample, sizeof(sample), time, 1000, 1.0f, 1.0f);
			imInputSendStream(input, sample, size);
			// The real-time sampling time is ideal for signals that are less than half the device sample time.
			Sleep((1000/SAMPLE_RATE)>>1);
			time = timeGetTime() - start_ticks;
		}	
		// wait complete  (play_time 1sec)
		while(imBufferGetQueuedCount(input_buffer))
			Sleep(1);			
		imInputStop(input);
		fprintf(stderr, "Input Sampler (Direct Positioning) completed ... \n\n");
	}
	
	/**** Telemetry test ****/
	if(1) // Forces Simulation Mode (Telemetry Stream)
	{		
		IMBuffer telemetry_buffer = imCreateBuffer(SAMPLE_RATE, IM_FORMAT_DATA_F32, TELEMETRY_CHANNELS, SAMPLE_COUNT<<1);
		imInputSetBuffer(input, telemetry_buffer);

		imInputStart(input, telemetry_filter_callback, (void*)telemetry_buffer);
		// motion input sampler by telemetry
		SIM_TELEMETRY_MESSAGE telemetry;	
		memset(&telemetry, 0, sizeof(SIM_TELEMETRY_MESSAGE));

		unsigned int dt = 1000/SAMPLE_RATE;
		for(int time=0; time<=1000; time+=dt) {
			telemetry.ay = sin(2 * IM_PI * (float)time/1000);			
			imInputSendStream(input, &telemetry, sizeof(SIM_TELEMETRY_MESSAGE));
			Sleep(dt);
		}
		imInputStop(input);
		fprintf(stderr, "Input Sampler (Forces Simulation) completed ... \n\n");		
	}		
	imDeleteInput(input);
	imDeleteBuffer(input_buffer);

    /* Clean up */
	imStop();
	imSetContext(NULL);
	imDestroyContext(context);
    return (0);
}
