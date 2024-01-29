using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using InnoMotion.Types;
using InnoMotion.Controller_InnoML;

namespace InnoML_test_cs
{
    class Program
    {
        static int SAMPLE_CHANNELS = MotionTypes.IM_FORMAT_CHANNELS_DEFAULT;
        static int SAMPLE_RATE = MotionTypes.IM_FORMAT_SAMPLE_RATE_DEFAULT;
        static int SAMPLE_COUNT = 4;

        public static IMotionDebugCallback debugCallback = new IMotionDebugCallback(Program.Callback_Debug);
        public static void Callback_Debug(UInt32 id, string message, IntPtr userdata)
        {
            Console.WriteLine(message);
            return;
        }

        public static bool timeout = false;
        public static IM_DIAGNOSTIC_AXIS_INFO[] info = new IM_DIAGNOSTIC_AXIS_INFO[3];
        public static IMotionCallback notifyCallback = new IMotionCallback(Program.Callback_Notify);
        public static void Callback_Notify(uint context_ptr, UInt32 state)
        {
            int error = InnoML.imGetDiagnostic(info, 3);
            if (state == MotionTypes.IM_END_OF_STREAM)
                timeout = true;
        }


        static void Main(string[] args)
        {
            /* Start up */
            // set device profile
            IM_DEVICE_DESC desc = new IM_DEVICE_DESC();
            desc.szName = "Inno Motion 6DOF";
            desc.nOptions |= MotionTypes.IM_CFG_DEBUG_MODE;
            IntPtr desc_ptr = Marshal.AllocHGlobal(Marshal.SizeOf(desc));
            Marshal.StructureToPtr(desc, desc_ptr, false);

            // Set log function
            InnoML.imSetLogFunction(debugCallback);

            Int32 master_buffer = InnoML.imCreateBuffer(SAMPLE_RATE, 0, 0, SAMPLE_COUNT, 2);
            Int32 context = InnoML.imCreateContext(master_buffer, 0, desc_ptr); // default device context

            // create device context
            InnoML.imSetContext(context); // make context current
            InnoML.imStart(); // start motion streaming (move center position)

            // get device description
            int devDscCount = InnoML.imGetDescription(desc_ptr);
            desc = (IM_DEVICE_DESC)Marshal.PtrToStructure(desc_ptr, typeof(IM_DEVICE_DESC));

            // get device diagnostics
            IM_DIAGNOSTIC_AXIS_INFO[] descAxis = new IM_DIAGNOSTIC_AXIS_INFO[MotionTypes.IM_FORMAT_CHANNELS_DEFAULT];
            int error = InnoML.imGetDiagnostic(descAxis, MotionTypes.IM_FORMAT_CHANNELS_DEFAULT);
            if (error < 0)
            {
                if (error == -1)
                    Console.WriteLine("Device Open Failed (running emulation mode) !");
                else
                {
                    Console.WriteLine("Device Driver Error (code : {0})", error);
                    return;
                }
            }

            /**** Motion Stream test (1st wave) ****/
            #region Motion Stream TEST
            double wave;
            short[,] buf = new short[SAMPLE_RATE, SAMPLE_CHANNELS];
            short[] sampleBuf = new short[SAMPLE_CHANNELS];
            for (int sampleRate = 0; sampleRate < SAMPLE_RATE; sampleRate++)
            {
                wave = Math.Sin(2 * MotionTypes.IM_PI * (float)sampleRate / SAMPLE_RATE);
                buf[sampleRate, 0] = (short)(wave * MotionTypes.MOTION_MAX_16());
                sampleBuf[0] = buf[sampleRate, 0];

#if true //  manually motion operation test
                InnoML.imBufferEnqueue(master_buffer, sampleBuf, sizeof(short) * SAMPLE_CHANNELS);
                System.Threading.Thread.Sleep(1000 / SAMPLE_RATE);
#endif
            }
            Console.WriteLine("1st wave completed ... \n\n");
            #endregion

            /**** Motion Buffer test (2nd wave) ****/
            #region Motion Buffer TEST
            Int32 buffer = InnoML.imCreateBuffer(SAMPLE_RATE, 0, 0, SAMPLE_RATE);
            InnoML.imBufferEnqueue(buffer, buf, buf.Length * sizeof(short));	// fillup motion data to buffer
            Int32 duration = InnoML.imBufferGetDuration(buffer);		        // get playtime (ms)


            // Play Source 
            Int32 source = InnoML.imCreateSource(buffer);
            InnoML.imSourcePlay(source, 0); // IM_LOOP_INFINITE

            while (InnoML.imGetPlayingSourceCount() > 0)
            {	// wait playlist count (sync)
                Int32 pos = InnoML.imSourceGetPosition(source);	// get position
                Console.WriteLine("*** Play Position ({0}) : {0}/{0} ms ***\n", pos * 100 / duration, pos, duration);
                System.Threading.Thread.Sleep(100);
            }

            InnoML.imSourceStop(source);
            InnoML.imDeleteSource(source);
            InnoML.imDeleteBuffer(buffer);
            Console.WriteLine("2nd wave completed ... \n\n");
            #endregion

            /**** Motion Source Player test (3rd wave) ****/
            #region Motion Effect Player TEST
#if WIN64
            string url = "../../../MotionData/waveform_sine.csv";
#else
            string url = "../../MotionData/waveform_sine.csv";
#endif
            buffer = InnoML.imLoadBuffer(url);
            source = InnoML.imCreateSource(buffer);
            duration = InnoML.imBufferGetDuration(buffer);

            if (true) // load url
            {
                Console.WriteLine("<Simple Play Test> \n");
                InnoML.imSourcePlay(source);
                while (InnoML.imGetPlayingSourceCount() > 0)
                {
                    System.Threading.Thread.Sleep(100);
                }
                InnoML.imSourceStop(source);
                Console.WriteLine("3rd wave completed (load url) ... \n\n");
            }

            if (true) // load file memory
            {
                byte[] data = File.ReadAllBytes(url);
                Int32 buffer2 = InnoML.imLoadBufferMemory(data, data.Length);
                duration = InnoML.imBufferGetDuration(buffer2);

                InnoML.imSourceSetBuffer(source, buffer2);
                InnoML.imSourcePlay(source);
                while (InnoML.imSourceGetPosition(source) < duration)
                {
                    Console.WriteLine("position ({0}/{1} ms)", InnoML.imSourceGetPosition(source), duration);
                    System.Threading.Thread.Sleep(100);
                }

                InnoML.imSourceStop(source);
                InnoML.imDeleteBuffer(buffer2);
                Console.WriteLine("3rd wave completed (load memory) ... \n\n");
            }

            if (true) // replay/volume/position/event/speed	
            {
                Console.WriteLine("<Additional Playback Tests> \n");
                InnoML.imSourceSetVolume(source, 50);	// volume (0~100)
                InnoML.imSourceSetSpeed(source, 50);	// speed (1/2x)
                InnoML.imSourcePlay(source, 0, notifyCallback);
                while (!timeout)
                {	// precise timeout by event callback (async)
                    int pos = InnoML.imSourceGetPosition(source);		// position
                    Console.WriteLine("position ({0}/{1} ms) \n", pos, duration);
                    System.Threading.Thread.Sleep(100);
                }
                Console.WriteLine("2nd wave completed ... \n\n");
            }

            if (true) // pause/resume/play/stop/loop
            {
                Console.WriteLine("<Playback State Tests> \n");
                InnoML.imSourcePlay(source, MotionTypes.IM_LOOP_INFINITE); // play loop
                Console.WriteLine("play (infinite repeat) ... \n");
                System.Threading.Thread.Sleep(1000);
                InnoML.imSourcePause(source, 1);	// pause
                Console.WriteLine("pause (1 sec) ... \n");
                System.Threading.Thread.Sleep(1000);
                InnoML.imSourcePause(source, 0);	// resume
                Console.WriteLine("resume (1 sec) ... \n");
                System.Threading.Thread.Sleep(1000);
                InnoML.imSourceStop(source);	// stop
                Console.WriteLine("stop (repeat break) ... \n");
                Console.WriteLine("3rd wave completed ... \n\n");
            }

            InnoML.imSourceStop(source);
            InnoML.imDeleteSource(source);
            InnoML.imDeleteBuffer(buffer);
            #endregion

            /* Clean up */
            InnoML.imStop(); // stop motion streaming (move init position)
            InnoML.imSetContext(0); // release context
            InnoML.imDestroyContext(context, 0); // shutdown device	
            InnoML.imDeleteBuffer(master_buffer);
            Marshal.FreeHGlobal(desc_ptr);
        }
    }
}
