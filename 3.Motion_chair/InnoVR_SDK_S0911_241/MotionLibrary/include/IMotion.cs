/********************************************************************************/
/**
\file      IMotion.cs
\brief     C# Interface of IMotion (Motion Library for Passive VR).
\copyright Copyright (C) 2016-2019 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

using System;
using System.Runtime.InteropServices;

using InnoMotion.Types;

namespace InnoMotion
{
    namespace Controller_IMotion
    {
        public class IMotion
        {
            #region Motion Driver Interface
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_Startup(UInt32 flags = 0, UInt32 param = 0);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_Shutdown();
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetDeviceCount();
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetDeviceDescription(Int32 index, IntPtr desc = default(IntPtr)); 
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetDeviceProfile(UInt32 devid = MotionTypes.IM_DEVICE_ID_DEFAULT, IntPtr desc = default(IntPtr));
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetFilterCount();
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetFilterDescription(Int32 index, IntPtr desc = default(IntPtr));
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_SetLogFunction(IMotionDebugCallback callback, IntPtr userdata = default(IntPtr));
            #endregion //  Motion Driver Interface

            #region Motion Device Interface
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static IntPtr IMotion_Create(UInt32 devid = 0, IntPtr desc = default(IntPtr));
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_Destroy(IntPtr device, UInt32 flags = 0);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_Start(IntPtr device, IntPtr format = default(IntPtr), UInt32 flags = MotionTypes.IM_DEVICE_MOVE_DEFAULT, IntPtr shared_context = default(IntPtr));
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_Stop(IntPtr device, UInt32 flags = MotionTypes.IM_DEVICE_MOVE_DEFAULT);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_SendStream(IntPtr device, short[] data, UInt32 size, UInt32 count = 1);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetInfo(IntPtr device, out IM_DIAGNOSTIC_INFO info);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetAxesInfo(IntPtr device, [Out] IM_DIAGNOSTIC_AXIS_INFO[] info = null, UInt32 max = 1);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetProfile(IntPtr device, IntPtr desc = default(IntPtr));
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetFormat(IntPtr device, IntPtr format); 
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_GetID(IntPtr device);
            #endregion // Motion Device Interface

            #region Motion Source Interface
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static IntPtr IMotion_CreateSource(IntPtr device, ref IM_FORMAT format,
                IMotionCallback user_func = null, UInt32 cb_flags = 0, UInt32 mask = 0,
                UInt32 samples = MotionTypes.IM_FORMAT_SAMPLES_DEFAULT,
                UInt32 buffers = MotionTypes.IM_FORMAT_BUFFERS_DEFAULT);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotion_DestroySource(IntPtr device, IntPtr source);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotionSource_Start(IntPtr source, UInt32 flags = 0);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotionSource_Stop(IntPtr source, UInt32 flags = MotionTypes.IM_FLUSH_FORCE);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotionSource_SubmitBuffer(IntPtr source, ref IM_BUFFER buffer);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static UInt32 IMotionSource_GetQueuedBufferCount(IntPtr source);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotionSource_Flush(IntPtr source, UInt32 flags = MotionTypes.IM_FLUSH_DONE);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static IntPtr IMotionSource_GetDevice(IntPtr source);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static Int32 IMotionSource_GetFormat(IntPtr source, out IM_FORMAT format);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static UInt32 IMotionSource_GetInfo(IntPtr source, out IM_BUFFER buffer);
            #endregion // Motion Source Interface

            #region Motion CSV Interface
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static int IMotion_LoadCSV(string filename, out IM_FORMAT format, out IntPtr motion_buf, out UInt32 motion_len, string key);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static int IMotion_LoadCSV_RAW(byte[] data, int size, out IM_FORMAT format, out IntPtr motion_buf, out UInt32 motion_len, string key);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static int IMotion_FreeCSV(IntPtr motion_buf);
            [DllImport("IMotion.dll", CallingConvention = CallingConvention.Cdecl)]
            extern public static int IMotion_SaveCSV(string filename, ref IM_FORMAT format, ref IntPtr motion_buf, UInt32 motion_len, string key);
            #endregion //Motion CSV Interface
        };
    }
};



