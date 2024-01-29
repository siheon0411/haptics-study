/********************************************************************************//**
\file      InnoML_Test_main_telemetry.cpp
\brief     Example of Washout input filtering using IPC Telemetry.
\copyright Copyright (C) 2019 InnoSimulation, Inc. All rights reserved. 
************************************************************************************/

#include <stdio.h>		// for printf
#include <windows.h>	// for sleep
#include <conio.h>		// for kbhit, getch
#include <InnoML.h>		// for motion

typedef struct {
	short	surge, sway, heave;	/**< platform translation, mm */ 
	short	roll, pitch, yaw;	/**< platform rotation, degree */ 	
} PLATFORM_POSITION_MESSAGE;

typedef struct {	
	short	force[IM_DOF_COUNT]; /**< acceleration(m/s^2), angular velocity(radians/s) */ 
} FORCE_SIMULATION_MESSAGE;

#define PLATFORM_CHANNELS	(sizeof(PLATFORM_POSITION_MESSAGE)/sizeof(short))
#define FORCE_CHANNELS	(sizeof(FORCE_SIMULATION_MESSAGE)/sizeof(short))
#define SAMPLE_RATE		IM_FORMAT_SAMPLE_RATE_DEFAULT
#define SAMPLE_COUNT	1

// telemetry example : project cars2 ipc
// cf) profile/ProjectCARS2_Profile.ini
#define PCARS_AXIS_COUNT 1829
typedef struct {
	float	force[PCARS_AXIS_COUNT];	/**< acceleration(m/s^2), angular velocity(radians/s) */  
} SIM_TELEMETRY_MESSAGE;
// 1735~1740 : AngularVelX/Y/Z, LocalAccelX/Y/Z
static int s_pcars_axis_map[IM_DOF_COUNT] = {1740,1738,1739,1737,1735,1736};
static int s_pcars_axis_scale[IM_DOF_COUNT] = {10,-10,10,-100,100,-100};

struct IPC_HANDLER {
	void* m_hFileHandle;
	void* m_pViewOfFile;
	int m_nFileSize;
	
	bool open(const char* filename, int filesize)
	{
		m_nFileSize = filesize;
		// creates as well as opens a file map.
		m_hFileHandle = CreateFileMapping(	INVALID_HANDLE_VALUE,               // handle to file
											NULL,								// security
											PAGE_READWRITE,                     // protection
											0,						            // high-order DWORD of size
											m_nFileSize,			            // low-order DWORD of size
											filename	  	                    // object name
										 );
		if( !m_hFileHandle ) {
			fprintf(stderr, "Unable to create file mapping. Error is %d", GetLastError());
			return false;
		}
	
		// Map the file into the current address space.
		m_pViewOfFile = MapViewOfFile(	m_hFileHandle,							// handle to file-mapping object
										FILE_MAP_ALL_ACCESS,					// access mode
										0,										// high-order DWORD of offset
										0,										// low-order DWORD of offset
										0										// number of bytes to map
									);
		if( m_pViewOfFile == NULL ) {
			fprintf(stderr, "Unable to map to the current address space. Error is %d", GetLastError());
			return false;
		}
		return true;
	}

	bool close()
	{
		if( m_pViewOfFile != NULL ){
			if( UnmapViewOfFile(m_pViewOfFile) == 0)
				fprintf(stderr, "Unable to unmap view of file. Error is %d", GetLastError());
		}
		if( m_hFileHandle!= NULL)
			CloseHandle(m_hFileHandle);

		m_hFileHandle = NULL;
		m_pViewOfFile = NULL;
		m_nFileSize = 0;
		return true;
	}

	int read(char* data, int nBytes)
	{
		//assert( m_nFileSize != 0 );
		if(nBytes > m_nFileSize)
			nBytes = m_nFileSize;
		memcpy( data, m_pViewOfFile, nBytes );
		return nBytes;
	}

	int write(const char* data, int nBytes)
	{
		//assert( m_nFileSize != 0 );
		if(nBytes > m_nFileSize)
			nBytes = m_nFileSize;
		memcpy( m_pViewOfFile, data, nBytes );
		return nBytes;
	}
};

static IMFilter create_washout_filter()
{	
	// noise covariance constant (default 5, 0~100)
	IMFilter noise_filter = imCreateFilter(IM_FILTER_NOISE);
	IM_FILTER_NOISE_PARAMS noise_params[] = {50,100,20,50,1,100};
	imFilterSetParams(noise_filter, noise_params, sizeof(IM_FILTER_NOISE_PARAMS), 6);
	// washout cutoff frequency (default 5 hz)
	IMFilter default_classical_washout = imCreateFilter(IM_FILTER_WASHOUT);
	IM_FILTER_WASHOUT_PARAMS washout_params[] = {5,5,5,10,10,5};
	imFilterSetParams(default_classical_washout, washout_params, sizeof(IM_FILTER_WASHOUT_PARAMS), 6);
	// output scale for motion cueing
	IMFilter simple_scaler = imCreateFilter(IM_FILTER_SCALE);
	IM_FILTER_SCALE_PARAMS scaler_params[] = {200,5,40,12,10,10};
	imFilterSetParams(simple_scaler, scaler_params, sizeof(IM_FILTER_SCALE_PARAMS), 6);
	// motion rate limit per msec (default 256 : rate/32767)
	IMFilter platform_limiter = imCreateFilter(IM_FILTER_RATELIMIT);

	IMFilter filter = imCreateFilter();
	imFilterAppend(filter, noise_filter);
	imFilterAppend(filter, default_classical_washout);
	imFilterAppend(filter, simple_scaler);	
	imFilterAppend(filter, platform_limiter);
	return filter;
}

int main(int argc, char *argv[])
{		
    /* Start up */	
	IMBuffer master_buffer = imCreateBuffer(SAMPLE_RATE, 0, PLATFORM_CHANNELS, SAMPLE_COUNT);
	// set default device profile (check "IMotion_#id.ini")
	IMContext context = imCreateContext(master_buffer);
	imSetContext(context);
	
	// get device description
	IM_DEVICE_DESC desc;
	imGetProfile(&desc);
	// get device diagnostics
	int error = imGetDiagnostic(NULL, 0);
	if(error) {
		if(error == -1)
			fprintf(stderr, "Device Open Failed (running emulation mode) !\n\n");
		else {
			fprintf(stderr, "Device Driver Error (code : %d) !\n", error);
			exit(0);
		}
	}		

	IPC_HANDLER ipc;
	ipc.open("$pcars$", PCARS_AXIS_COUNT<<2);
	SIM_TELEMETRY_MESSAGE telemetry = {0};
	
	/**** Force Simulation ****/
	imStart();
	// It is usually used to convert logical motion data to physical motion data. (force -> motion)
	IMBuffer input_buffer = imCreateBuffer(SAMPLE_RATE, IM_FORMAT_DATA_S16, FORCE_CHANNELS, SAMPLE_COUNT, 2);
	IMInput input = imCreateInput(input_buffer);
	
	// Set output filter for the motion input
	IMFilter filter = create_washout_filter();
	imInputSetFilter(input, filter);
	imInputStart(input); // filter build

	FORCE_SIMULATION_MESSAGE sample;	
	memset(&sample, 0, sizeof(FORCE_SIMULATION_MESSAGE));
	unsigned int dt = 1000/SAMPLE_RATE;
	while(!kbhit()) {
		ipc.read((char*)telemetry.force, PCARS_AXIS_COUNT<<2);
		for(int i=0; i<IM_DOF_COUNT; i++) {
			sample.force[i] = telemetry.force[s_pcars_axis_map[i]] * s_pcars_axis_scale[i] * 256;	// quantizing (16 bit)
		}
		imBufferEnqueue(input_buffer, &sample, sizeof(FORCE_SIMULATION_MESSAGE)); // encoding (pcm buffer)
		// The real-time sampling time is ideal for signals that are less than half the device sample time.
		Sleep((1000/SAMPLE_RATE)>>1);
	}

	imInputStop(input);	
	imDeleteFilter(filter);
	fprintf(stderr, "Force Simulation completed ... \n\n");		
		
	ipc.close();

    /* Clean up */
	imDeleteBuffer(input_buffer);
	imDeleteInput(input);

	imStop(); // stop motion streaming (move init position)
	imDeleteBuffer(master_buffer);
	imSetContext(NULL); // release context
	imDestroyContext(context); // shutdown device	
	return 0;
}
