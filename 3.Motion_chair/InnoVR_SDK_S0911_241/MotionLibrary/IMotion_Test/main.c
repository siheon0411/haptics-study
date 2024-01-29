/********************************************************************************//**
\file      IMotion_Test_main.c
\brief     Basic example using IMotion's C interface.
\copyright Copyright (C) 2016-2017 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

#include <stdio.h>
#include <math.h>
#include <windows.h>

#include "IMotion.h"

#define SAMPLE_CHANNELS	IM_FORMAT_CHANNELS_DEFAULT
#define SAMPLE_RATE		IM_FORMAT_SAMPLE_RATE_DEFAULT

float motion_frequency = 1; /* motion frequency in cycles per sample */
float motion_amplitude = 1; /* motion scale, -32768 ~ 32767 */

int	main(int argc, char *argv[])
{
	IMotion* motion;
	IMotionSource* source;
	IM_DEVICE_DESC desc;
	IM_FORMAT format;
	IM_BUFFER buffer;
	int16 buf[SAMPLE_RATE][SAMPLE_CHANNELS] = {0,};
	int i, error;

    /* Start up */	
	IMotion_Startup(0, 0);

	// Create device (default driver)
	motion = IMotion_Create(0, NULL);
	IMotion_Start(motion, 0, 0, 0);
	
	IMotion_GetProfile(motion, &desc);

	/**** Motion Stream test (1st wave) ****/
	memset(buf, 0, sizeof(int16)*SAMPLE_RATE*SAMPLE_CHANNELS);
	for(i=0; i<SAMPLE_RATE; i++) {
		double wave = motion_amplitude * sin(2 * IM_PI * (float)i/SAMPLE_RATE * motion_frequency);
		buf[i][0] = (int16)(wave*MOTION_MAX_16);
		#if 1 // manually device operation test
		IMotion_SendStream(motion, (uint8*)&buf[i], sizeof(int16)*SAMPLE_CHANNELS, 1);
		Sleep(1000/SAMPLE_RATE);
		#endif
	}
	
	// diagnostics
	error = IMotion_GetInfo(motion, NULL);
	if(error) {
		if(error == -1)
			fprintf(stderr, "Device Open Failed (running emulation mode) !\n\n");
		else {
			fprintf(stderr, "Device Driver Error (code : %d) !\n", error);
			exit(0);
		}
	}
	
	/**** Motion Buffer test (2nd wave) ****/
	memset(&format, 0, sizeof(IM_FORMAT));
	format.nSampleRate = SAMPLE_RATE;
	format.nChannels = SAMPLE_CHANNELS;
	format.nDataFormat = IM_FORMAT_DATA_DEFAULT;
	source = IMotion_CreateSource(motion, &format, NULL, 0);
	
	memset(&buffer, 0, sizeof(IM_BUFFER));
    buffer.nMotionBytes = SAMPLE_RATE*SAMPLE_CHANNELS*sizeof(int16);
    buffer.pMotionData = (uint8*)buf;
	#if 1 // buffer loop test
	buffer.nLoopCount = 1; 
	#endif
	IMotionSource_SubmitBuffer(source, &buffer);

	IMotionSource_Start(source, 0);	
    // Wait Queued Buffers 
	while(IMotionSource_GetQueuedBufferCount(source) > 0)
		Sleep(10);
	IMotionSource_Stop(source, 0);

    /* Clean up */	
	IMotion_DestroySource(motion, source);	
	IMotion_Stop(motion, 0);
	IMotion_Destroy(motion, 0);
	IMotion_Shutdown();
	return (0);
}