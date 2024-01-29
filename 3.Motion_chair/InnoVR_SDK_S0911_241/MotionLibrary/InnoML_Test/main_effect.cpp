/********************************************************************************//**
\file      InnoML_Test_main_effect.cpp
\brief     Example of controlling multiple device with the Context interface.
\copyright Copyright (C) 2016-2017 InnoSimulation, Inc. All rights reserved.
************************************************************************************/

#include <stdio.h>		// for printf
#include <windows.h>	// for sleep
#include <math.h>		// for sin
#include <InnoML.h>		// for motion

#define SAMPLE_CHANNELS	3	// 3-DOF

static bool timeout = false;
static void motion_notify(void* obj, unsigned int state)
{		
	IM_DIAGNOSTIC_AXIS_INFO info[3];
	int error = imGetDiagnostic(info, 3);

	if(state == IM_END_OF_STREAM)
		timeout = true;
}

static int GenMotionBuffer(short* buf, int samples, int channels, int index)
{
	for(int i=0; i<samples; i++) {
		buf[i*channels + index] = (short)(sin(2 * IM_PI * (float)i/samples)*MOTION_MAX_16);
	}
	return samples;
}

IMBuffer GenMotionWave(int channel)
{
	IMBuffer buffer;
	short buf[IM_FORMAT_SAMPLE_RATE_DEFAULT*IM_FORMAT_CHANNELS_DEFAULT];

	memset(buf, 0, sizeof(buf));
	GenMotionBuffer(buf, IM_FORMAT_SAMPLE_RATE_DEFAULT, IM_FORMAT_CHANNELS_DEFAULT, channel);	
	buffer = imCreateBuffer(IM_FORMAT_SAMPLE_RATE_DEFAULT, 0, 0, IM_FORMAT_SAMPLE_RATE_DEFAULT);
	imBufferEnqueue(buffer, buf, sizeof(buf));	
	return buffer;
}

struct motion_listener
{
	motion_listener() : complete_counter(0) {}
	virtual bool on_event(unsigned int state)
	{
		if(state == IM_END_OF_STREAM) {
			complete_counter++;			
			fprintf(stderr, "listener : %d wave completed ... \n",complete_counter);
		}
		return true;
	}
	int complete_counter;
};

static void motion_event_handler(void* obj, unsigned int msg)
{		
	motion_listener* listener = (motion_listener*)obj;
	listener->on_event(msg);
}

int main(int argc, char *argv[])
{
    /* Start up */	
	// set default device profile (check "IMotion_#id.ini")
	IMContext context = imCreateContext(); 
	imSetContext(context);
	imStart();

	/**** Motion Source Player test ****/
	// url load
	const char* url = "../../MotionData/waveform_sine.csv";
				
	IMBuffer buffer = imLoadBuffer(url);		// load url	
	IMSource source = imCreateSource(buffer);	
	int duration = imBufferGetDuration(buffer); // duration (1 sec)
	
	if(1) // load url
	{		
		fprintf(stderr, "<Simple Play Test> \n");
		imSourcePlay(source);
		while(imGetPlayingSourceCount()) {	// wait playlist count (sync)
			Sleep(10);
		}
		imSourceStop(source);
		fprintf(stderr, "1st wave completed (load url) ... \n\n");
	}
	
	if(1) // load file memory
	{		
		fprintf(stderr, "<Load File Memory Test> \n");
		// read file memory
		FILE* fp = fopen(url, "rb");
		if(fp) {
			size_t	lSize;
			fseek(fp, 0, SEEK_END);
			lSize = ftell (fp);
			fseek(fp, 0, SEEK_SET);	
			void* data = malloc(lSize);
			fread (data, 1, lSize, fp);
			// load buffer from file memory
			IMBuffer buffer2 = imLoadBufferMemory(data, lSize);	// load memory	
			int duration2 = imBufferGetDuration(buffer2); // duration (1 sec)
			free(data);
			fclose(fp);
			imSourceSetBuffer(source, buffer2);
		}

		imSourcePlay(source);
		while(imSourceGetPosition(source) < duration) {
			fprintf(stderr, "position (%d/%d ms) \n", imSourceGetPosition(source), duration);
			Sleep(10);
		}
		imSourceStop(source);	
		fprintf(stderr, "1st wave completed (load memory) ... \n\n");
	}

	if(1) // replay/volume/position/event/speed	
	{
		fprintf(stderr, "<Additional Playback Tests> \n");
		imSourceSetVolume(source, 50);	// volume (0~100)
		imSourceSetSpeed(source, 50);	// speed (1/2x)
		imSourcePlay(source, 0, motion_notify, NULL);
		while(!timeout) {	// precise timeout by event callback (async)
			int pos = imSourceGetPosition(source);		// position
			fprintf(stderr, "position (%d/%d ms) \n", pos, duration);
			Sleep(10);
		}
		fprintf(stderr, "2nd wave completed ... \n\n");
	}

	if(1) // pause/resume/play/stop/loop
	{
		fprintf(stderr, "<Playback State Tests> \n");
		imSourcePlay(source, IM_LOOP_INFINITE); // play loop
		fprintf(stderr, "play (infinite repeat) ... \n");
		Sleep(1000);
		imSourcePause(source, true);	// pause
		fprintf(stderr, "pause (1 sec) ... \n");
		Sleep(1000);
		imSourcePause(source, false);	// resume
		fprintf(stderr, "resume (1 sec) ... \n");
		Sleep(1000);		
		imSourceStop(source);	// stop
		fprintf(stderr, "stop (repeat break) ... \n");
		fprintf(stderr, "3rd wave completed ... \n\n");
	}

	/**** Motion Source Mixer test ****/
	IMBuffer buffers[SAMPLE_CHANNELS];
	IMSource wave[SAMPLE_CHANNELS];

	for(int i=0; i<SAMPLE_CHANNELS; i++) {
		buffers[i] = GenMotionWave(i);
		wave[i] = imCreateSource(buffers[i]);
	}

	if(1) // mix test
	{
		fprintf(stderr, "<Mixer Test (wave1 + wave2)> \n");
		// play wave1 (mix to channel 1)
		imSourcePlay(wave[1], 0, NULL);
		// play wave2 (mix to channel 2)
		imSourcePlay(wave[2], 0, NULL);
		Sleep(1000);
		fprintf(stderr, "wave1 & wave2 mix completed ... \n\n");
	}

	if(1) // replay test (Same buffer and same source)
	{
		fprintf(stderr, "<Replay Test 1 (Same buffer and same source) >\n");
		motion_listener listener;
		fprintf(stderr, "play wave0 \n");
		imSourcePlay(wave[0], 0, motion_event_handler, &listener);
		Sleep(500);	
		fprintf(stderr, "play wave0 (replay same source)\n");
		imSourcePlay(wave[0], 0, motion_event_handler, &listener);
		Sleep(200);	

		while(listener.complete_counter == 0)
			Sleep(10);
		fprintf(stderr, "wave0 is completed (2nd source) ... \n\n");
	}
			
	if(1) // reference test (Same buffer and other sources)
	{		
		fprintf(stderr, "<Replay Test 2 (Same buffer and other sources)>\n");
		// Use same buffer as wave0
		imSourceSetBuffer(source, imSourceGetBuffer(wave[0]));

		fprintf(stderr, "play wave0 \n");
		imSourcePlay(wave[0], 0, NULL);
		Sleep(500);		
		fprintf(stderr, "play source (mix other source) \n");
		imSourcePlay(source, 0, NULL);
		Sleep(200);	

		while(imGetPlayingSourceCount() > 1)
			Sleep(10);
		fprintf(stderr, "wave0 is completed (1st source) ... \n");

		while(imGetPlayingSourceCount())
			Sleep(10);
		fprintf(stderr, "source is completed (2nd source) ... \n\n");
	}
		
    /* Clean up */	
	imStop();
	imSetContext(NULL);
	imDestroyContext(context);
	return 0;
}
