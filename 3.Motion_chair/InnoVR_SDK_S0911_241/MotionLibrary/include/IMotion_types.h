/********************************************************************************//**
\file      IMotion_types.h
\brief     This header provides the common type declarations for the InnoVR SDK.
\copyright Copyright (C) 2016-2019 InnoSimulation Co., Ltd. All rights reserved. 
************************************************************************************/

#ifndef _IMOTION_TYPES_H_
#define _IMOTION_TYPES_H_

#ifdef __cplusplus
extern "C"{
#endif
	
#ifdef __cplusplus
#	define IMDEFAULT(x) =x
#else
#	define IMDEFAULT(x)
#endif
		
/**
 *  \name IM_VERSION_*
 *
 *   Declare library version
 */
#define IM_VERSION_MAJOR 1
#define IM_VERSION_MINOR 3
#define IM_VERSION_PATCH 0

#define IM_STR_IMPL(x) #x
#define IM_STR(x) IM_STR_IMPL(x)
#define IM_VERSIONNUM(X, Y, Z) ((X)*1000 + (Y)*100 + (Z))	
#define IM_VERSION_NUMBER IM_VERSIONNUM(IM_VERSION_MAJOR, IM_VERSION_MINOR, IM_VERSION_PATCH)
#define IM_VERSION_STRING IM_STR(IM_VERSION_MAJOR) "." IM_STR(IM_VERSION_MINOR) "." IM_STR(IM_VERSION_PATCH)

/**
 *  \name common types
 *
 *  Declare common types 
 */
typedef unsigned char	uint8;
typedef char			int8;
typedef unsigned short	uint16;
typedef short int		int16;
typedef unsigned int	uint32;
typedef int				int32;
typedef unsigned __int64 uint64;
typedef __int64			int64;

/**
 *  \name common macro
 *
 *  Declare common macro that is frequently used
 */
#define IM_STRING_MAX	256
#define IM_PI			3.14159265358979323846
#define IM_MAXSHORT		0x7FFF
#define IM_MAXINT		0x7FFFFFFF
#define IM_MAX_ENUM		0x7FFFFFFF

#define MOTION_MASK_BITSIZE       (0xFF)
#define MOTION_MASK_DATATYPE      (1<<8)

#define MOTION_SAMPLE_BIT(fmt)	(fmt & MOTION_MASK_BITSIZE)
#define MOTION_SAMPLE_BYTE(fmt)	(MOTION_SAMPLE_BIT(fmt) >> 3)
#define MOTION_ISFLOAT(fmt)     (fmt & MOTION_MASK_DATATYPE)

#define MOTION_MAX_VAL(bits)	((1 << (bits - 1)) - 1)
#define MOTION_MAX_16			MOTION_MAX_VAL(16)

#define MOTION_MIN_VAL(bits)	-(1 << (bits - 1))
#define MOTION_MIN_16			MOTION_MIN_VAL(16)

#define MOTION_MIN(a,b)			((a) < (b) ?  (a) : (b))
#define MOTION_MAX(a,b)			((a) > (b) ?  (a) : (b))
#define MOTION_CLAMP(val,min,max)	MOTION_MAX(min, MOTION_MIN(val,max))
#define MOTION_CLAMP_BITS(val, bits)	MOTION_CLAMP(val, MOTION_MIN_VAL(bits), MOTION_MAX_VAL(bits))
#define MOTION_CLAMP_16(val)	MOTION_CLAMP(val, MOTION_MIN_16, MOTION_MAX_16)

#define MOTION_BITCOUNT(x)    (((BX_(x)+(BX_(x)>>4)) & 0x0F0F0F0F) % 255)
#define BX_(x)         ((x) - (((x)>>1)&0x77777777) \
                             - (((x)>>2)&0x33333333) \
                             - (((x)>>3)&0x11111111))
	
#define MOTION_INT_U2S(val, bits)	((val & (1<<(bits-1))) ? -((~val & ((1<<bits)-1))+1) : val)
#define MOTION_INT_S2U(val, bits)	((val < 0) ? (val + (1<<bits)) : val)

/**
 *  \name IM_ERROR_*
 *
 *  Device error status declaration
 *  (Note, This enumerator is used as the return value of the diagnostic function.)
 */
typedef enum {
	IM_DISCONNECTED =-1,/**< Disconnected State */
	IM_OK,				/**< Normal State */	
	IM_ERROR_DRIVER,	/**< Device Driver Error */	
	IM_ERROR_EMER,		/**< Emergency Error */	
	IM_ERROR_ALARM,		/**< Alarm Error */	
	IM_ERROR_SERVO,		/**< Servo Off */	
	IM_ERROR_COUNT,		
} IM_ERROR;

/**
 *  \name IM_CFG_*
 *
 *  Declare driver option (LSB:16)
 *  (Used in IM_DEVICE_DESC options and profiles.) 
 */
#define IM_CFG_DEBUG_MODE		0x1	// console log
#define IM_CFG_FILE_LOG			0x2	// check "IMotion.log"
#define IM_CFG_EMUL_MODE		0x4	// reserved
#define IM_CFG_ASYNC_MODE		0x8	// asynchronous device running mode (check "bBusy")
#define IM_CFG_FORCE_PROFILE	0x10// force device settings to profile (check "IMotion.ini")

/**
 *  \name IM_CFG_DEVICE_*
 *
 *  Declare device option (MSB:16)
 *  (Used only for profiles "IMotion.ini")
 */
#define IM_CFG_DEVICE_ZERO_INIT			0x10000	// reserved
#define IM_CFG_DEVICE_WEIGHT_EST		0x20000	// reserved
#define IM_CFG_DEVICE_INC_ENCODER		0x40000	// reserved
#define IM_CFG_DEVICE_ZERO_INIT_NOSLEEP	0x80000 // reserved
#define IM_CFG_DEVICE_READ_ALARM		0x100000 // reserved
#define IM_CFG_DEVICE_SET_IPADDRESS		0x200000 // reserved
#define IM_CFG_DEVICE_THREAD_UNSAFE		0x400000 // reserved

/**
 *  \name IM_DEVICE_*
 *
 *  Declare motion device connection and disconnection options
 *  (Used in IMotion_Create, IMotion_Destroy, imCreateContext, imDestroyContext flags.)
 */
#define IM_DEVICE_ID_DEFAULT		11	/**< Default device IP */
#define IM_DEVICE_ID_MAX			255	/**< 1 ~ 255 */
#define IM_DEVICE_MOVE_NONE			0x0 /**< Used in IMotion_Start/Stop or imStart/Stop flags */
#define IM_DEVICE_MOVE_SETTLE		0x1 /**< Used in IMotion_Start/Stop or imStart/Stop flags */
#define IM_DEVICE_MOVE_NEUTRAL		0x2 /**< Used in IMotion_Start/Stop or imStart/Stop flags */
#define IM_DEVICE_MOVE_DEFAULT		0x3 /**< Settle & Neutral : Start(S->N), Stop(N->S) */
#define IM_DEVICE_MOVE_PAUSE		0x4 /**< Reserved */
#define IM_DEVICE_MOTOR_POWER_OFF	0x10 /**< Used in IMotion_Destroy flags */

/**
 *  \name IM_BIT_*
 *
 *  Motion channel bit mask declaration
 *  (Used in IM_DEVICE_DESC mask.)
 */
typedef enum {
	IM_DOF_SURGE =0,/**< Rear-Front movement */
	IM_DOF_SWAY,	/**< Left-Right movement */
	IM_DOF_HEAVE,	/**< Down-Up movement */
	IM_DOF_ROLL,	/**< Left-Right rotation */
	IM_DOF_PITCH,	/**< Rear-Front rotation */
	IM_DOF_YAW,		/**< Horizontal Left-Right rotation */
	IM_DOF_COUNT
} IM_DOF_TYPE;

#define IM_BIT_DOF(dof_type)	(1<<dof_type)

#define IM_BIT_DOF_SURGE	IM_BIT_DOF(IM_DOF_SURGE)
#define IM_BIT_DOF_SWAY		IM_BIT_DOF(IM_DOF_SWAY)
#define IM_BIT_DOF_HEAVE	IM_BIT_DOF(IM_DOF_HEAVE)
#define IM_BIT_DOF_ROLL		IM_BIT_DOF(IM_DOF_ROLL)
#define IM_BIT_DOF_PITCH	IM_BIT_DOF(IM_DOF_PITCH)
#define IM_BIT_DOF_YAW		IM_BIT_DOF(IM_DOF_YAW)

#define IM_BIT_MASK_DEFAULT	(IM_BIT_DOF_HEAVE | IM_BIT_DOF_ROLL | IM_BIT_DOF_PITCH) /**< default motion channel mask */ 

/**
 *  \name IM_FORMAT_*
 *
 *  Declare format macro of motion buffer
 *  (Used in IMotion_CreateSource & imCreateBuffer.)
 */
#define IM_FORMAT_TYPE_DOF				0	/**< Degree of freedom (mm, radians) */
#define IM_FORMAT_TYPE_AXIS				1	/**< Length of Actuator (mm) */
#define IM_FORMAT_TYPE_MATRIX			2	/**< Reserved */

#define IM_FORMAT_DATA_S8				0x8008	/**< Signed 8-bit samples */
#define IM_FORMAT_DATA_S16				0x8010	/**< Signed 16-bit samples */
#define IM_FORMAT_DATA_S32				0x8020  /**< Signed 32-bit samples */
#define IM_FORMAT_DATA_S64				0x8040  /**< Signed 64-bit samples */
#define IM_FORMAT_DATA_F32				0x8120  /**< 32-bit floating point samples */
#define IM_FORMAT_DATA_F64				0x8140  /**< 64-bit floating point samples */

#define IM_FORMAT_SAMPLE_RATE_MAX		200	/**< 5 ms */
#define IM_FORMAT_CHANNELS_MAX			8	/**< 6-DOF and 2 extra channels */

#define IM_FORMAT_TYPE_DEFAULT			IM_FORMAT_TYPE_DOF	/**< DOF type */
#define IM_FORMAT_SAMPLE_RATE_DEFAULT	50	/**< 20 ms */
#define IM_FORMAT_CHANNELS_DEFAULT		3	/**< 3-DOF (heave, roll, pitch) */
#define IM_FORMAT_DATA_DEFAULT			IM_FORMAT_DATA_S16	/**< (-32768 ~ 32767) */

// motion latency : samples * (1000 / sample_rate) 
#define IM_FORMAT_SAMPLES_DEFAULT		2	/**< default master sample count */ 
#define IM_FORMAT_BUFFERS_DEFAULT		1	/**< single buffer */
	
/**
 *  \name IM_LOOP_*
 *
 *  Declare motion buffer loop macro
 *  (Used in IM_BUFFER.nLoopCount & imSourcePlay.)
 */
#define IM_LOOP_MAX			254
#define IM_LOOP_INFINITE	255		

/**
 *  \name IM_FLUSH_*
 *
 *  Declare IMotion source playback complete macro
 *  (Used in IMotionSource_Flush & IMotionSource_Stop.)
 */
#define IM_FLUSH_DONE		0	
#define IM_FLUSH_FORCE		1
#define IM_FLUSH_CURRENT	2

/**
 *  \name IM_END_*
 *
 *  Declare motion buffer playback status macro
 *  (Used in IM_BUFFER.Flags & IMotionCallback.)
 */
#define IM_ERROR_STATE		0x1
#define IM_START_OF_BUFFER	0x2
#define IM_END_OF_LOOP		0x4		
#define IM_END_OF_BUFFER	0x8		// current buffer
#define IM_END_OF_STREAM	0x10	

/**
 *  \name IM_BUFFER_LOCK_*
 *
 *  Motion buffer lock options
 *  (Used in imBufferLock flags.)
 */
#define IM_BUFFER_LOCK_WRITE	0x1	/**< Indicates that the buffer being locked is write-enabled. */ 
#define IM_BUFFER_LOCK_DISCARD	0x2 /**< Indicates that you plan to clear the contents of the buffer and fill in the new data. */
#define IM_BUFFER_LOCK_WAIT		0x4 /**< If a lock cannot be obtained, retries until a lock is obtained or another error occurs. */
#define IM_BUFFER_LOCK_PEEK		0x8 /**< Indicates that it is for memory peek without changing the read or write location. */

/**
 * Motion buffer format structure 
 * (Note, this structure can only be used in IMotion.)
 */
typedef struct {
    uint32		nType;			/**< motion format type */
    uint32		nSampleRate;	/**< samples per second */
    uint32		nChannels;		/**< number of channels (i.e. heave, roll, pitch, etc.) */
    uint32		nDataFormat;	/**< data type (IM_FORMAT_DATA_*) */
    uint32		nBlockAlign;	/**< block size of data (i.e. nBlockAlign = nChannels * MOTION_SAMPLE_BYTE(nDataFormat)) */
} IM_FORMAT;

/**
 * Motion buffer structure 
 * (Note, this structure can only be used in IMotion.)
 */
typedef struct {
    uint32		nFlags;			/**< Either 0 or IM_END_OF_STREAM. */
    uint32		nMotionBytes;	/**< Size of the motion data buffer in bytes. */
    const uint8* pMotionData;	/**< Pointer to the motion data buffer. */
    uint32		nPlayBegin;		/**< First sample in this buffer to be played. */
    uint32		nPlayLength;	/**< Length of the region to be played in samples, or 0 to play the whole buffer. */
    uint32		nLoopBegin;		/**< First sample of the region to be looped. */
    uint32		nLoopLength;	/**< Length of the desired loop region in samples, or 0 to loop the entire buffer. */
    uint32		nLoopCount;		/**< Number of times to repeat the loop region, or IM_LOOP_INFINITE to loop forever. */
    void*		pContext;		/**< Context value to be passed back in callbacks. */
} IM_BUFFER;

/**
 * Motion device information structure 
 * (Note, this structure is used to obtain specifications of supported devices or to set profiles.)
 */
typedef struct {
	uint32		nId;		/**< Motion Device Index. */
	uint16		nType;		/**< Motion Driver Type. */
	uint16		nRateLimit;	/**< Maximum movement per millisecond (0~MOTION_MAX_16, default 0) */
	uint32		nMask;		/**< Bit mask of support DOF Channels (IM_BIT_*) */
	uint32		nVersion;	/**< Number of device version (major*1000 + minor*100 + patch). */
	uint32		nOptions;	/**< Device operation options. (IM_CFG_*) */
	uint32		nAxisMap;	/**< Device axis mapping (Up to 8 channels with up to 16 axis values : 0x76543210) */
	const char* szName;		/**< Motion Device Name. */
	const char* szDetail;	/**< Detailed description of motion device. */
	const char* szIPAddress;/**< Motion Device IP Address. */
	const char* szFilter;	/**< Motion Filter Name. */
} IM_DEVICE_DESC;

/**
 * Motion device status diagnostic structure 
 * (Note, this structure is used to obtain diagnostic information about motion devices.)
 */
typedef struct {
	int32		bBusy;		/**< Device Running Status. */
	int32		bHome;		/**< Reserved Home Info. */
	int32		bAlarm;		/**< Alarm Error Status. */
	int32		bInpos;		/**< Reserved Home Info. */
	int32		bEmer;		/**< Emergency Error Status. */
	int32		dCmd;		/**< Mean of Axis Commands. */
	int32		dEnc;		/**< Mean of Axis Encoder. */
} IM_DIAGNOSTIC_INFO;

/**
 * Motion device status information structure 
 * (Note that this structure is used by the motion device plugins.)
 */
typedef struct {
	int32		bCurrentOn;	/**< Motor On/Off Status. */
	int32		bServoOn;	/**< Servo On/Off Status. */
	int32		bDCCOn;		/**< Reset DCC. */
	int32		bAlarmResetOn;	/**< Reset Alarm. */
} IM_DEVICE_INFO;

/**
 * Motion device axis diagnostic structure 
 * (Note, this structure is used to obtain diagnostic information for each axis of the motion device.)
 */
typedef struct {
	int32		bBusy;		/**< Device Running Status. */
	int32		bHome;		/**< Reserved Home Info. */
	int32		bAlarm;		/**< Alarm Error Status. */
	int32		bInpos;		/**< Reserved Home Info. */
	int32		bEmer;		/**< Emergency Error Status. */
	int32		dCmd;		/**< Axis Commands. */
	int32		dEnc;		/**< Axis Encoder. */
	int32		bCurrentOn;	/**< Motor On/Off Status. */
	int32		bServoOn;	/**< Servo On/Off Status. */
	int32		bDCCOn;		/**< Reset DCC. */
	int32		bAlarmResetOn;	/**< Reset Alarm. */
} IM_DIAGNOSTIC_AXIS_INFO;

/**
 * Declare prototype of motion source callback function.
 * (Note, this callback is used to detect notification of motion source playback completion.)
 */
typedef void (*IMotionCallback)(void* context, uint32 state);

/**
 * Declare prototype of the motion input callback function.
 * (Note, this callback is used to detect motion input streaming notifications from the mixer.)
 * (Note, this function can only be used in InnoML.)
 */
typedef int (*IMotionInputCallback)(void* context, void* data, int size);

/**
 * Declare a prototype for the log output callback function.
 * (Note, this callback is used to define user log output functions.)
 */
typedef void (*IMotionDebugCallback)(uint32 id, const char* message, void* userdata);

/**
 * Declare a prototype for the custom filter callback function.
 * (Note, this callback is used to define user filter functions.)
 * (Note, this function can only be used in InnoML.)
 */
typedef float (*IMotionFilterCallback)(void* context, void* data, int size, IM_FORMAT* src_format, const IM_FORMAT* dst_format);

/**
 *  \name IM_FILTER_*
 *
 *  Motion filter type and processor declaration
 *  (Used by imCreateFilter in innoML.)
 */
#define IM_FILTER_PROCESSOR_MAX	64	/**< Maximum number of filters that can be processed at one time */

typedef enum {
	IM_FILTER_DEFAULT = 0,	/**< default filter group */
	IM_FILTER_NOISE,		/**< kalman filter (needs IM_FILTER_NOISE_PARAMS) */
	IM_FILTER_MEAN,			/**< moving average filter (needs IM_FILTER_MEAN_PARAMS) */
	IM_FILTER_HIGHPASS,		/**< high-pass filter (needs IM_FILTER_HIGHPASS_PARAMS) */
	IM_FILTER_LOWPASS,		/**< low-pass filter (needs IM_FILTER_LOWPASS_PARAMS) */
	IM_FILTER_INTEGRAL,		/**< integrator (needs IM_FILTER_INTEGRAL_PARAMS)  */
	IM_FILTER_TILT,			/**< tilt-coordinator (need not PARAMS) */
	IM_FILTER_SCALE,		/**< Increase or decrease the range of motion by a given value (needs IM_FILTER_SCALE_PARAMS) */
	IM_FILTER_OFFSET,		/**< Moves the motion range by the given value (needs IM_FILTER_OFFSET_PARAMS) */
	IM_FILTER_COMBINE,		/**< Combines two channel values into the specified operation mode (needs IM_FILTER_COMBINE_PARAMS) */
	IM_FILTER_LIMIT,		/**< value limiter (needs IM_FILTER_LIMIT_PARAMS) */	
	IM_FILTER_RATELIMIT,	/**< rate limiter (needs IM_FILTER_RATELIMIT_PARAMS) */
	IM_FILTER_WASHOUT,		/**< washout filter (needs IM_FILTER_WASHOUT_PARAMS) */	
	IM_FILTER_KINEMATICS,	/**< kinematics (needs IM_FILTER_KINEMATICS_PARAMS) */
	IM_FILTER_FORMAT,		/**< data type converter (need IM_FILTER_FORMAT_PARAMS) */
	IM_FILTER_CHANNEL,		/**< channel mapper (needs IM_FILTER_CHANNEL_PARAMS) */
	IM_FILTER_RESAMPLE,		/**< reserved */	
	IM_FILTER_CUSTOM,		/**< custom filter (needs user filter function) */
	IM_FILTER_COUNT,		/**< the number of supported filter types */
} IM_FILTER_TYPE;

/**
 * Motion filter structure 
 * (Note that this structure is only used for IMotion.)
 */
typedef struct {
	IM_FILTER_TYPE	nId;		/**< Filter Type (IM_FILTER_TYPE) */
	void*			pParams;	/**< Filter Params Data*/
	uint32			nSize;		/**< Filter Params Size */
	uint32			nCount;		/**< Filter Params Count */
	IMotionFilterCallback	pProcessor; /**< Custom filter function */
	void*			pContext;	/**< Custom filter function context. */
} IM_FILTER;

/**
 * Motion filter information structure  
 * (Note that this structure is only used for IMotion.)
 */
typedef struct {
	uint32		nId;		/**< Motion Filter Index */
	const char* szName;		/**< Motion Filter Name. */
	const char* szDetail;	/**< Detailed description of motion filter. */
} IM_FILTER_DESC;

/**
 * Motion filter params structure 
 * (Note, These structures are used to set filter parameters for each channel of motion buffer.)
 * (Note, this structure can only be used in InnoML.)
 */
typedef struct {
	int32		nCovariance;	/**< noise covariance factor (0~100, default 5) */
} IM_FILTER_NOISE_PARAMS;

/**
 * Average filter processor structure 
 */
typedef struct {
	int32		nCount;			/**< number of average values (0~16, default 4) */
} IM_FILTER_MEAN_PARAMS;

typedef struct {
	int32		nOrder;			/**< filter order (default 1, max 3) */
	float		fCutoffFrequency[3];/**< cutoff frequency (default 5) */
} IM_FILTER_HIGHPASS_PARAMS;

typedef struct {
	int32		nOrder;			/**< filter order (default 1, max 2) */
	float		fCutoffFrequency[3];/**< cutoff frequency (default 5) */
} IM_FILTER_LOWPASS_PARAMS;

typedef struct {
	int32		nOrder;			/**< filter order (default 1) */
} IM_FILTER_INTEGRAL_PARAMS;

typedef struct {
	float		fScaleFactor;	/**< scale factor (default 1) */
} IM_FILTER_SCALE_PARAMS;
	
typedef struct {
	float		fOffset;		/**< offset value (default 0) */
} IM_FILTER_OFFSET_PARAMS;

typedef struct {
	int32		nMode;			/**< Reserved */
	int32		nAxis1;			/**< Source Channel 1 */
	int32		nAxis2;			/**< Source Channel 2 */
} IM_FILTER_COMBINE_PARAMS;
	
typedef struct {
	int32		nMin;			/**< limit min value (default MOTION_MIN_16) */
	int32		nMax;			/**< limit max value (default MOTION_MAX_16) */
} IM_FILTER_LIMIT_PARAMS;

typedef struct {
	int32		nRateMax;		/**< rate limit above nRateMax (0~MOTION_MAX_16, default 256) */
} IM_FILTER_RATELIMIT_PARAMS;
	
typedef struct {
	float		fCutoffFrequency;/**< cutoff frequency (default 5) */
} IM_FILTER_WASHOUT_PARAMS;

typedef struct {
	int32		nVersion;		/**< kinematics version (700/800/1000) */	
} IM_FILTER_KINEMATICS_PARAMS;

typedef struct {
	int32		nDataFormat;
} IM_FILTER_FORMAT_PARAMS;

typedef struct {
	int32		nAxis;			/**< source axis number (1~255) */
} IM_FILTER_CHANNEL_PARAMS;

#ifdef __cplusplus
}
#endif

#endif // _IMOTION_TYPES_H_