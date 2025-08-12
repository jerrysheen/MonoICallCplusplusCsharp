using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameScripts
{
    public class HelloWorld
    {
        public static void SayHello()
        {
            Console.WriteLine("Hello from C# script!");
        }

        public static string GetMessage(string name)
        {
            return $"Hello, {name}! From C# script.";
        }
    }
}
