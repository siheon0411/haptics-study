/********************************************************************************//**
\file      InnoML_Test_main_filter.cpp
\brief     Examples of source and input filtering using motion filters.
\copyright Copyright (C) 2018 InnoSimulation, Inc. All rights reserved. 
************************************************************************************/

#include <stdio.h>		// for printf
#include <windows.h>	// for sleep
#include <math.h>		// for sin
#include <InnoML.h>		// for motion

#pragma comment(lib, "winmm.lib")

typedef struct {
#if 0 // 3-DOF (Using the channel conversion filter)
	short	heave, roll, pitch;	/**< platform translation & rotation, mm & degree */ 
#else // 6-DOF (Internally channel conversion)
	short	surge, sway, heave;	/**< platform translation, mm */ 
	short	roll, pitch, yaw;	/**< platform rotation, degree */ 	
#endif
} PLATFORM_POSITION_MESSAGE;

typedef struct {	
	short	surge, sway, heave;	/**< acceleration, m/s^2 */ 
	short	roll, pitch, yaw;	/**< angular velocity, radians/s */ 
} FORCE_SIMULATION_MESSAGE;

#define PLATFORM_CHANNELS	(sizeof(PLATFORM_POSITION_MESSAGE)/sizeof(short))
#define FORCE_CHANNELS	(sizeof(FORCE_SIMULATION_MESSAGE)/sizeof(short))
#define SAMPLE_RATE		IM_FORMAT_SAMPLE_RATE_DEFAULT
#define SAMPLE_COUNT	1

static float simple_scaler_processor(void* context, void* data, int size, IM_FORMAT* src_format, const IM_FORMAT* dst_format)
{
	// 1. BUILD : For the filter build process, first specify the supported conversion results in the source format.
	if(src_format == NULL || dst_format == NULL)
		return 0;
	
	float cvt_ratio = 1.0f;
	if(data == 0 && size == 0) {
		// Where it initializes the custom context data and informs the support conversion format.
		return cvt_ratio; // Notify that it is a supported conversion format for the build process of the filter
	}
	
	// 2. CONVERT : It processes the actual user filter logic here.
	float factor = 1.0f;
	if(context)
		factor = *(float*)context;		
	int channels = src_format->nChannels;

#define scale_amplitude(type, min, max)	\
    {	\
        const type *src = (const type *) data;	\
        type *dst = (type *) data;	\
		int samples = size/(sizeof (type) * channels); \
		while(samples--) {	\
			for(int i=0; i<channels; i++) {	\
				(*dst) = MOTION_CLAMP((*src) * factor, min, max);	\
				dst++;	\
				src++;	\
			}	\
		}	\
	}

	switch (src_format->nDataFormat) {
    case IM_FORMAT_DATA_S16:
        scale_amplitude(int16, MOTION_MIN_VAL(16), MOTION_MAX_VAL(16));
        break;
    case IM_FORMAT_DATA_S32:
        scale_amplitude(int32, MOTION_MIN_VAL(32), MOTION_MAX_VAL(32));
        break;
    case IM_FORMAT_DATA_F32:
        scale_amplitude(float, -1, 1);
        break;
    }
#undef scale_amplitude
	
	return cvt_ratio;
}

static float channel_mask_processor(void* context, void* data, int size, IM_FORMAT* src_format, const IM_FORMAT* dst_format)
{
	// 1. BUILD : For the filter build process, first specify the supported conversion results in the source format.
	if(src_format == NULL || dst_format == NULL)
		return 0;
	if(src_format->nChannels != 6 || dst_format->nChannels != 3)
		return 0; // If the conversion format is not supported by the build process of the filter ...
	
	float cvt_ratio = dst_format->nChannels / (float)src_format->nChannels;
	if(data == 0 && size == 0) {
		// Where it initializes the custom context data and informs the support conversion format.
		src_format->nChannels = dst_format->nChannels;	
		return cvt_ratio; // Notify that it is a supported conversion format for the build process of the filter
	}
	
	// 2. CONVERT : It processes the actual user filter logic here.
	int32 mask = IM_BIT_MASK_DEFAULT;
	if(context)
		mask = *(int32*)context;
		
#define mask_chans_6_to_3(type)	\
    {	\
        const type *src = (const type *) data;	\
        type *dst = (type *) data;	\
		int samples = size/(sizeof (type) * 6); \
		while(samples--) {	\
			for(int i=0; i<6; i++) {	\
				if(mask & (1<<i)) {		\
					(*dst) = (*src);	\
					dst++;	\
				}	\
				src++;	\
			}	\
		}	\
	}

	switch (MOTION_SAMPLE_BIT(src_format->nDataFormat)) {
    case 8:
        mask_chans_6_to_3(int8);
        break;
    case 16:
        mask_chans_6_to_3(int16);
        break;
    case 32:
        mask_chans_6_to_3(int32);
        break;
    }
#undef mask_chans_6_to_3

	src_format->nChannels = dst_format->nChannels;	
	return cvt_ratio;
}

static int input_filter_callback(void* netstream, void* stream, int len)
{	
	IMFilter filter = *(IMFilter*)netstream;
	if(filter == 0)
		return 0;
	
	// Here you manually filter the output of the motion input. (filter convert)
	int size = imFilterProcess(filter, stream, len);

	// Here you can use math filters (washout, scaler, mapping algorithms, ...)
#if 0 
	PLATFORM_POSITION_MESSAGE* sample = (PLATFORM_POSITION_MESSAGE*)stream;
	for(int i=0; i<size/sizeof(PLATFORM_POSITION_MESSAGE); i++) {
		sample[i].heave *= 2;
	}
#endif
	return size;
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
		
	/**** Motion Filters Setup  ****/
	// motion rate limit per msec (default 256 : rate/32767)
	IMFilter platform_limiter = imCreateFilter(IM_FILTER_RATELIMIT);	
	// moving average count (default 4)
	IMFilter mean_filter = imCreateFilter(IM_FILTER_MEAN);
	// noise covariance constant (default 5, 0~100)
	IMFilter noise_filter = imCreateFilter(IM_FILTER_NOISE);
	// washout cutoff frequency (default 5 hz)
	IMFilter default_classical_washout = imCreateFilter(IM_FILTER_WASHOUT);
	// high-pass cutoff frequency (default 5 hz, 1st HPF)
	IMFilter highpass_filter = imCreateFilter(IM_FILTER_HIGHPASS);
	// low-pass cutoff frequency (0.2 hz, 1st LPF)
	IMFilter lowpass_filter = imCreateFilter(IM_FILTER_LOWPASS);
	IM_FILTER_LOWPASS_PARAMS lowpass_params[] = {0.2,1}; 
	imFilterSetParams(lowpass_filter, lowpass_params, sizeof(IM_FILTER_LOWPASS_PARAMS), 1);
	// scale for motion cueing
	IMFilter simple_scaler = imCreateFilter(IM_FILTER_SCALE);
	IM_FILTER_SCALE_PARAMS scaler_params[] = {20};
	imFilterSetParams(simple_scaler, scaler_params, sizeof(IM_FILTER_SCALE_PARAMS), 1);

	/**** Motion Cueing (Context Filter)  ****/
	// It is usually used to remove noise from mixed motion data or to convert it to a motion device specification.
	IMFilter master_filter = imCreateFilter();
	if(1) // context filter 
	{
	#if 0 // DOF channel mapping support in other motion platforms
		IMFilter custom_channel_mask = imCreateFilter(IM_FILTER_CUSTOM, channel_mask_processor, &desc.nMask);
		imFilterAppend(master_filter, custom_channel_mask);
	#endif
		imFilterAppend(master_filter, platform_limiter);	// prevent damage to the motion platform
		// Set output filter for the motion context	(master buffer)
		imSetFilter(master_filter);
	}

	imStart(); // master_filter build 

	/**** Direct Positioning (Effect Filter)  ****/
	// It is usually used to convert motion effect data more smoothly. (vibration, bumps, collision, ...)
	if(1) // buffer filter
	{	
		IMFilter filter = imCreateFilter();
		imFilterAppend(filter, noise_filter);	// Smoothing with noise filter
		//imFilterAppend(filter, mean_filter);	// Smoothing with average filter
						
		srand(timeGetTime());
		IMBuffer effect_buffer = imCreateBuffer(SAMPLE_RATE, IM_FORMAT_DATA_S16, PLATFORM_CHANNELS, SAMPLE_RATE);
		PLATFORM_POSITION_MESSAGE sample;
		memset(&sample, 0, sizeof(sample));
		for(int i=0; i<SAMPLE_RATE; i++) { // 1 sec samples
			short noise = (rand()&(0x100-1)) - (0x100>>1); // test noise (-128~127)
			float bump = sin(2 * IM_PI * (float)i/SAMPLE_RATE); // sampling (-1~1)
			sample.heave = bump * MOTION_MAX_16;		// quantizing (16 bit)
			sample.heave = MOTION_CLAMP_16(sample.heave + noise);
			imBufferEnqueue(effect_buffer, &sample, sizeof(sample)); // encoding (pcm buffer)
		}		
		int duration = imBufferGetDuration(effect_buffer); // duration (1 sec)

		// The result of filtering the source buffer is stored directly in the destination buffer.
		IMBuffer cvt_buffer = 0;
	#if 1
		imBufferConvert(effect_buffer, &cvt_buffer, master_buffer, filter);	// build & convert
	#else
		cvt_buffer = imCreateBuffer(SAMPLE_RATE, IM_FORMAT_DATA_S16, PLATFORM_CHANNELS, SAMPLE_RATE);	
		imFilterBuild(filter, effect_buffer, cvt_buffer);
		// Here you manually filter the output of the motion input. (filter convert)
		imFilterProcess(filter);
	#endif

		IMSource source = imCreateSource(cvt_buffer);	
		imSourcePlay(source);
		while(imSourceGetPosition(source) < duration) Sleep(10);
		imSourceStop(source);

		imDeleteSource(source);
		imDeleteBuffer(cvt_buffer);
		imDeleteBuffer(effect_buffer);
		imDeleteFilter(filter);
		fprintf(stderr, "Motion Buffer filtering completed ... \n\n");	
	}
	if(1) // source filter
	{				
		IMFilter filter = imCreateFilter();
		imFilterAppend(filter, highpass_filter);
		//imFilterAppend(filter, lowpass_filter);
		
		const char* url = "../../MotionData/waveform_sine.csv";				
		IMBuffer effect_buffer = imLoadBuffer(url);		// load url	
		int duration = imBufferGetDuration(effect_buffer); // duration (1 sec)
		IMSource source = imCreateSource(effect_buffer);
		
		// Set output filter for the motion source
		imSourceSetFilter(source, filter);		
		imSourcePlay(source); // filter build
		while(imSourceGetPosition(source) < duration) Sleep(10);
		imSourceStop(source);

		imDeleteSource(source);
		imDeleteBuffer(effect_buffer);
		imDeleteFilter(filter);
		fprintf(stderr, "Motion Source filtering completed ... \n\n");	
	}	

	/**** Force Simulation (Washout Filter) ****/
	// It is usually used to convert logical motion data to physical motion data.
	// The washout filter requires a buffer of type IM_FORMAT_TYPE_FORCE. (force -> motion)
	IMBuffer input_buffer = imCreateBuffer(SAMPLE_RATE, IM_FORMAT_DATA_S16, FORCE_CHANNELS, SAMPLE_COUNT, IM_FORMAT_BUFFERS_DEFAULT, IM_FORMAT_TYPE_DOF);
	IMInput input = imCreateInput(input_buffer);

	if(1) // input filter (default washout)
	{
		IMFilter filter = imCreateFilter();
		imFilterAppend(filter, noise_filter);
		imFilterAppend(filter, default_classical_washout);
		imFilterAppend(filter, simple_scaler);	
		imFilterAppend(filter, platform_limiter);
		
		// Set output filter for the motion input
		imInputSetFilter(input, filter);
		imInputStart(input); // filter build
		FORCE_SIMULATION_MESSAGE sample;	
		memset(&sample, 0, sizeof(FORCE_SIMULATION_MESSAGE));
		unsigned int dt = 1000/SAMPLE_RATE;
		for(int time=0; time<=3000; time+=dt) { // 3 sec washout
			// Press and hold the accelerator for 1 second.
			float acceleration = (time < 1000) ? 1 : 0;	// sampling (-1~1)
			sample.surge = acceleration * MOTION_MAX_16;	// quantizing (16 bit)
			imBufferEnqueue(input_buffer, &sample, sizeof(FORCE_SIMULATION_MESSAGE)); // encoding (pcm buffer)
			// The real-time sampling time is ideal for signals that are less than half the device sample time.
			Sleep((1000/SAMPLE_RATE)>>1);
		}
		imInputStop(input);	

		imDeleteFilter(filter);
		fprintf(stderr, "Motion Input filtering completed ... \n\n");		
	}
	if(1) // custom filter (custom washout)
	{
		// Setup a custom filter by rebuilding a built-in filter
		IMFilter custom_classical_washout = imCreateFilter();
		{			
		// 3rd HPF (heave acceleration), 2nd HPF (roll/pitch velocity)
		IMFilter highpass_filter = imCreateFilter(IM_FILTER_HIGHPASS);
		IM_FILTER_HIGHPASS_PARAMS highpass_params[] = {{0},{0},{5,3},{5,2},{5,2},{0}};
		imFilterSetParams(highpass_filter, highpass_params, sizeof(IM_FILTER_HIGHPASS_PARAMS), 6);		
		// 2nd integral (heave position), 1st integral (roll/pitch angle)
		IMFilter integrator = imCreateFilter(IM_FILTER_INTEGRAL);
		IM_FILTER_INTEGRAL_PARAMS integrator_params[] = {0,0,2,1,1,0}; 
		imFilterSetParams(integrator, integrator_params, sizeof(IM_FILTER_INTEGRAL_PARAMS), 6);	
		// 2nd LPF (surge/sway acceleration)
		IMFilter lowpass_filter = imCreateFilter(IM_FILTER_LOWPASS);
		IM_FILTER_LOWPASS_PARAMS lowpass_params[] = {{5,2},{5,2},{0},{0},{0},{0}};
		imFilterSetParams(lowpass_filter, lowpass_params, sizeof(IM_FILTER_LOWPASS_PARAMS), 6);
		// tilt-coordination (add special force to pitch/roll)
		IMFilter tilt_coordinator = imCreateFilter(IM_FILTER_TILT);

		// simulate cues of initial acceleration (translational and rotational)
		imFilterAppend(custom_classical_washout, highpass_filter);	
		imFilterAppend(custom_classical_washout, integrator);		
		// simulate sustaining accelerations (G-Force)
		imFilterAppend(custom_classical_washout, lowpass_filter);	
		imFilterAppend(custom_classical_washout, tilt_coordinator);	
		}

		// Setup a custom function that can use third-party libraries	
		float scale_factor = 20;
		IMFilter custom_scaler = imCreateFilter(IM_FILTER_CUSTOM, simple_scaler_processor, &scale_factor);
	
		IMFilter filter = imCreateFilter();
		imFilterAppend(filter, noise_filter);
		imFilterAppend(filter, custom_classical_washout); // cf) default_classical_washout
		imFilterAppend(filter, custom_scaler);
		imFilterAppend(filter, platform_limiter);
				
	#if 1 // Set convert filter and buffer for input callback
		imFilterBuild(filter, input_buffer, master_buffer);
		imInputStart(input, input_filter_callback, &filter);	
	#else		
		imInputSetFilter(input, filter);
		imInputStart(input);
	#endif
	
		FORCE_SIMULATION_MESSAGE sample;	
		memset(&sample, 0, sizeof(FORCE_SIMULATION_MESSAGE));
		unsigned int dt = 1000/SAMPLE_RATE;
		for(int time=0; time<=3000; time+=dt) { // 3 sec washout
			float acceleration = (time < 1000) ? 127 : 0;	// sampling (-127~127)
			sample.surge = acceleration * 256;	// quantizing (16 bit)
			imInputSendStream(input, &sample, sizeof(FORCE_SIMULATION_MESSAGE)); // encoding (pcm buffer)
			Sleep((1000/SAMPLE_RATE)>>1);
		}
		imInputStop(input);	

		imDeleteFilter(filter);
		fprintf(stderr, "Custom filter & Input callback completed ... \n\n");		
	}		
	imDeleteBuffer(input_buffer);
	imDeleteInput(input);

    /* Clean up */
	imStop(); // stop motion streaming (move init position)
	imDeleteFilter(master_filter);
	imDeleteBuffer(master_buffer);
	imSetContext(NULL); // release context
	imDestroyContext(context); // shutdown device	
	return 0;
}
