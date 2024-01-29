/********************************************************************************/
/**
\file      InnoML.cs
\brief     C# Interface of InnoML (Motion Library for Interactive VR).
\copyright Copyright (C) 2016-2019 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

using System;
using System.Runtime.InteropServices;

using InnoMotion.Types;

namespace InnoMotion
{
    namespace Controller_InnoML
    {
        public class InnoML
        {
            const string DLLName = "InnoML";
            //const string DLLName = "InnoML.dll";

            #region Context function
            /* Create Context Object */
            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imCreateContext(
                Int32 buffer = 0,               /* master buffer for motion device (need only motion format) */
                UInt32 id = 0,                  /* number of motion device connect ip (1~255) */
                IntPtr desc = default(IntPtr)   /* description for creating a motion device (name, version, options) */                
            );

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSetContext(Int32 ctx, UInt32 flags = 0);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imGetContext();

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imGetBuffer();

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSetFilter(Int32 filter);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imGetFilter();

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imStart([MarshalAs(UnmanagedType.FunctionPtr)]IMotionInputCallback callback = null, IntPtr streamer_obj = default(IntPtr), Int32 shared_context = 0, UInt32 flags = MotionTypes.IM_DEVICE_MOVE_DEFAULT);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imStop(UInt32 flags = MotionTypes.IM_DEVICE_MOVE_DEFAULT);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imGetDescriptionCount();

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imGetDescription(IntPtr desc = default(IntPtr), Int32 count = 1);
            
            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imGetProfile(IntPtr desc = default(IntPtr), UInt32 devid = 0);
            
            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imGetDiagnostic([Out] IM_DIAGNOSTIC_AXIS_INFO[] axis = null, Int32 count = 1);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imGetPlayingSourceCount();

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSetMasterVolume(Int32 volume);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imStopAllSources();

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSetLogFunction(IMotionDebugCallback callback, IntPtr userdata = default(IntPtr));

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imDestroyContext(Int32 ctx, UInt32 flags = 0);
            #endregion

            #region Buffer function
            /* Create Buffer Object */
            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32/*IMBuffer*/ imCreateBuffer(
                Int32 sample_rate = 0,  /* motion samples per second (50 ~ 1000) */
                Int32 format = 0,    /* motion data format (always IM_FORMAT_DATA_S16) */
                Int32 channels = 0,     /* number of channels (1~8) */
                Int32 samples = 0,      /* motion sample count (power of 2) */
                Int32 buffers = 0,      /* motion buffer count (most 1 will be used) */
                Int32 type = 0          /* motion format type (most IM_FORMAT_TYPE_DOF will be used) */
            );

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32/*IMBuffer*/ imCreateBufferFromFormat(IntPtr format = default(IntPtr)/*IM_FORMAT*/, Int32 samples = 0, Int32 buffers = 0);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32/*IMBuffer*/ imLoadBuffer(string url, string key = null);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imLoadBufferMemory(byte[] data, Int32 size, string key = null);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static float imBufferConvert(Int32 buffer, ref Int32 adjusted_buffer, Int32 desired_buffer = 0, Int32 filter = 0);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imBufferGetSize(Int32 buffer/*IMBuffer*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imBufferGetDuration(Int32 buffer/*IMBuffer*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imBufferGetFormat(
                Int32 buffer,         // IMBuffer  
                ref int sample_rate, // int* 
                ref int format,      // int* 
                ref int channels,    // int* 
                ref int samples);    // int* 

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static float imBufferConvert(Int32 buffer/*IMBuffer*/, IntPtr adjusted_buffer /*IMBuffer**/, Int32 desired_buffer = 0);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imBufferEnqueue(
                Int32 buffer, // IMBuffer
                [In] short[,] data, // void*
                Int32 size);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imBufferEnqueue(
                Int32 buffer, // IMBuffer
                [In] short[] data, // void*
                Int32 size);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imBufferDequeue(
                Int32 buffer, // IMBuffer
                [Out] short[,] data, // void*
                Int32 size);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imBufferDequeue(
                Int32 buffer, // IMBuffer
                [Out] short[] data, // void*
                Int32 size);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imBufferDequeue(
                Int32 buffer, // IMBuffer
                IntPtr data, // void*
                Int32 size);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imBufferGetQueuedCount(Int32 buffer/*IMBuffer*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imDeleteBuffer(Int32 buffer/*IMBuffer*/);
            #endregion

            #region Source function
            /* Create Source Object */
            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 /*IMSource*/ imCreateSource(Int32 buffer = 0/*IMBuffer*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 /*IMBuffer*/ imSourceSetBuffer(Int32 source/*IMSource*/, Int32 buffer/*IMBuffer*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 /*IMBuffer*/ imSourceGetBuffer(Int32 source /*IMSource*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSourceSetFilter(Int32 source, Int32 filter);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSourceGetFilter(Int32 source);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSourcePlay(
                Int32 source /*IMSource*/,
                Int32 loop_count = 0,
                IMotionCallback listener_func = null,
                IntPtr listener_obj = default(IntPtr)/* IMObject* = void* */
                );

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSourceStop(Int32 source /*IMSource*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSourcePause(Int32 source /*IMSource*/, Int32 paused);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSourceSetVolume(Int32 source /*IMSource*/, Int32 volume);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSourceSetSpeed(Int32 source, Int32 speed);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imSourceGetPosition(Int32 source /*IMSource*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imDeleteSource(Int32 source /*IMSource*/);
            #endregion

            #region Input function
            /* Create Input Object */
            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 /*IMInput*/ imCreateInput(Int32 buffer = 0/*IMBuffer*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imInputSetBuffer(Int32 input/*IMInput*/, Int32 buffer/*IMBuffer*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 /*IMBuffer*/ imInputGetBuffer(Int32 input/*IMInput*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imInputSetFilter(Int32 input/*IMInput*/, Int32 filter/*IMFilter*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 /*IMFilter*/ imInputGetFilter(Int32 input/*IMInput*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imInputStart(
                Int32 input /*IMInput*/,
                [MarshalAs(UnmanagedType.FunctionPtr)]IMotionInputCallback callback = null,
				IntPtr netstream = default(IntPtr)/* IMObject* = void* */
			);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imInputStop(Int32 input/*IMInput*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imInputSendStream(Int32 input/*IMInput*/, IntPtr data /* void* */, Int32 size);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imInputSendStream(Int32 input, [In] short[] data, Int32 size);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imDeleteInput(Int32 input/*IMInput*/);
            #endregion

            #region Filter function
            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 /*IMFilter*/ imCreateFilter(
                IM_FILTER_TYPE type = IM_FILTER_TYPE.IM_FILTER_DEFAULT, /*  filter type */
                IMotionFilterCallback processor = null,                 /* custom filter processor */
                IntPtr processor_obj = default(IntPtr)                  /* custom filter params */
                );

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imFilterSetParams(
                Int32 filter,                   /* filter object */
                IntPtr data = default(IntPtr),  /* filter params data */
                Int32 size = 0,                 /* filter params size */
                Int32 count = 0                 /* filter params count (~IM_FORMAT_CHANNELS_MAX) */
                );

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imFilterSetParams(
                Int32 filter,       /* filter object */
                [In] byte[] data,   /* filter params data (byte array) */
                Int32 size = 0,     /* filter params size */
                Int32 count = 0     /* filter params count (~IM_FORMAT_CHANNELS_MAX) */
                );

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imFilterGetParams(
                Int32 filter,                   /* filter object */
                IntPtr data = default(IntPtr),  /* filter params data */
                Int32 size = 0,                 /* filter params size */
                Int32 count = 0                 /* filter params count (~IM_FORMAT_CHANNELS_MAX) */
                );

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imFilterAppend(Int32 filter /*IMFilter*/, Int32 child /*IMFilter*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imFilterRemove(Int32 filter /*IMFilter*/, Int32 child /*IMFilter*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static float imFilterBuild(Int32 filter /*IMFilter*/, Int32 src_buffer /*IMBuffe*/, Int32 dst_buffer = 0 /*IMBuffer*/);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imFilterProcess(Int32 filter /*IMFilter*/, IntPtr data, Int32 size = 0);

            [DllImport(DLLName, CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 imDeleteFilter(Int32 filter /*IMFilter*/);
            #endregion
        };
    }
};



