/********************************************************************************//**
\file      InnoML_Test_main_context.cpp
\brief     Example of multiple contexts using InnoML.
\copyright Copyright (C) 2016-2017 InnoSimulation, Inc. All rights reserved.
************************************************************************************/

#include <stdio.h>		// for printf
#include <windows.h>	// for sleep
#include <math.h>		// for sin
#include <InnoML.h>		// for motion

static int GenMotionBuffer(short* buf, int samples, int channels, int index)
{
	for(int i=0; i<samples; i++) {
		buf[i*channels + index] = (short)(sin(2 * IM_PI * (float)i/samples)*MOTION_MAX_16);
	}
	return samples;
}

static int CreateSineMotion(int channel)
{	
	short buf[IM_FORMAT_SAMPLE_RATE_DEFAULT*IM_FORMAT_CHANNELS_DEFAULT] = {0,};	
	GenMotionBuffer(buf, IM_FORMAT_SAMPLE_RATE_DEFAULT, IM_FORMAT_CHANNELS_DEFAULT, channel);

	IMBuffer buffer = imCreateBuffer(IM_FORMAT_SAMPLE_RATE_DEFAULT, IM_FORMAT_DATA_DEFAULT, IM_FORMAT_CHANNELS_DEFAULT, IM_FORMAT_SAMPLE_RATE_DEFAULT);
	imBufferEnqueue(buffer, buf, sizeof(buf));

	return imCreateSource(buffer);
}

static IMContext context[2] = {0,};
static void PlayMotion(int index, bool stack)
{
	if(index > 1)
		return;

	IMContext ctx = context[index];
	IMContext old = imGetContext();	// save
	imSetContext(ctx);
	int motion = CreateSineMotion(index);

	if(stack) {
		PlayMotion(index+1, stack);
		ctx = imGetContext();		// current
		Sleep(500);
	}

	imSourcePlay(motion, 0, NULL);
	imSetContext(old);	// restore
}


int main(int argc, char *argv[])
{		
	/*
	Motion Resampling
	For resampling, the number of context samples must be at least resample_rate ()
	resample_rate = DST_SAMPLES / SRC_SAMPLES (x1, x2, x4, ...)
	*/
	if(1) // Buffer Resample Test - rate(2x), duration (1sec)
	{
	#if 0 // Upsample (50Hz -> 100Hz)
		#define SRC_SAMPLES	50
		#define DST_SAMPLES	100
	#else // Downsample (100Hz -> 50Hz)
		#define SRC_SAMPLES	100
		#define DST_SAMPLES	50
	#endif

		IMBuffer master = imCreateBuffer(DST_SAMPLES);	
		IMContext context = imCreateContext(master);
		imSetContext(context);	
		imStart();

		short buf[SRC_SAMPLES*IM_FORMAT_CHANNELS_DEFAULT] = {0,};	
		GenMotionBuffer(buf, SRC_SAMPLES, IM_FORMAT_CHANNELS_DEFAULT, 0);
		IMBuffer buffer = imCreateBuffer(SRC_SAMPLES, 0, 0, SRC_SAMPLES);
		imBufferEnqueue(buffer, buf, sizeof(buf));
		
		// Create Adjusted Buffer
		IMBuffer adjusted_buffer;
		float cvt_ratio = imBufferConvert(buffer, &adjusted_buffer);

		IMSource source = imCreateSource(adjusted_buffer);		
		imSourcePlay(source);
		while(imGetPlayingSourceCount())	// wait playlist count (sync)
			Sleep(10);
		
		imStop();
		imSetContext(NULL);	
		imDestroyContext(context);
		fprintf(stderr, "1st wave No-Resampling completed ... (%dhz -> %dhz) \n\n", SRC_SAMPLES, DST_SAMPLES);
	}
	
	if(1) // Context Resample Test - rate(2x), duration (1sec)
	{
	#if 1 // Upsample (50Hz -> 100Hz)
		#define SRC_SAMPLES	50
		#define DST_SAMPLES	100
	#else // Downsample (100Hz -> 50Hz)
		#define SRC_SAMPLES	100
		#define DST_SAMPLES	50
	#endif

		IMBuffer master = imCreateBuffer(DST_SAMPLES);	
		IMContext context = imCreateContext(master);
		imSetContext(context);		
		imStart();

		short buf[SRC_SAMPLES*IM_FORMAT_CHANNELS_DEFAULT] = {0,};	
		GenMotionBuffer(buf, SRC_SAMPLES, IM_FORMAT_CHANNELS_DEFAULT, 0);
		IMBuffer buffer = imCreateBuffer(SRC_SAMPLES, 0, 0, SRC_SAMPLES);
		imBufferEnqueue(buffer, buf, sizeof(buf));	
		IMSource source = imCreateSource(buffer);

		imSourcePlay(source);
		while(imGetPlayingSourceCount())	// wait playlist count (sync)
			Sleep(10);

		imStop();
		imSetContext(NULL);	
		imDestroyContext(context);
		fprintf(stderr, "2nd wave Upsampling completed ... (%dhz -> %dhz) \n\n", SRC_SAMPLES, DST_SAMPLES);
	}	
	
	/*
	Multiple Context
	*/
	if(1) {
		context[0] = imCreateContext(0, 11);
		context[1] = imCreateContext(0, 12);

		imSetContext(context[0]);
		imStart();
		imSetContext(context[1]);
		imStart();

		if(1) // Concurrent Context Test
		{
			PlayMotion(1, false);
			//Sleep(1000);
			PlayMotion(0, false);
			Sleep(1000);
			fprintf(stderr, "Concurrent Context Handling ... \n\n");
		}
	
		if(1) // Recursive Context Test
		{
			PlayMotion(0, true);
			Sleep(1000);
			fprintf(stderr, "Recursive Context Handling ... \n\n");
		}
	
		imSetContext(context[0]);
		imStop();
		imSetContext(context[1]);
		imStop();

		imSetContext(NULL);
		imDestroyContext(context[0]);	
		imDestroyContext(context[1]);
	}

	/*
	Context Sharing
	*/
	if(1) {
		context[0] = imCreateContext(0, 11);
		context[1] = imCreateContext(0, 12);
		
		imSetContext(context[1]);
		imStart(0,0,context[0]);	// Start ctx1 (begin Sharing)
		
		imSetContext(context[0]);	
		imStart();					// Start ctx0

		PlayMotion(0, false);		// Play ctx0
		Sleep(500);

		imSetContext(context[1]);
		imStop();					// Stop ctx1 (end Sharing)
		fprintf(stderr, "context sharing completed ... \n\n");	
		Sleep(500);

		imSetContext(context[0]);
		imStop();					// Stop ctx0

		imSetContext(NULL);
		imDestroyContext(context[0]);	
		imDestroyContext(context[1]);
	}
	return 0;
}
