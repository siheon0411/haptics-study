/********************************************************************************//**
\file      IMotion_Test_main_stream.cpp
\brief     Real-time motion streaming example using IMotion interface.
\copyright Copyright (C) 2016-2017 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

#include <stdio.h>
#include <math.h>
#include <windows.h>

#include "IMotion.h"

#define SAMPLE_CHANNELS			3	// 3-DOF
#define SAMPLE_RATE				200	// 10ms
#define SAMPLE_FORMAT			IM_FORMAT_DATA_S16	// signed short (-32768 ~ 32767)
#define SAMPLE_BYTES			(MOTION_SAMPLE_BYTE(SAMPLE_FORMAT)*SAMPLE_CHANNELS)
#define STREAMING_BUFFER_SIZE	(4*SAMPLE_BYTES) // 80 ms delay 
#define MAX_BUFFER_COUNT		2 

int streaming_length = SAMPLE_RATE * SAMPLE_BYTES * 2; // play time : 2 sec
uint8 buffers[MAX_BUFFER_COUNT][STREAMING_BUFFER_SIZE]; 
int currentBuffer = 0;
HANDLE semaphore;

#define MOTION_FREQUENCY	1.0		// Motion Samples Per Sec (1 cycle)
#define MOTION_AMPLITUDE	1.0		// Motion Sample Volume (0 ~ 1.0)

int ReadStream(uint8* buffer, int buffer_bytes, int read_bytes, float frq[], float amp[]) 
{
	int16* buf16 = (int16*)buffer;
	int samples = buffer_bytes/SAMPLE_BYTES;
	int pos = read_bytes/SAMPLE_BYTES;
	for(int i=0; i<samples; i++) {
		buf16[0] = amp[0] * sin(2 * IM_PI * (float)(pos+i)/SAMPLE_RATE * frq[0]) * MOTION_MAX_16;
		buf16[1] = amp[1] * sin(2 * IM_PI * (float)(pos+i)/SAMPLE_RATE * frq[1]) * MOTION_MAX_16;
		buf16[2] = amp[2] * sin(2 * IM_PI * (float)(pos+i)/SAMPLE_RATE * frq[2]) * MOTION_MAX_16;
		buf16 += SAMPLE_CHANNELS;
	}

	return buffer_bytes;
}

void ChangedBufferCallback(void* context, uint32 state)
{
	if(state != IM_END_OF_BUFFER)
		return;

	IMotion* handler = (IMotion*)context;
	// diagnostics
	int error = handler->GetInfo(NULL);

	ReleaseSemaphore(semaphore, 1, NULL);
}

void WaitEndOfBuffer() {
	WaitForSingleObject(semaphore, INFINITE);
}

int main(int argc, char *argv[])
{
    /* Start up */	
	IMotion_Startup();
	
	/* Create device (default driver) */
	IMotion* motion = IMotion_Create();
	if(motion == NULL) {
        fprintf(stderr, "Couldn't open motion\n");
		IMotion_Shutdown();
		return 0;
    }
	motion->Start();
	
    semaphore = CreateSemaphore(NULL, 1, 2, NULL);
	
	/* Create source */
	IM_FORMAT format;
	memset(&format, 0, sizeof(IM_FORMAT));
	format.nSampleRate = SAMPLE_RATE;
	format.nChannels = SAMPLE_CHANNELS;
	format.nDataFormat = SAMPLE_FORMAT;
	IMotionSource* source = motion->CreateSource(&format, ChangedBufferCallback, IM_END_OF_BUFFER);
	source->Start();
	
    /* Submit Streamimg Buffers */
	int pause_test = 1;
	float frq[SAMPLE_CHANNELS] = {MOTION_FREQUENCY, MOTION_FREQUENCY, MOTION_FREQUENCY};
	float amp[SAMPLE_CHANNELS] = {MOTION_AMPLITUDE, MOTION_AMPLITUDE, MOTION_AMPLITUDE};
	int currentPos = 0;
	while(currentPos < streaming_length) {
		// Read Buffer
		int size = ReadStream(buffers[currentBuffer], STREAMING_BUFFER_SIZE, currentPos, frq, amp);
		if(!size) 
		{
			fprintf(stderr, "ReadStream Error !\n");
			break;
		}
		currentPos += size;

		// Pause & Resume
		if(pause_test && currentPos > streaming_length/2) {
			fprintf(stderr, "Pause !\n\n");
			Sleep(1000);
			fprintf(stderr, "Resume !\n\n");
			pause_test = 0;
		}

		// Play Buffer
		IM_BUFFER buf;	
		memset(&buf, 0, sizeof(IM_BUFFER));	
		buf.nMotionBytes = size;
		buf.pMotionData = (const uint8*)buffers[currentBuffer];
		buf.pContext = motion;
		source->SubmitBuffer(&buf);
		fprintf(stderr, "Stream Submit : %d/%d \n", currentPos, streaming_length);
			
        currentBuffer++;
        currentBuffer %= MAX_BUFFER_COUNT;

		// Wait Buffer
		WaitEndOfBuffer();
	}
	
    /* Wait Queued Buffers */
	while(source->GetQueuedBufferCount() > 0)
		WaitEndOfBuffer();
	source->Stop();
	
    /* Clean up */	
	CloseHandle(semaphore);	
	motion->DestroySource(source);	
	IMotion_Destroy(motion);
	IMotion_Shutdown();

    return (0);
}
