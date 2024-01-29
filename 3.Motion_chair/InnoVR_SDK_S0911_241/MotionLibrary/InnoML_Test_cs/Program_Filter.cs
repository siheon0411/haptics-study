using System;
using System.Runtime.InteropServices;

using InnoMotion.Types;
using InnoMotion.Controller_InnoML;

namespace InnoML_test_cs
{
    class Program
    {
        static int PLATFORM_CHANNELS = 6;
        static int FORCE_CHANNELS = 6;
                
        static int SAMPLE_CHANNELS = 6;
        static int SAMPLE_RATE = MotionTypes.IM_FORMAT_SAMPLE_RATE_DEFAULT;
        static int SAMPLE_COUNT = 1;

        public static IMotionDebugCallback debugCallback = new IMotionDebugCallback(Program.Callback_Debug);
        public static void Callback_Debug(UInt32 id, string message, IntPtr userdata)
        {
            Console.WriteLine(message);
            return;
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

            Int32 master_buffer = InnoML.imCreateBuffer(SAMPLE_RATE, 0, PLATFORM_CHANNELS, SAMPLE_COUNT);
            Int32 context = InnoML.imCreateContext(master_buffer, 0, desc_ptr); // default device context

            // create device context
            InnoML.imSetContext(context); // make context current
            InnoML.imStart(); // start motion streaming (move center position)

            // get device description
            InnoML.imGetProfile(desc_ptr);
            desc = (IM_DEVICE_DESC)Marshal.PtrToStructure(desc_ptr, typeof(IM_DEVICE_DESC));

            // get device diagnostics
            IM_DIAGNOSTIC_AXIS_INFO[] descAxis = new IM_DIAGNOSTIC_AXIS_INFO[SAMPLE_CHANNELS];
            int error = InnoML.imGetDiagnostic(descAxis, SAMPLE_CHANNELS);
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

            /**** filter setup ****/
            Int32 noise_filter = InnoML.imCreateFilter(IM_FILTER_TYPE.IM_FILTER_NOISE);
            Int32 default_classical_washout = InnoML.imCreateFilter(IM_FILTER_TYPE.IM_FILTER_WASHOUT);
            Int32 simple_scaler = InnoML.imCreateFilter(IM_FILTER_TYPE.IM_FILTER_SCALE);
            Int32 platform_limiter = InnoML.imCreateFilter(IM_FILTER_TYPE.IM_FILTER_RATELIMIT);

            IM_FILTER_SCALE_PARAMS scaler_params = new IM_FILTER_SCALE_PARAMS(20);
            IntPtr scaler_params_ptr = Marshal.AllocHGlobal(Marshal.SizeOf(scaler_params));
            Marshal.StructureToPtr(scaler_params, scaler_params_ptr, false);
            InnoML.imFilterSetParams(simple_scaler, scaler_params_ptr, Marshal.SizeOf(typeof(IM_FILTER_SCALE_PARAMS)), 1);
            Marshal.FreeHGlobal(scaler_params_ptr);

            /**** Force Simulation (Washout Filter) ****/
            // It is usually used to convert logical motion data to physical motion data. (force -> motion)
            Int32 input_buffer = InnoML.imCreateBuffer(SAMPLE_RATE, MotionTypes.IM_FORMAT_DATA_S16, FORCE_CHANNELS, SAMPLE_COUNT);
            Int32 input = InnoML.imCreateInput(input_buffer);

            if (true) // input filter (default washout)
            {
                Int32 filter = InnoML.imCreateFilter();
                InnoML.imFilterAppend(filter, noise_filter);
                InnoML.imFilterAppend(filter, default_classical_washout);
                InnoML.imFilterAppend(filter, simple_scaler);
                InnoML.imFilterAppend(filter, platform_limiter);

                // Filters the output of the motion input.
                InnoML.imInputSetFilter(input, filter);
                InnoML.imInputStart(input); // filter build

                short[] force = new short[FORCE_CHANNELS];
                int dt = 1000 / SAMPLE_RATE;
                for (int time = 0; time <= 3000; time += dt) // 3 sec washout
                {
                    // Press and hold the accelerator for 1 second.
                    double acceleration = (time < 1000) ? 1.0f : 0;	// sampling (-1~1)

                    // 0 : Surge, 1 : Sway, 2 : Heave   (acceleration, m/s^2) 
                    // 3 : Roll, 4 : Pitch, 5 : Yaw     (angular velocity, radians/s)
                    force[0] = (short)(MotionTypes.MOTION_MAX_16() * acceleration); // Surge
                    //force[1] = (short)(MotionTypes.MOTION_MAX_16() * acceleration); // Sway
                    //force[2] = (short)(MotionTypes.MOTION_MAX_16() * acceleration); // Heave

                    InnoML.imBufferEnqueue(input_buffer, force, force.Length * 2);
                    // The real-time sampling time is ideal for signals that are less than half the device sample time.
			        System.Threading.Thread.Sleep((1000 / SAMPLE_RATE) >> 1);
                }
                InnoML.imInputStop(input);

                InnoML.imDeleteFilter(filter);
                Console.WriteLine("Motion Input filtering completed ... \n");
            }
            InnoML.imDeleteBuffer(input_buffer);
            InnoML.imDeleteInput(input);

            /* Clean up */
            InnoML.imStop(); // stop motion streaming (move init position)
            InnoML.imDeleteBuffer(master_buffer);
            InnoML.imSetContext(0); // release context
            InnoML.imDestroyContext(context, 0); // shutdown device	
            return;
        }
    }
}
