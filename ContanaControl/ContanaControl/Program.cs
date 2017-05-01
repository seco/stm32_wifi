using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;

namespace ContanaControl
{
    class Program
    {
        static void Main(string[] args)
        {
            SerialPort port = new SerialPort("COM4", 115200, Parity.None, 8, StopBits.One);
            port.Open();
            port.Write("close");
            port.Close();
        }
    }
}
