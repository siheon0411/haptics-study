/********************************************************************************//**
\file      IMotion.h
\brief     C Interface of IMotion (Motion Library for Passive VR).
\copyright Copyright (C) 2016-2019 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

#ifndef _IMOTION_H_
#define _IMOTION_H_

#include "IMotion_types.h"

#ifdef __cplusplus
#include "IMotion.hpp"
#else
typedef void IMotion;
typedef void IMotionSource;
typedef void IMotionFilter;
#endif

#ifdef __cplusplus
extern "C"{
#endif	
	
#ifndef IM_DRIVER_STATIC_LIB
#	ifdef IM_DRIVER_EXPORTS
#		define IM_DRIVER_DLL_API __declspec(dllexport)
#	else
#		define IM_DRIVER_DLL_API __declspec(dllimport)
#	endif
#else
#  define IM_DRIVER_DLL_API
#endif

/************************************
 * @section Motion Driver Interface 
 ************************************/

/**
 * This function initializes motion library.
 */
IM_DRIVER_DLL_API int32 IMotion_Startup(uint32 flags IMDEFAULT(0), uint32 param IMDEFAULT(0));

/**
 * This function cleans up all motion objects.
 */
IM_DRIVER_DLL_API int32 IMotion_Shutdown();

/**
 * This function gets the number of supported device types.
 */
IM_DRIVER_DLL_API int32 IMotion_GetDeviceCount();

/**
 * This function gets the specifications of all devices.
 */
IM_DRIVER_DLL_API int32 IMotion_GetDeviceDescription(int32 index, IM_DEVICE_DESC* desc IMDEFAULT(0));	

/**
 * This function gets the currently set device profile.
 */
IM_DRIVER_DLL_API int32 IMotion_GetDeviceProfile(uint32 devid IMDEFAULT(IM_DEVICE_ID_DEFAULT), IM_DEVICE_DESC* desc IMDEFAULT(0));	

/**
 * This function gets the number of supported filter types.
 */
IM_DRIVER_DLL_API int32 IMotion_GetFilterCount();

/**
 * This function gets the specifications of all filters.
 */
IM_DRIVER_DLL_API int32 IMotion_GetFilterDescription(int32 index, IM_FILTER_DESC* desc IMDEFAULT(0));	

/**
 * This function registers the log output callback function.
 */
IM_DRIVER_DLL_API int32 IMotion_SetLogFunction(IMotionDebugCallback callback, void *userdata IMDEFAULT(0));

/************************************
 * @section Motion Device Interface 
 ************************************/
/**
 * This function creates a motion device object and connects it to the device.
 */
IM_DRIVER_DLL_API IMotion* IMotion_Create(uint32 devid IMDEFAULT(IM_DEVICE_ID_DEFAULT), const IM_DEVICE_DESC* desc IMDEFAULT(0));

/**
 * This function disconnect the device and destroys the device object.
 */
IM_DRIVER_DLL_API int32 IMotion_Destroy(IMotion* device, uint32 flags IMDEFAULT(0));

/**
 * This function moves the device to the center position to start motion control.
 */
IM_DRIVER_DLL_API int32 IMotion_Start(IMotion* device, const IM_FORMAT* format IMDEFAULT(0), uint32 flags IMDEFAULT(IM_DEVICE_MOVE_DEFAULT), IMotion* shared_context IMDEFAULT(0));

/**
 * This function stops motion control and moves the device to its initial position.
 */
IM_DRIVER_DLL_API int32 IMotion_Stop(IMotion* device, uint32 flags IMDEFAULT(IM_DEVICE_MOVE_DEFAULT)); 

/**
 * This function send the position data directly to the device for manual motion control. 
 */
IM_DRIVER_DLL_API int32 IMotion_SendStream(IMotion* device, uint8* data, uint32 size, uint32 count IMDEFAULT(1)); 

/**
 * This function obtains diagnostic information about the motion device.
 */
IM_DRIVER_DLL_API int32 IMotion_GetInfo(IMotion* device, IM_DIAGNOSTIC_INFO* info IMDEFAULT(0));

/**
 * This function obtains detailed diagnostic information for each axis of the motion device.
 */
IM_DRIVER_DLL_API int32 IMotion_GetAxesInfo(IMotion* device, IM_DIAGNOSTIC_AXIS_INFO* info IMDEFAULT(0), uint32 max IMDEFAULT(1));

/**
 * This function sets the status information for each axis of the motion device.
 * (Only bCurrentOn, bServoOn, bDCCOn, bAlarmResetOn can be set) 
 */
IM_DRIVER_DLL_API int32 IMotion_SetAxesInfo(IMotion* device, const IM_DIAGNOSTIC_AXIS_INFO* info IMDEFAULT(0), uint32 max IMDEFAULT(1));

/**
 * This function gets the profile set on the current motion device. 
 */
IM_DRIVER_DLL_API int32 IMotion_GetProfile(IMotion* device, IM_DEVICE_DESC* desc IMDEFAULT(0)); 

/**
 * This function gets the format of the target motion device.
 */
IM_DRIVER_DLL_API int32 IMotion_GetFormat(IMotion* device, IM_FORMAT* format); 

/**
 * This function gets the ID of the current motion device. 
 */
IM_DRIVER_DLL_API int32 IMotion_GetID(IMotion* device); 

/************************************
 * @section Motion Source Interface 
 ************************************/

/**
 * This function creates a source object for playback of motion data.
 */
IM_DRIVER_DLL_API IMotionSource* IMotion_CreateSource(IMotion* device, const IM_FORMAT* format, 
	IMotionCallback user_func IMDEFAULT(0), uint32 cb_flags IMDEFAULT(0), uint32 mask IMDEFAULT(0), 
	uint32 samples IMDEFAULT(IM_FORMAT_SAMPLES_DEFAULT), uint32 buffers IMDEFAULT(IM_FORMAT_BUFFERS_DEFAULT)); 

/**
 * This function destroys the motion source object.
 */
IM_DRIVER_DLL_API int32 IMotion_DestroySource(IMotion* device, IMotionSource* source); 
	
/**
 * This function starts playing the motion data with an internal thread. 
 */
IM_DRIVER_DLL_API int32 IMotionSource_Start(IMotionSource* source, uint32 flags IMDEFAULT(0));

/**
 * This function removes the internal thread and stops playing the motion data.
 */
IM_DRIVER_DLL_API int32 IMotionSource_Stop(IMotionSource* source, uint32 flags IMDEFAULT(IM_FLUSH_FORCE));

/**
 * This function adds motion buffer to the playlist of motion source object.
 */
IM_DRIVER_DLL_API int32 IMotionSource_SubmitBuffer(IMotionSource* source, const IM_BUFFER* buffer);

/**
 * This function gets the number of playlists in the motion source object. 
 */
IM_DRIVER_DLL_API uint32 IMotionSource_GetQueuedBufferCount(IMotionSource* source);

/**
 * This function flush all the buffers in the playlist of the motion source object.
 */
IM_DRIVER_DLL_API int32 IMotionSource_Flush(IMotionSource* source, uint32 flags IMDEFAULT(IM_FLUSH_DONE));

/**
 * This function gets the motion device object from which the motion source object was created. 
 */
IM_DRIVER_DLL_API IMotion* IMotionSource_GetDevice(IMotionSource* source);

/**
 * This function gets the format information of the motion source object.
 */
IM_DRIVER_DLL_API int32 IMotionSource_GetFormat(IMotionSource* source, IM_FORMAT* format);

/**
 * This function gets the playback information of the motion source object.
 */
IM_DRIVER_DLL_API uint32 IMotionSource_GetInfo(IMotionSource* source, IM_BUFFER* buffer IMDEFAULT(0));

#ifdef __cplusplus
}
#endif

#endif // _IMOTION_H_
