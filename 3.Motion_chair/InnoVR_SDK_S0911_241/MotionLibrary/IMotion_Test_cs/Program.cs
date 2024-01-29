using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using InnoMotion.Types;
using InnoMotion.Controller_IMotion;

namespace IMotion_test_cs
{
    class Program
    {
        private const int SAMPLE_CHANNELS = MotionTypes.IM_FORMAT_CHANNELS_DEFAULT;
        private const int SAMPLE_RATE = MotionTypes.IM_FORMAT_SAMPLE_RATE_DEFAULT;

        static int loop_count = 1;
        static bool done = false;
        public void MotionCallback(uint data_ptr, uint state)
        {
            if (MotionTypes.IM_END_OF_BUFFER == state)
            {
                Console.WriteLine("<< CallbackData : IM_END_OF_BUFFER >>");
            }
            if (MotionTypes.IM_END_OF_LOOP == state)
            {
                Console.WriteLine("<< CallbackData : IM_END_OF_LOOP[{0}] >>", loop_count);
                if (loop_count-- == 0)
                    done = true;
            }
        }


        static void Main(string[] args)
        {
            IMotionCallback ICallback = new IMotionCallback(new Program().MotionCallback);

            IntPtr motion;
            uint currentPos = 0;
            
            float motion_frequency = 1; /* motion frequency in cycles per sample */
            float motion_amplitude = 0.5f; /* motion scale, -32768 ~ 32767 */

            /* Start up */
            IMotion.IMotion_Startup();

            // Create Device (default driver)
            IM_DEVICE_DESC desc = new IM_DEVICE_DESC();
            desc.szName = "Inno Motion Seat";
            desc.nOptions |= MotionTypes.IM_CFG_DEBUG_MODE;
            
            IntPtr desc_ptr = Marshal.AllocHGlobal(Marshal.SizeOf(desc));
            Marshal.StructureToPtr(desc, desc_ptr, false);

            motion = IMotion.IMotion_Create(0, desc_ptr);

            // get device description
            IMotion.IMotion_GetProfile(motion, desc_ptr);
            desc = (IM_DEVICE_DESC)Marshal.PtrToStructure(desc_ptr, typeof(IM_DEVICE_DESC));

            // get device diagnostics
            #region IMotion Diagnostic Example
            int error = 0;
            IM_DIAGNOSTIC_INFO info;
            IM_DIAGNOSTIC_AXIS_INFO[] axisInfo = new IM_DIAGNOSTIC_AXIS_INFO[SAMPLE_CHANNELS];

            error = IMotion.IMotion_GetInfo(motion, out info);
            error = IMotion.IMotion_GetAxesInfo(motion, axisInfo, SAMPLE_CHANNELS);
            if (error == -1)
                Console.WriteLine("Device Open Failed (running emulation mode) !");
            else if( error > 0)
                Console.WriteLine("Driver Error (code : {0})", error);
            #endregion            
            
            IMotion.IMotion_Start(motion);

            /********* Motion Stream test (1st wave) *********/
            double wave;
            short[] streamSample = new short[SAMPLE_CHANNELS];

            #region Create motion data
            short[,] motionData = new short[SAMPLE_RATE, SAMPLE_CHANNELS];
            for (int i = 0; i < SAMPLE_RATE; i++)
            {
                wave = motion_amplitude * Math.Sin(2 * MotionTypes.IM_PI * (float)i / SAMPLE_RATE * motion_frequency);
                motionData[i, 0] = (short)(wave * MotionTypes.MOTION_MAX_16());
                motionData[i, 1] = (short)(wave * MotionTypes.MOTION_MAX_16());
                motionData[i, 2] = (short)(wave * MotionTypes.MOTION_MAX_16());
            }
            #endregion

            #region IMotion_SendStream Example (Heave - Roll - Pitch)
            for (int channel = 0; channel < SAMPLE_CHANNELS; channel++){
                for (int sampleRate = 0; sampleRate < SAMPLE_RATE; sampleRate++)
                {
                    streamSample[channel] = motionData[sampleRate, channel]; 

                    IMotion.IMotion_SendStream(motion, streamSample, sizeof(short) * SAMPLE_CHANNELS);
                    System.Threading.Thread.Sleep(1000 / SAMPLE_RATE);
                }

                Array.Clear(streamSample, 0, streamSample.Length);
                //IMotion.IMotion_SendStream(motion, streamSample, sizeof(short) * SAMPLE_CHANNELS);
                System.Threading.Thread.Sleep(1000);
            }
            #endregion


            /******** Motion Buffer test (2nd wave) *********/
            IM_FORMAT format = new IM_FORMAT();
            format.nSampleRate = SAMPLE_RATE;
            format.nChannels = SAMPLE_CHANNELS;
            format.nDataFormat = MotionTypes.IM_FORMAT_DATA_DEFAULT;
            IntPtr source = IMotion.IMotion_CreateSource(motion, ref format, ICallback, MotionTypes.IM_END_OF_BUFFER);

            #region Create submit buffer data
            short[] submitData = new short[SAMPLE_RATE * SAMPLE_CHANNELS];
            int index = 0;
            for(int sampleRate = 0; sampleRate < SAMPLE_RATE; sampleRate++){
                for(int sampleChannel = 0; sampleChannel < SAMPLE_CHANNELS; sampleChannel++){
                    submitData[index++] = motionData[sampleRate, sampleChannel];
                }
            }
            IntPtr submitData_ptr = Marshal.AllocHGlobal(submitData.Length * sizeof(short));
            Marshal.Copy(submitData, 0, submitData_ptr, submitData.Length);
            #endregion

            IM_BUFFER buffer = new IM_BUFFER();
            buffer.nMotionBytes = SAMPLE_RATE * SAMPLE_CHANNELS * sizeof(short);
            buffer.pMotionData = submitData_ptr;

            IMotion.IMotionSource_SubmitBuffer(source, ref buffer);
            IMotion.IMotionSource_Start(source);

            // Wait Queued Buffers 
            while (IMotion.IMotionSource_GetQueuedBufferCount(source) > 0)
            {
                System.Threading.Thread.Sleep(100);
                currentPos = IMotion.IMotionSource_GetInfo(source, out buffer);
            }

            #region IMotionSource_GetFormat Example
            Int32 res = IMotion.IMotionSource_GetFormat(source, out format);
            #endregion

            #region IMotion_GetDeviceDescription Example
            int devs = IMotion.IMotion_GetDeviceCount();
            for (int i = 0; i < devs; i++)
            {
                IMotion.IMotion_GetDeviceDescription(i, desc_ptr);
                desc = (IM_DEVICE_DESC)Marshal.PtrToStructure(desc_ptr, typeof(IM_DEVICE_DESC));
            }
            #endregion
            
            IMotion.IMotionSource_Stop(source);
            IMotion.IMotion_DestroySource(motion, source);

            IMotion.IMotion_Stop(motion, MotionTypes.IM_DEVICE_MOVE_NEUTRAL);
            IMotion.IMotion_Start(motion);

            /******** Motion Buffer test (3rd wave) *********/
            #region IMotion_LoadCSV Example
#if WIN32
            string url = "../../MotionData/waveform_sine.csv";
#else
            string url = "../../../MotionData/waveform_sine.csv";
#endif
            UInt32 dataLength = 0;
            IntPtr dataPtr = new IntPtr();

#if false   // IMotion_LoadCSV_RAW Test
            /* Load motion file (.csv) */
            byte[] byteArrayData = File.ReadAllBytes(url);
            if (IMotion.IMotion_LoadCSV_RAW(byteArrayData, byteArrayData.Length, out format, out dataPtr, out dataLength, null) == 0){
                Console.WriteLine("Couldn't load " + url.ToString());
                IMotion.IMotion_Shutdown();
                return;
            }
#else       // IMotion_LoadCSV Test
            /* Load motion file (.csv) */
            if (IMotion.IMotion_LoadCSV(url, out format, out dataPtr, out dataLength, null) == 0) {
                Console.WriteLine("Couldn't load " + url.ToString());
                IMotion.IMotion_Shutdown();
                return;
            }
#endif
            
            int ArraySize = (int)dataLength / MotionTypes.MOTION_SAMPLE_BYTE((int)format.nDataFormat);
            short[] data = new short[ArraySize];
            Marshal.Copy(dataPtr, data, 0, ArraySize);

            /* Create Source and Submit Buffer */
            source = IMotion.IMotion_CreateSource(motion, ref format, ICallback, MotionTypes.IM_END_OF_LOOP);
            IM_BUFFER loadBuffer = new IM_BUFFER();
            loadBuffer.nMotionBytes = dataLength;
            loadBuffer.pMotionData = dataPtr;
            loadBuffer.pContext = motion;
#if false // buffer loop test
            loadBuffer.nLoopCount = MotionTypes.IM_LOOP_INFINITE;
#endif
            IMotion.IMotionSource_SubmitBuffer(source, ref loadBuffer);

            /* Let the motion run */
            IMotion.IMotionSource_Start(source);
            while (!done && IMotion.IMotionSource_GetQueuedBufferCount(source) > 0){
                System.Threading.Thread.Sleep(100);
            }

            IMotion.IMotionSource_Stop(source);
            IMotion.IMotion_Stop(motion);

            IMotion.IMotion_FreeCSV(dataPtr);
            #endregion

            /* Clean up */
            IMotion.IMotion_DestroySource(motion, source);
            IMotion.IMotion_Destroy(motion);
            IMotion.IMotion_Shutdown();

            Marshal.FreeHGlobal(submitData_ptr);
            Marshal.FreeHGlobal(desc_ptr);
        }
    }
}
