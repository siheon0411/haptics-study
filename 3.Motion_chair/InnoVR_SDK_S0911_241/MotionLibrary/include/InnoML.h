/********************************************************************************//**
\file      InnoML.h
\brief     C Interface of InnoML (Motion Library for Interactive VR).
\copyright Copyright (C) 2016-2019 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

#ifndef INNO_ML_H
#define INNO_ML_H

#include "IMotion_types.h"

#ifdef __cplusplus
extern "C"{
#endif
	
#ifndef INNO_ML_STATIC_BUILD
#	ifdef INNO_ML_EXPORTS
#		define IM_API __declspec(dllexport)
#	else
#		define IM_API __declspec(dllimport)
#	endif
#else
#  define IM_API
#endif	

/** Declare motion context object type */  
typedef int32		IMContext;
/** Declare motion buffer object type */ 
typedef int32		IMBuffer;
/** Declare motion source object type */  
typedef int32		IMSource;
/** Declare motion input object type */
typedef int32		IMInput;
/** Declare motion filter object type */
typedef int32		IMFilter;

/************************************
 * @section IMContext (Motion Device Context) 
 ************************************/
/**
 * This function creates a motion device context and connects it to the device.
 */
IM_API IMContext	imCreateContext(
	IMBuffer	buffer			IMDEFAULT(0),	/* master buffer for motion device (need only motion format) */
	uint32		id				IMDEFAULT(0),	/* number of motion device connect ip (1~255) */
	const IM_DEVICE_DESC* desc	IMDEFAULT(0)	/* description for creating a motion device (name, version, options) */	
	);

/**
 * This function switches the specified context object to the active context.
 */
IM_API int32		imSetContext(IMContext ctx, uint32 flags IMDEFAULT(0));

/**
 * This function gets the currently active context.
 */
IM_API IMContext	imGetContext();

/**
 * This function gets the master buffer of the active context.
 */
IM_API IMBuffer		imGetBuffer();
	
/**
 * This function sets the filter to convert the output of the motion source.
 */
IM_API int32		imSetFilter(IMFilter filter);

/**
 * This function gets the motion filter set for the current motion source.
 */
IM_API IMFilter		imGetFilter();

/**
 * This function starts motion streaming by moving the device to the center position.
 */
IM_API int32		imStart(IMotionInputCallback callback IMDEFAULT(0), const void* streamer_obj IMDEFAULT(0), IMContext shared_context IMDEFAULT(0), uint32 flags IMDEFAULT(IM_DEVICE_MOVE_DEFAULT));

/**
 * This function stops motion streaming and moves the device to its initial position.
 */
IM_API int32		imStop(uint32 flags IMDEFAULT(IM_DEVICE_MOVE_DEFAULT));

/**
 * This function gets the number of supported device types.
 */
IM_API int32		imGetDescriptionCount();

/**
 * This function gets the specifications of all devices.
 */
IM_API int32		imGetDescription(IM_DEVICE_DESC* desc IMDEFAULT(0), int32 count IMDEFAULT(1));

/**
 * This function gets the profile set in the active motion context. 
 */
IM_API int32		imGetProfile(IM_DEVICE_DESC* desc IMDEFAULT(0), uint32 devid IMDEFAULT(0)); 
	
/**
 * This function obtains detailed diagnostic information for each axis of the active motion context.
 */
IM_API int32		imGetDiagnostic(IM_DIAGNOSTIC_AXIS_INFO* axis IMDEFAULT(0), int32 count IMDEFAULT(1));

/**
 * This function gets the number of motion sources that are currently playing.
 */
IM_API int32		imGetPlayingSourceCount();

/**
 * This function sets the volume that controls the amplitude of the master buffer.
 */
IM_API int32		imSetMasterVolume(int32 volume);

/**
 * This function stops all sources being played.
 */
IM_API int32		imStopAllSources();

/**
 * This function registers the log output function.
 * (Note, you need to use IM_CFG_DEBUG_MODE to output the device run log.)
 */
IM_API int32		imSetLogFunction(IMotionDebugCallback callback, void *userdata IMDEFAULT(0));

/**
 * This function disconnect the device and destroys the device context.
 */
IM_API int32		imDestroyContext(IMContext ctx, uint32 flags IMDEFAULT(0));

/************************************
 * @section IMBuffer (Motion Buffer) 
 ************************************/
/**
 * This function creates a motion buffer object for managing motion data.
 */
IM_API IMBuffer		imCreateBuffer(
	int32	sample_rate		IMDEFAULT(0),	/* motion samples per second (50 ~ 1000) */
	int32	format			IMDEFAULT(0),	/* motion data format (IM_FORMAT_DATA_*) */
	int32	channels		IMDEFAULT(0),	/* number of channels (1~8) */
	int32	samples			IMDEFAULT(0),	/* motion sample count (power of 2) */
	int32	buffers			IMDEFAULT(0),	/* motion buffer count (most 1 will be used) */
	int32	type			IMDEFAULT(0)	/* motion format type (most IM_FORMAT_TYPE_DOF will be used) */
	);

/**
 * This function creates a motion buffer object for motion data management from the format structure.
 */
IM_API IMBuffer		imCreateBufferFromFormat(const IM_FORMAT* format IMDEFAULT(0), int32 samples IMDEFAULT(0), int32 buffers IMDEFAULT(0));

/**
 * This function creates a motion buffer object from the motion file name.
 */
IM_API IMBuffer		imLoadBuffer(const char* url, const char* key IMDEFAULT(0));

/**
 * This function creates a motion buffer object from the motion file memory.
 */
IM_API IMBuffer		imLoadBufferMemory(const void* data, int32 size, const char* key IMDEFAULT(0));

/**
 * This function creates a new motion buffer object that is converted to the desired buffer format.
 */
IM_API float		imBufferConvert(IMBuffer buffer, IMBuffer* adjusted_buffer, IMBuffer desired_buffer IMDEFAULT(0), IMFilter filter IMDEFAULT(0));

/**
 * This function adds motion data to the motion buffer object.
 */
IM_API int32		imBufferEnqueue(IMBuffer buffer, const void* data, int32 size);

/**
 * This function removes motion data from the motion buffer object.
 */
IM_API int32		imBufferDequeue(IMBuffer buffer, void* data, int32 size);

/**
 * This function lock motion buffer for direct memory access.
 * (This function directly accesses the memory to save the cost of copying the enqueue or dequeue.)
 */
IM_API void*		imBufferLock(IMBuffer buffer, int32 size IMDEFAULT(0), uint32 flags IMDEFAULT(0));

/**
 * This function unlocks the locked motion buffer.
 * (The motion buffer that is locked with imBufferLock can be used after unlocking with this function.)
 */
IM_API int32		imBufferUnlock(IMBuffer buffer);

/**
 * This function specifies the motion buffer to be shared.
 * (Internally, new data that is filled in shared buffer is also queued to buffer.)
 */
IM_API int32		imBufferSetSharedBuffer(IMBuffer buffer, IMBuffer shared_buffer);

/**
 * This function gets the motion data byte size.
 */
IM_API int32		imBufferGetSize(IMBuffer buffer);

/**
 * This function gets the duration time of the motion data.
 */
IM_API int32		imBufferGetDuration(IMBuffer buffer);

/**
 * This function gets the format information of the motion data.
 */
IM_API int32		imBufferGetFormat(IMBuffer buffer, int32* sample_rate, int32* format, int32* channels, int32* samples);

/**
 * This function gets the format information of the motion data.
 */
IM_API int32		imBufferGetInfo(IMBuffer buffer, IM_FORMAT* format, int32* samples IMDEFAULT(0), int32* buffers IMDEFAULT(0));

/**
 * This function gets the number of motion samples stored in the motion data.
 */
IM_API int32		imBufferGetQueuedCount(IMBuffer buffer);

/**
 * This function releases the motion data memory and deletes the motion buffer object.
 * (Internally, the reference counter is used)
 */
IM_API int32		imDeleteBuffer(IMBuffer buffer);

/************************************
 * @section IMSource (Motion Source) 
 ************************************/
/**
 * This function creates a motion source object for playback of motion data.
 */
IM_API IMSource		imCreateSource(IMBuffer buffer IMDEFAULT(0));

/**
 * This function sets the motion buffer to be played by the motion source.
 */
IM_API IMBuffer		imSourceSetBuffer(IMSource source, IMBuffer buffer);

/**
 * This function obtains the motion buffer set in the current motion source.
 */
IM_API IMBuffer		imSourceGetBuffer(IMSource source);

/**
 * This function sets the filter to convert the output of the motion source.
 */
IM_API int32		imSourceSetFilter(IMSource source, IMFilter filter);

/**
 * This function gets the motion filter set for the current motion source.
 */
IM_API IMFilter		imSourceGetFilter(IMSource source);

/**
 * This function starts playback by adding this motion source to the playlist in the active context.
 */
IM_API int32		imSourcePlay(IMSource source, int32 loop_count IMDEFAULT(0), IMotionCallback listener_func IMDEFAULT(0), const void* listener_obj IMDEFAULT(0));

/**
 * This function stops playback by removing this motion source from the playlist in the active context.
 */
IM_API int32		imSourceStop(IMSource source);

/**
 * This function pauses or resumes playback of the motion source.
 */
IM_API int32		imSourcePause(IMSource source, int32 paused);

/**
 * This function sets the volume that controls the amplitude of this motion source.
 * (Range [0..100])
 */
IM_API int32		imSourceSetVolume(IMSource source, int32 volume);

/**
 * This function sets the playback speed of this motion source.
 * (Range [25 | 50 | 100 | 200 | 400])
 */
IM_API int32		imSourceSetSpeed(IMSource source, int32 speed);

/**
 * This function gets the current play time of the motion source.
 * (See also imBufferGetDuration)
 */
IM_API int32		imSourceGetPosition(IMSource source);

/**
 * This function releases the motion buffer and deletes the motion source object.
 * (Internally, the reference counter is used)
 */
IM_API int32		imDeleteSource(IMSource source);

/************************************
 * @section IMInput (Motion Input)
 ************************************/
/**
 * This function creates a motion input object that streams motion data at runtime.
 */
IM_API IMInput		imCreateInput(IMBuffer buffer IMDEFAULT(0));

/**
 * This function sets the motion buffer to be recorded by the motion input.
 * (Note, The input buffer must be greater than or equal to the master buffer size.)
 */
IM_API IMBuffer		imInputSetBuffer(IMInput input, IMBuffer buffer);

/**
 * This function obtains the motion buffer set in the current motion input.
 */
IM_API IMBuffer		imInputGetBuffer(IMInput input);

/**
 * This function sets the filter to convert the output of the motion input.
 */
IM_API int32		imInputSetFilter(IMInput input, IMFilter filter);

/**
 * This function gets the motion filter set for the current motion input.
 */
IM_API IMFilter		imInputGetFilter(IMInput input);

/**
 * This function starts streaming by adding a motion input object to the input list of the active context.
 */
IM_API int32		imInputStart(IMInput input, IMotionInputCallback callback IMDEFAULT(0), const void* streamer_obj IMDEFAULT(0));

/**
 * This function removes this motion input object from the active context input list and stops streaming.
 */
IM_API int32		imInputStop(IMInput input);

/**
 * This function records motion input data that requires sampling in real time.
 * (Note, use imBufferEnqueue to record the already sampled motion data)
 */
IM_API int32		imInputSendStream(IMInput input, const void* data, int32 size);

/**
 * This function releases the motion buffer and deletes the motion input object.
 * (Internally, the reference counter is used) 
 */
IM_API int32		imDeleteInput(IMInput input);	

/************************************
 * @section IMFilter (Motion Filter)
 ************************************/
/**
 * This function creates a motion filter object for filtering motion data.
 * (Note, possible to create custom filter with filter processor and parameters in CUSTOM type)
 */
IM_API IMFilter		imCreateFilter(
	IM_FILTER_TYPE			type			IMDEFAULT(IM_FILTER_DEFAULT),	/* filter type */
	IMotionFilterCallback	processor		IMDEFAULT(0),	/* custom filter processor */
	const void*				processor_obj	IMDEFAULT(0)	/* custom filter params */
	);

/**
 * This function sets the filter parameters in the filter object.
 * (Note, use built-in filter parameters that matches the type of motion filter created)
 */
IM_API int32		imFilterSetParams(
	IMFilter	filter,					/* filter object */
	const void* data	IMDEFAULT(0),	/* filter params data */
	int32		size	IMDEFAULT(0),	/* filter params size */
	int32		count	IMDEFAULT(0)	/* filter params count (~IM_FORMAT_CHANNELS_MAX) */
	);

/**
 * This function gets the filter parameters in the filter object.
 * (Note, use built-in filter parameters that matches the type of motion filter created)
 */
IM_API int32		imFilterGetParams(
	IMFilter	filter,					/* filter object */
	void*		data	IMDEFAULT(0),	/* filter params data */
	int32		size	IMDEFAULT(0),	/* filter params size */
	int32		count	IMDEFAULT(0)	/* filter params count (~IM_FORMAT_CHANNELS_MAX) */
	);

/**
 * This function adds filter object to the filter group object.
 */
IM_API int32		imFilterAppend(IMFilter filter, IMFilter child);

/**
 * This function removes filter object from the filter group object.
 */
IM_API int32		imFilterRemove(IMFilter filter, IMFilter child);

/**
 * This function performs a build operation that uses the specified filter to directly convert the source buffer to the destination buffer.
 * (You must call this function once before converting it to the imFilterProcess function.) 
 * (Note, if a filter is set on the motion source and input, it is used internally, so you do not need to call this function directly.)
 */
IM_API float		imFilterBuild(IMFilter filter, IMBuffer src_buffer, IMBuffer dst_buffer IMDEFAULT(0));

/**
 * This function converts the source buffer directly using the specified filter and stores it in the destination buffer.
 * (The conversion result is stored in data as much as the size. If data is NULL, it is queued in the destination buffer.) 
 * (Note, if a filter is set on the motion source and input, it is used internally, so you do not need to call this function directly.)
 */
IM_API int32		imFilterProcess(IMFilter filter, void* data IMDEFAULT(0), int32 size IMDEFAULT(0));

/**
 * This function releases the motion buffer and deletes the motion filter object.
 * (Internally, the reference counter is used) 
 */
IM_API int32		imDeleteFilter(IMFilter filter);

#ifdef __cplusplus
}
#endif

#endif // INNO_ML_H
