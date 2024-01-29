/********************************************************************************//**
\file      IMotion.hpp
\brief     C++ Interface of IMotion (Motion Library for Passive VR).
\copyright Copyright (C) 2016-2019 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

#ifndef _IMOTION_HPP_
#define _IMOTION_HPP_

#include "IMotion_types.h"

#ifndef IM_DRIVER_STATIC_LIB
#	ifdef IM_DRIVER_EXPORTS
#		define IM_DRIVER_DLL_API __declspec(dllexport)
#	else
#		define IM_DRIVER_DLL_API __declspec(dllimport)
#	endif
#else
#  define IM_DRIVER_DLL_API
#endif
	
class IMotionSource;
class IMotionFilter;

/**
 * Motion device C++ interface 
 */
class IMotion 
{
public:
	IMotion(uint32 id) { m_id = id; }	/**< Refer to IMotion_Create() details. */
	virtual ~IMotion() {}				/**< Refer to IMotion_Destroy() details. */	
	virtual int32 Start(const IM_FORMAT* format=0, uint32 flags=IM_DEVICE_MOVE_DEFAULT, IMotion* shared_context=0) { return 0; }/**< Refer to IMotion_Start() details. */		
	virtual int32 Stop(uint32 flags=IM_DEVICE_MOVE_DEFAULT) { return 0; }				/**< Refer to IMotion_Stop() details. */	
	virtual int32 SendStream(uint8* data, uint32 size, uint32 count=1) = 0;				/**< Refer to IMotion_SendStream() details. */	
	virtual int32 GetInfo(IM_DIAGNOSTIC_INFO* info=0) = 0;								/**< Refer to IMotion_GetInfo() details. */	
	virtual int32 GetAxesInfo(IM_DIAGNOSTIC_AXIS_INFO* info=0, uint32 max=1) = 0;		/**< Refer to IMotion_GetAxesInfo() details. */
	virtual int32 SetAxesInfo(const IM_DIAGNOSTIC_AXIS_INFO* info=0, uint32 max=1) = 0;	/**< Refer to IMotion_SetAxesInfo() details. */	
	virtual uint32 GetID() { return m_id; }							/**< Refer to IMotion_GetID() details. */ 
	virtual int32 GetProfile(IM_DEVICE_DESC* desc=0);				/**< Refer to IMotion_GetProfile() details. */	
	virtual int32 GetFormat(IM_FORMAT* format);						/**< Refer to IMotion_GetFormat() details. */
	virtual IMotionSource* CreateSource(const IM_FORMAT* format, IMotionCallback user_func=0, uint32 cb_flags=0, uint32 mask=0, 
		uint32 samples=IM_FORMAT_SAMPLES_DEFAULT, uint32 buffers=IM_FORMAT_BUFFERS_DEFAULT) = 0;	/**< Refer to IMotion_CreateSource() details. */				
	virtual int32 DestroySource(IMotionSource* source) = 0;			/**< Refer to IMotion_DestroySource() details. */	
	virtual IMotionFilter* CreateFilter(const IM_FILTER_DESC* desc=0, IM_FILTER_TYPE type=IM_FILTER_DEFAULT, 
		IMotionFilterCallback processor=0, const void* context=0) = 0;			/**< Refer to IMotion_CreateFilter() details. */	
	virtual int32 DestroyFilter(IMotionFilter* filter) = 0;			/**< Refer to IMotion_DestroyFilter() details. */	

protected:
	uint32			m_id;
	IM_DEVICE_DESC	m_profile;
	IM_FORMAT		m_format;
};

/**
 * Motion source C++ interface. 
 */
class IMotionSource 
{
public:
	IMotionSource(IMotion* device, const IM_FORMAT* format);/**< Refer to IMotion_CreateSource() details. */
	virtual ~IMotionSource() {}								/**< Refer to IMotion_DestroySource() details. */
	virtual int32 Start(uint32 flags=0) = 0;				/**< Refer to IMotionSource_Start() details. */
	virtual int32 Stop(uint32 flags=IM_FLUSH_FORCE) = 0;	/**< Refer to IMotionSource_Stop() details. */
	virtual int32 SubmitBuffer(const IM_BUFFER* buffer) = 0;/**< Refer to IMotionSource_SubmitBuffer() details. */
	virtual uint32 GetQueuedBufferCount() = 0;				/**< Refer to IMotionSource_GetQueuedBufferCount() details. */
	virtual int32 Flush(uint32 flags=IM_FLUSH_DONE) = 0;	/**< Refer to IMotionSource_Flush() details. */
	virtual IMotion* GetDevice() { return m_device; }		/**< Refer to IMotionSource_GetDevice() details. */
	virtual int32 GetFormat(IM_FORMAT* format);				/**< Refer to IMotionSource_GetFormat() details. */
	virtual uint32 GetInfo(IM_BUFFER* buffer=0) = 0;		/**< Refer to IMotionSource_GetInfo() details. */

protected:
	IMotion*	m_device;
	IM_FORMAT	m_format;
};

#endif // _IMOTION_HPP_