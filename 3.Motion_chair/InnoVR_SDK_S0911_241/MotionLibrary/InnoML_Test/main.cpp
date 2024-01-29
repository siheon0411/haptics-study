/********************************************************************************//**
\file      InnoML_Test_main.cpp
\brief     Basic example using InnoML.
\copyright Copyright (C) 2016-2017 InnoSimulation, Inc. All rights reserved.
************************************************************************************/

#include <stdio.h>		// for printf
#include <windows.h>	// for sleep
#include <math.h>		// for sin
#include <InnoML.h>		// for motion

#define SAMPLE_CHANNELS	IM_FORMAT_CHANNELS_DEFAULT
#define SAMPLE_RATE		IM_FORMAT_SAMPLE_RATE_DEFAULT
#define SAMPLE_COUNT	4 

int main(int argc, char *argv[])
{		
    /* Start up */	
	// set device profile	
	IM_DEVICE_DESC desc;
	memset(&desc, 0, sizeof(IM_DEVICE_DESC));
	desc.szName = "Inno Motion Seat";
	desc.nOptions |= IM_CFG_DEBUG_MODE;
	
	// create device context
	IMBuffer master_buffer = imCreateBuffer(SAMPLE_RATE, 0, SAMPLE_CHANNELS, SAMPLE_COUNT, 2); //<- 요기 6ch
	IMContext context = imCreateContext(master_buffer,0,&desc); // default device context
	imSetContext(context); // make context current
	imStart(); // start motion streaming (move center position)
	
	// get device description
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
	
	/**** Motion Stream test (1st wave) ****/
	int16 buf[SAMPLE_RATE][SAMPLE_CHANNELS] = {0,};	
	memset(buf, 0, sizeof(buf));
	for(int i=0; i<SAMPLE_RATE; i++) {
		buf[i][0] = (int16)(sin(2 * IM_PI * (float)i/SAMPLE_RATE)*MOTION_MAX_16);
	#if 1 // manually motion operation test
		imBufferEnqueue(master_buffer, buf[i], sizeof(int16)*SAMPLE_CHANNELS);
		Sleep(1000/SAMPLE_RATE);
	#endif
	}
	fprintf(stderr, "1st wave completed ... \n\n");
	
	/**** Motion Buffer test (2nd wave) ****/
	IMBuffer buffer = imCreateBuffer(SAMPLE_RATE,0,SAMPLE_CHANNELS,SAMPLE_RATE); //<- 요기 6ch
	imBufferEnqueue(buffer, buf, sizeof(buf));	// fillup motion data to buffer
	int duration = imBufferGetDuration(buffer);		// get playtime (ms)
	
    // Play Source
	IMSource source = imCreateSource(buffer);	
	imSourcePlay(source,0,0,0); // IM_LOOP_INFINITE
	while(imGetPlayingSourceCount()) {	// wait playlist count (sync)
		int pos = imSourceGetPosition(source);	// get position
		//fprintf(stderr, "*** Play Position (%d%%) : %d/%d ms ***\n", pos*100/duration, pos, duration);
		Sleep(10);
	}
	imSourceStop(source);
	imDeleteSource(source);	
	imDeleteBuffer(buffer);
	fprintf(stderr, "2nd wave completed ... \n\n");
	
    /* Clean up */
	imStop(); // stop motion streaming (move init position)
	imSetContext(NULL); // release context
	imDestroyContext(context,0); // shutdown device	
	imDeleteBuffer(master_buffer);
	return 0;
}
