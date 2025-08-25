using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameScripts
{
    public static class ManagedClass
    {
        public static void Main()
        {
            for (int i = 0; i < 3; i++)
            {
                var comp = new Component(i, $"Tag {i * 10}");
            }
           
            Component componentFromC = Component.NativeDirectCreateNewAndReturn("Test");
            Component.SetManagedObjectValue(componentFromC);
            Console.WriteLine($"_TestObject #{componentFromC._TestObject} value"); ;
            Console.WriteLine($"_nativeHandle #{componentFromC._nativeHandle} id"); ;
        }
    }
}
