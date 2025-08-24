using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GameScripts
{
    //[System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential, Pack=1)]
    //public struct VertexStruct
    //{
    //    public float positionx;
    //    public float positiony;
    //    public float positionz;
    //    public float normalx;
    //    public float normaly;
    //    public float normalz;
    //    public float normalw;
    //}

    public class Component
    {
        public IntPtr _nativeHandle = IntPtr.Zero;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern IntPtr NativeCallComponentNew(int id, string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void NativeCallComponentDelete(IntPtr nativeHandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int NativeCallGetId(IntPtr nativeHandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void NativeCallSetId(IntPtr nativeHandle, int id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string NativeCallGetTagInternal(IntPtr nativeHandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void NativeCallSetTagInternal(IntPtr nativeHandle, string tag);
        
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Component NativeDirectCreateNewAndReturn(string tag);


        //[MethodImpl(MethodImplOptions.InternalCall)]
        //public static extern void NativeCallSetVertex(VertexStruct[] vertexStruct);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static unsafe extern void NativeCallSetFloatArray(float* data, int length, float scale);

        public int Id
        {
            get => NativeCallGetId(_nativeHandle);
            set => NativeCallSetId(_nativeHandle, value);
        }

        public string Tag
        {
            get => NativeCallGetTagInternal(_nativeHandle);
            set => NativeCallSetTagInternal(_nativeHandle, value);
        }

        public Component() 
        {
            _nativeHandle = (IntPtr)1523;
        }

        public Component(int id, string tag)
        {
            _nativeHandle = NativeCallComponentNew(id, tag);

            float[] data = new float[] { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
            unsafe
            {
                fixed (float* p = data)
                {
                    NativeCallSetFloatArray(p, data.Length, 2.0f);
                }
            }
            if (_nativeHandle != IntPtr.Zero)
            {
                Console.ForegroundColor = ConsoleColor.Green;
               // Console.WriteLine($"ComponentWrapper #{id} constructed in managed code with tag: {Tag}!");
                Console.ForegroundColor = ConsoleColor.White;
            }
        }

        ~Component()
        {
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine($"ComponentWrapper #{Id} deconstructed in managed code!");
            Console.ForegroundColor = ConsoleColor.White;
            NativeCallComponentDelete(_nativeHandle);
            _nativeHandle = IntPtr.Zero;
        }
    }
}
