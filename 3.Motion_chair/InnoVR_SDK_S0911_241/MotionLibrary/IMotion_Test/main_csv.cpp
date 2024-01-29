/********************************************************************************//**
\file      IMotion_Test_main_csv.cpp
\brief     Example of using motion effect files with IMotion.
\copyright Copyright (C) 2016-2017 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

#include <stdio.h>
#include <windows.h>

#include "IMotion.h"
#include "IMotion_csv.h"

struct
{
	IM_FORMAT format;
    uint8 *motion;		/* Pointer to wave data */
    uint32 motionlen;	/* Length of wave data */
    int motionpos;		/* Current play position */
} wave;

static int done = 0;
static int loop_count = 1;
void LoopCallback(void* context, uint32 state)
{
	if(state & IM_END_OF_LOOP) {
		fprintf(stderr, "\n*** Loop Counter : %d ***\n", loop_count);
		if(loop_count-- == 0)
			done = 1;
	}

	IMotion* handler = (IMotion*)context;
	// diagnostics
	int error = handler->GetInfo(NULL);
	if(error)
		fprintf(stderr, "Driver Error (code : %d) !\n", error);
}

int
main(int argc, char *argv[])
{
    /* Start up */	
	IMotion_Startup();
		
	/* Load motion file (.csv) */
    if (argv[1] == NULL) {
		argv[1] = "../../MotionData/waveform_sine.csv";	
    }

	if(IMotion_LoadCSV(argv[1], &wave.format, &wave.motion, &wave.motionlen, NULL) == NULL) {
		fprintf(stderr, "Couldn't load %s\n", argv[1]);
		IMotion_Shutdown();
		exit(2);
	}
	
    /* Show the list of available drivers */
	int device_id = 0;
#if 1
    fprintf(stderr, "Available motion devices: ");
	IM_DEVICE_DESC desc;
	memset(&desc, 0, sizeof(IM_DEVICE_DESC));

    for (int i = 0; i < IMotion_GetDeviceCount(); ++i) {
		IMotion_GetDeviceDescription(i, &desc);
        if (i == 0) {
			printf("%s", desc.szName);
        } else {
			printf(", %s", desc.szName);
        }
		if(!strcmp(desc.szName, "Inno Motion Seat")) {
			device_id = i;
		}
    }
#endif
	
	IMotion_GetDeviceDescription(device_id, &desc);
	desc.nOptions |= IM_CFG_DEBUG_MODE; // log
	fprintf(stderr, "\n=> Using motion device: %s\n\n", desc.szName);
	
	/* Create device */
	IMotion* motion = IMotion_Create(11, &desc);
	if(motion == NULL) {
        fprintf(stderr, "Couldn't open motion\n");
		IMotion_FreeCSV(wave.motion);
		IMotion_Shutdown();
		exit(1);
    }
	motion->Start();
	
    /* Create Source and Submit Buffer */
	IMotionSource* source = motion->CreateSource(&wave.format, LoopCallback, IM_END_OF_LOOP);

	IM_BUFFER buffer;	
	memset(&buffer, 0, sizeof(IM_BUFFER));	
	buffer.nMotionBytes = wave.motionlen;
	buffer.pMotionData = wave.motion;
#if 0 // buffer loop test
	buffer.nLoopCount = IM_LOOP_INFINITE;
#endif
	buffer.pContext = motion;
	source->SubmitBuffer(&buffer);	

    /* Let the motion run */
	source->Start();	
    while (!done && (source->GetQueuedBufferCount() > 0)) {		
        Sleep(100);
	}
	source->Stop();
	
    /* Clean up */	
    IMotion_FreeCSV(wave.motion);
	motion->DestroySource(source);	
	IMotion_Destroy(motion);
	IMotion_Shutdown();

    return (0);
}
