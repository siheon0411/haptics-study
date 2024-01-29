/********************************************************************************/
/**
\file      IMotion_types.cs
\brief     C# Interface of the common type declarations
\copyright Copyright (C) 2016-2019 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

using System;
using System.Runtime.InteropServices;

namespace InnoMotion
{
    namespace Types
    {
        #region Structures
        /**
         * Motion buffer format structure 
         * (Note, this structure can only be used in IMotion.)
         */
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FORMAT
        {
            public UInt32 nType;        /**< motion format type */
            public UInt32 nSampleRate;  /**< samples per second */
            public UInt32 nChannels;    /**< number of channels (i.e. heave, roll, pitch, etc.) */
            public UInt32 nDataFormat;  /**< data type (IM_FORMAT_DATA_*) */
            public UInt32 nBlockAlign;  /**< block size of data (i.e. nBlockAlign = nChannels * MOTION_SAMPLE_BYTE(nDataFormat)) */

            public IM_FORMAT(UInt32 uinit = 0)
            {
                this.nType = uinit;
                this.nSampleRate = uinit;
                this.nChannels = uinit;
                this.nDataFormat = uinit;
                this.nBlockAlign = uinit;
            }
        }

        /**
         * Motion buffer structure 
         * (Note, this structure can only be used in IMotion.)
         */
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_BUFFER
        {
            public UInt32 nFlags;       /**< Either 0 or IM_END_OF_STREAM. */
            public UInt32 nMotionBytes; /**< Size of the motion data buffer in bytes. */
            public IntPtr pMotionData;  /**< Pointer to the motion data buffer. (const uint8*)*/
            public UInt32 nPlayBegin;   /**< First sample in this buffer to be played. */
            public UInt32 nPlayLength;  /**< Length of the region to be played in samples, or 0 to play the whole buffer. */
            public UInt32 nLoopBegin;   /**< First sample of the region to be looped. */
            public UInt32 nLoopLength;  /**< Length of the desired loop region in samples, or 0 to loop the entire buffer. */
            public UInt32 nLoopCount;   /**< Number of times to repeat the loop region, or IM_LOOP_INFINITE to loop forever. */
            public IntPtr pContext;     /**< Context value to be passed back in callbacks. (void*) */

            public IM_BUFFER(uint uinit = 0)
            {
                this.nFlags = uinit;
                this.nMotionBytes = uinit;
                this.pMotionData = default(IntPtr);
                this.nPlayBegin = uinit;
                this.nPlayLength = uinit;
                this.nLoopBegin = uinit;
                this.nLoopLength = uinit;
                this.nLoopCount = uinit;
                this.pContext = default(IntPtr);
            }
        }

        /**
         * Motion device information structure 
         * (Note, this structure is used to obtain specifications of supported devices or to set profiles.)
         */
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct IM_DEVICE_DESC
        {
            public UInt32 nId;          /**< Motion Device Index. */
            public UInt16 nType;        /**< Either Out device or Capture device. */
            public UInt16 nRateLimit;	/**< Maximum movement per millisecond (0~MOTION_MAX_16, default 0) */
            public UInt32 nMask;        /**< Bit mask of support DOF Channels (IM_BIT_*) */
            public UInt32 nVersion;     /**< Number of device version (major*1000 + minor*100 + patch). */
            public UInt32 nOptions;     /**< Device operation options. (IM_CFG_*) */
            public UInt32 nAxisMap;		/**< Device axis map (0~15 axis * max 8 : 0x76543210) */
            [MarshalAs(UnmanagedType.LPStr)]
            public string szName;       /**< Motion Device Name. */
            [MarshalAs(UnmanagedType.LPStr)]
            public string szDetail;     /**< Detailed description of motion device. */
            [MarshalAs(UnmanagedType.LPStr)]
            public string szIPAddress;  /**< Motion Device IP Address. */
            [MarshalAs(UnmanagedType.LPStr)]
            public string szFilter;	    /**< Motion Filter Name. */

            public IM_DEVICE_DESC(UInt32 uinit = 0)
            {
                this.nId = uinit;
                this.nType = (UInt16)uinit;
                this.nRateLimit = (UInt16)uinit;
                this.nMask = uinit;
                this.nVersion = uinit;
                this.nOptions = uinit;
                this.nAxisMap = uinit;
                this.szName = null;
                this.szDetail = null;
                this.szIPAddress = null;
                this.szFilter = null;
            }
        }

        /**
         * Motion device status diagnostic structure 
         * (Note, this structure is used to obtain diagnostic information about motion devices.)
         */
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_DIAGNOSTIC_INFO
        {
            public Int32 bBusy; /**< Device Running Status. */
            public Int32 bHome; /**< Reserved Home Info. */
            public Int32 bAlarm;/**< Alarm Error Status. */
            public Int32 bInpos;/**< Reserved Home Info. */
            public Int32 bEmer; /**< Emergency Error Status. */
            public Int32 fCmd;  /**< Mean of Axis Commands. */
            public Int32 fEnc;  /**< Mean of Axis Encoder. */

            public IM_DIAGNOSTIC_INFO(Int32 init = 0)
            {
                this.bBusy = init;
                this.bHome = init;
                this.bAlarm = init;
                this.bInpos = init;
                this.bEmer = init;
                this.fCmd = init;
                this.fEnc = init;
            }
        }

        /**
         * Motion device status information structure 
         * (Note that this structure is used by the motion device plugins.)
         */
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_DEVICE_INFO
        {
            public Int32 bCurrentOn;    /**< Motor On/Off Status. */
            public Int32 bServoOn;      /**< Servo On/Off Status. */
            public Int32 bDCCOn;        /**< Reset DCC. */
            public Int32 bAlarmResetOn; /**< Reset Alarm. */

            public IM_DEVICE_INFO(Int32 init = 0)
            {
                bCurrentOn = init;
                bServoOn = init;
                bDCCOn = init;
                bAlarmResetOn = init;
            }
        }

        /**
         * Motion device axis diagnostic structure 
         * (Note, this structure is used to obtain diagnostic information for each axis of the motion device.)
         */
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_DIAGNOSTIC_AXIS_INFO
        {
            public Int32 bBusy;         /**< Device Running Status. */
            public Int32 bHome;         /**< Reserved Home Info. */
            public Int32 bAlarm;        /**< Alarm Error Status. */
            public Int32 bInpos;        /**< Reserved Home Info. */
            public Int32 bEmer;         /**< Emergency Error Status. */
            public Int32 fCmd;          /**< Axis Commands. */
            public Int32 fEnc;          /**< Axis Encoder. */
            public Int32 bCurrentOn;    /**< Motor On/Off Status. */
            public Int32 bServoOn;      /**< Servo On/Off Status. */
            public Int32 bDCCOn;        /**< Reset DCC. */
            public Int32 bAlarmResetOn; /**< Reset Alarm. */

            public IM_DIAGNOSTIC_AXIS_INFO(Int32 init = 0)
            {
                this.bBusy = init;
                this.bHome = init;
                this.bAlarm = init;
                this.bInpos = init;
                this.bEmer = init;
                this.fCmd = init;
                this.fEnc = init;
                this.bCurrentOn = init;
                this.bServoOn = init;
                this.bDCCOn = init;
                this.bAlarmResetOn = init;
            }
        }

        /**
         * Motion filter params structure 
         * (Note, These structures are used to set filter parameters for each channel of motion buffer.)
         * (Note, this structure can only be used in InnoML.)
         */
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_NOISE_PARAMS
        {
            public Int32 nCovariance;	/**< noise covariance factor (0~100, default 5) */

            public IM_FILTER_NOISE_PARAMS(Int32 covariance = 5)
            {
                nCovariance = covariance;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_MEAN_PARAMS
        {
            public Int32 nCount;		/**< number of average values (0~16, default 4) */

            public IM_FILTER_MEAN_PARAMS(Int32 count = 4)
            {
                nCount = count;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_HIGHPASS_PARAMS
        {
            public Int32 nOrder;                /**< filter order (default 1, max 3) */
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]   
            public float[] fCutoffFrequency;	/**< cutoff frequency (default 5) */

            public IM_FILTER_HIGHPASS_PARAMS(float cutoffFrequency = 5, Int32 order = 1)
            {
                nOrder = order;
                fCutoffFrequency = new float[3];
                fCutoffFrequency[0] = cutoffFrequency;
                fCutoffFrequency[1] = cutoffFrequency;
                fCutoffFrequency[2] = cutoffFrequency;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_LOWPASS_PARAMS
        {
            public Int32 nOrder;                /**< filter order (default 1, max 2) */
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]   
            public float[] fCutoffFrequency;	/**< cutoff frequency (default 5) */

            public IM_FILTER_LOWPASS_PARAMS(float cutoffFrequency = 5, Int32 order = 1)
            {
                nOrder = order;
                fCutoffFrequency = new float[3];
                fCutoffFrequency[0] = cutoffFrequency;
                fCutoffFrequency[1] = cutoffFrequency;
                fCutoffFrequency[2] = cutoffFrequency;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_INTEGRAL_PARAMS
        {
            public Int32 nOrder;          /**< filter order (default 1) */

            public IM_FILTER_INTEGRAL_PARAMS(Int32 order = 1)
            {
                nOrder = order;
            }
        }
 
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_SCALE_PARAMS
        {
            public float fScaleFactor;      /**< scale factor (default 1) */

            public IM_FILTER_SCALE_PARAMS(float scaleFactor = 1)
            {
                fScaleFactor = scaleFactor;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_OFFSET_PARAMS
        {
            public float fOffset;	/**< offset value (default 0) */

            public IM_FILTER_OFFSET_PARAMS(float offset = 0)
            {
                fOffset = offset;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_COMBINE_PARAMS
        {
            public Int32 nMode;		/**< Reserved */
            public Int32 nAxis1;	/**< Source Channel 1 */
            public Int32 nAxis2;	/**< Source Channel 2 */

            public IM_FILTER_COMBINE_PARAMS(Int32 mode = 0, Int32 axis1 = 0, Int32 axis2 = 0)
            {
                nMode = mode;
                nAxis1 = axis1;
                nAxis2 = axis2;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_LIMIT_PARAMS
        {
            public Int32 nMin;      /**< limit min value (default MOTION_MIN_16) */
            public Int32 nMax;      /**< limit max value (default MOTION_MAX_16) */

            public IM_FILTER_LIMIT_PARAMS(Int32 min = -32768, Int32 max = 32767)
            {
                nMin = min;
                nMax = max;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_RATELIMIT_PARAMS
        {
            public Int32 nRateMax;		/**< rate limit above nRateMax (0~MOTION_MAX_16, default 256) */

            public IM_FILTER_RATELIMIT_PARAMS(Int32 rateMax = 256)
            {
                nRateMax = rateMax;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_WASHOUT_PARAMS
        {
            public float fCutoffFrequency;	/**< cutoff frequency (default 5) */

            public IM_FILTER_WASHOUT_PARAMS(float cutoffFrequency = 5)
            {
                fCutoffFrequency = cutoffFrequency;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_KINEMATICS_PARAMS
        {
            public Int32 nVersion;	/**< kinematics version (700/800/1000) */

            public IM_FILTER_KINEMATICS_PARAMS(Int32 version = 1000)
            {
                nVersion = version;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_FORMAT_PARAMS
        {
            public Int32 nDataFormat;

            public IM_FILTER_FORMAT_PARAMS(Int32 dataFormat = 0)
            {
                nDataFormat = dataFormat;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_CHANNEL_PARAMS
        {
            public Int32 nAxis;		/**< source axis number (0~255) */

            public IM_FILTER_CHANNEL_PARAMS(Int32 axis = 0)
            {
                nAxis = axis;
            }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER
        {
            public IM_FILTER_TYPE nId;	/**< Filter Type (IM_FILTER_TYPE) */
            public IntPtr pParams;	    /**< Filter Params Data*/
            public UInt32 nSize;		/**< Filter Params Size */
            public UInt32 nCount;		/**< Filter Params Count */
            public IMotionFilterCallback pProcessor; /**< Custom filter function */
            public IntPtr pContext;	/**< Custom filter function context. */
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct IM_FILTER_DESC
        {
	        public UInt32 nId;		/**< Motion Filter Index */
	        public string szName;	/**< Motion Filter Name. */
	        public string szDetail;	/**< Detailed description of motion filter. */
        }
        #endregion //Structures

        #region Callback function (Delegate)
        /**
        * Declare prototype of motion source callback function.
        * (Note, this callback is used to detect notification of motion source playback completion.)
         */
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void IMotionCallback(uint context_ptr, UInt32 state);

        /**
         * Declare prototype of the motion input callback function.
         * (Note, this callback is used to detect motion input streaming notifications from the mixer.)
         * (Note, this function can only be used in InnoML.)
         */
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int IMotionInputCallback(uint context_ptr, IntPtr data_ptr, int size);

        /**
         * Declare a prototype for the log output callback function.
         * (Note, this callback is used to define user log output functions.)
         */
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void IMotionDebugCallback(UInt32 id, string message, IntPtr userdata);

        /**
         * Declare a prototype for the custom filter callback function.
         * (Note, this callback is used to define user filter functions.)
         * (Note, this function can only be used in InnoML.)
         */
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate float IMotionFilterCallback(uint context_ptr, IntPtr data, int size, ref IM_FORMAT src_format, ref IM_FORMAT dst_format);
        #endregion

        #region Enum
        public enum IM_ERROR
        {
	        IM_DISCONNECTED =-1,/**< Disconnected State */
	        IM_OK,				/**< Normal State */	
	        IM_ERROR_DRIVER,	/**< Device Driver Error */	
	        IM_ERROR_EMER,		/**< Emergency Error */	
	        IM_ERROR_ALARM,		/**< Alarm Error */	
	        IM_ERROR_SERVO,		/**< Servo Off */	
	        IM_ERROR_COUNT,		
        }

        public enum IM_FILTER_TYPE
        {
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
        }

        public enum IM_DOF_TYPE
        {
            IM_DOF_SURGE = 0,   /**< Rear-Front movement */
            IM_DOF_SWAY,	    /**< Left-Right movement */
            IM_DOF_HEAVE,	    /**< Down-Up movement */
            IM_DOF_ROLL,	    /**< Left-Right rotation */
            IM_DOF_PITCH,	    /**< Rear-Front rotation */
            IM_DOF_YAW,		    /**< Horizontal Left-Right rotation */
            IM_DOF_COUNT
        }
        #endregion

        public class MotionTypes
        {
            /**
             *  \name common macro
             *
             *  Declare common macro that is frequently used
             */
            #region Common macro
            public const int IM_STRING_MAX = 256;
            public const double IM_PI    = 3.14159265358979323846;
            public const int IM_MAXSHORT = 0x7FFF;
            public const int IM_MAXINT   = 0x7FFFFFFF;
            public const int IM_MAX_ENUM = 0x7FFFFFFF;

            public const int MOTION_MASK_BITSIZE = 0xFF;
            public const int MOTION_MASK_DATATYPE = 1<<8;

            public static int MOTION_SAMPLE_BIT(int fmt) { return (fmt & MOTION_MASK_BITSIZE); }
            public static int MOTION_SAMPLE_BYTE(int fmt) { return MOTION_SAMPLE_BIT(fmt) >> 3; }

            public static int MOTION_MAX_VAL(int bits) { return ((1 << (bits - 1)) - 1); }
            public static int MOTION_MAX_16() { return MOTION_MAX_VAL(16); }
            
            public static int MOTION_MIN_VAL(int bits) { return -(1 << (bits - 1)); }
            public static int MOTION_MIN_16() { return MOTION_MIN_VAL(16); }
            #endregion

            /**
             *  \name IM_CFG_*
             *
             *  Declare driver option (LSB:16)
             *  (Used in IM_DEVICE_DESC options and profiles.) 
             */
            #region IM_CFG_
            public const int IM_CFG_DEBUG_MODE    = 0x1;    // console log
            public const int IM_CFG_FILE_LOG      = 0x2;    // check "IMotion.log"
            public const int IM_CFG_EMUL_MODE     = 0x4;    // reserved
            public const int IM_CFG_ASYNC_MODE    = 0x8;    // asynchronous device running mode (check "bBusy")
            public const int IM_CFG_FORCE_PROFILE = 0x10;   // force device settings to profile (check "IMotion.ini")
            #endregion

            /**
             *  \name IM_CFG_DEVICE_*
             *
             *  Declare device option (MSB:16)
             *  (Used only for profiles "IMotion.ini")
             */
            #region IM_CFG_DEVICE_
            public const int IM_CFG_DEVICE_ZERO_INIT        = 0x10000; // reserved
            public const int IM_CFG_DEVICE_WEIGHT_EST       = 0x20000; // reserved
            public const int IM_CFG_DEVICE_INC_ENCODER      = 0x40000; // reserved
            public const int IM_CFG_DEVICE_ZERO_INIT_NOSLEEP= 0x80000; // reserved
            public const int IM_CFG_DEVICE_READ_ALARM       = 0x100000; // reserved
            public const int IM_CFG_DEVICE_SET_IPADDRESS    = 0x200000; // reserved
            public const int IM_CFG_DEVICE_THREAD_UNSAFE    = 0x400000; // reserved
            #endregion

            /**
             *  \name IM_DEVICE_*
             *
             *  Declare motion device connection and disconnection options
             *  (Used in IMotion_Create, IMotion_Destroy, imCreateContext, imDestroyContext flags.)
             */
            #region IM_DEVICE_
            public const int IM_DEVICE_ID_DEFAULT       = 11;   /* Default device IP */
            public const int IM_DEVICE_ID_MAX           = 255;  /* 1 ~ 255 */            
            public const int IM_DEVICE_MOVE_NONE        = 0x0;  /* Used in IMotion_Start/Stop or imStart/Stop flags */
            public const int IM_DEVICE_MOVE_SETTLE      = 0x1;  /* Used in IMotion_Start/Stop or imStart/Stop flags */
            public const int IM_DEVICE_MOVE_NEUTRAL     = 0x2;  /* Used in IMotion_Start/Stop or imStart/Stop flags */
            public const int IM_DEVICE_MOVE_DEFAULT     = 0x3;  /* Settle & Neutral : Start(S->N), Stop(N->S) */
            public const int IM_DEVICE_MOVE_PAUSE       = 0x4;  /* Reserved */
            public const int IM_DEVICE_MOTOR_POWER_OFF  = 0x10; /* Used in IMotion_Destroy flags */
            #endregion

            /**
             *  \name IM_BIT_*
             *
             *  Motion channel bit mask declaration
             *  (Used in IM_DEVICE_DESC mask.)
             */
            #region IM_BIT_
            public static byte IM_BIT_DOF_SURGE = 1 << (byte)IM_DOF_TYPE.IM_DOF_SURGE;
            public static byte IM_BIT_DOF_SWAY  = 1 << (byte)IM_DOF_TYPE.IM_DOF_SWAY;
            public static byte IM_BIT_DOF_HEAVE = 1 << (byte)IM_DOF_TYPE.IM_DOF_HEAVE;
            public static byte IM_BIT_DOF_ROLL  = 1 << (byte)IM_DOF_TYPE.IM_DOF_ROLL;
            public static byte IM_BIT_DOF_PITCH = 1 << (byte)IM_DOF_TYPE.IM_DOF_PITCH;
            public static byte IM_BIT_DOF_YAW   = 1 << (byte)IM_DOF_TYPE.IM_DOF_YAW;
            public static byte IM_BIT_MASK_DEFAULT = (byte)(IM_BIT_DOF_HEAVE | IM_BIT_DOF_ROLL | IM_BIT_DOF_PITCH); /*< default motion channel mask */ 
            #endregion

            /**
             *  \name IM_FORMAT_*
             *
             *  Declare format macro of motion buffer
             *  (Used in IMotion_CreateSource & imCreateBuffer.)
             */
            #region IM_FORMAT_
            public const int IM_FORMAT_TYPE_DOF     = 0;    /* Degree of freedom (mm, radians) */
            public const int IM_FORMAT_TYPE_AXIS    = 1;    /* Length of Actuator (mm) */
            public const int IM_FORMAT_TYPE_MATRIX  = 2;    /* Reserved */

            public const int IM_FORMAT_DATA_S8  = 0x8008;   /* Signed 8-bit samples */
            public const int IM_FORMAT_DATA_S16 = 0x8010;   /* Signed 16-bit samples */
            public const int IM_FORMAT_DATA_S32 = 0x8020;   /* Signed 32-bit samples */
            public const int IM_FORMAT_DATA_S64 = 0x8040;   /* Signed 64-bit samples */
            public const int IM_FORMAT_DATA_F32 = 0x8120;   /* 32-bit floating point samples */
            public const int IM_FORMAT_DATA_F64 = 0x8140;   /* 64-bit floating point samples */
            
            public const int IM_FORMAT_SAMPLE_RATE_MAX  = 200; /* 5ms */
            public const int IM_FORMAT_CHANNELS_MAX     = 8;   /* 6-DOF (surge, sway, heave, roll, pitch, yaw) */

            public const int IM_FORMAT_TYPE_DEFAULT         = IM_FORMAT_TYPE_DOF;   /* DOF type */
            public const int IM_FORMAT_SAMPLE_RATE_DEFAULT  = 50;                   /* 20ms */
            public const int IM_FORMAT_CHANNELS_DEFAULT     = 3;                    /* 3-DOF (heave, roll, pitch) */
            public const int IM_FORMAT_DATA_DEFAULT         = IM_FORMAT_DATA_S16;   /* (-32768 ~ 32767) */

            // motion latency : (samples * buffers) * (1000 / sample_rate) 
            public const int IM_FORMAT_SAMPLES_DEFAULT = 2; // default master sample count
            public const int IM_FORMAT_BUFFERS_DEFAULT = 1; // single buffer
            #endregion

            /**
             *  \name IM_LOOP_*
             *
             *  Declare motion buffer loop macro
             *  (Used in IM_BUFFER.nLoopCount & imSourcePlay.)
             */
            #region IM_LOOP_
            public const int IM_LOOP_MAX        = 254;
            public const int IM_LOOP_INFINITE   = 255;
            #endregion

            /**
             *  \name IM_FLUSH_*
             *
             *  Declare IMotion source playback complete macro
             *  (Used in IMotionSource_Flush.)
             */
            #region IM_FLUSH_
            public const int IM_FLUSH_DONE      = 0;
            public const int IM_FLUSH_FORCE     = 1;
            public const int IM_FLUSH_CURRENT   = 2;
            #endregion

            /**
             *  \name IM_END_*
             *
             *  Declare motion buffer playback status macro
             *  (Used in IM_BUFFER.Flags & IMotionCallback.)
             */
            #region IM_END_
            public const int IM_ERROR_STATE     = 0x1;
            public const int IM_START_OF_BUFFER = 0x2;
            public const int IM_END_OF_LOOP     = 0x4;
            public const int IM_END_OF_BUFFER   = 0x8;  // current buffer
            public const int IM_END_OF_STREAM   = 0x10;
            #endregion

            /**
            *  \name IM_BUFFER_LOCK_*
            *
            *  Motion buffer lock options
            *  (Used in imBufferLock flags.)
            */
            #region IM_BUFFER_LOCK_
            public const int IM_BUFFER_LOCK_WRITE   = 0x1; /**< Indicates that the buffer being locked is write-enabled. */ 
            public const int IM_BUFFER_LOCK_DISCARD = 0x2; /**< Indicates that you plan to clear the contents of the buffer and fill in the new data. */
            public const int IM_BUFFER_LOCK_WAIT	= 0x4; /**< If a lock cannot be obtained, retries until a lock is obtained or another error occurs. */
            public const int IM_BUFFER_LOCK_PEEK    = 0x8; /**< Indicates that it is for memory peek without changing the read or write location. */
            #endregion

            /**
             *  \name IM_FILTER_*
             *
             *  Motion filter type and processor declaration
             *  (Used by imCreateFilter in innoML.)
             */
            #region IM_FILTER_
            public const int IM_FILTER_PROCESSOR_MAX = 64;
            #endregion

        }
    }
}