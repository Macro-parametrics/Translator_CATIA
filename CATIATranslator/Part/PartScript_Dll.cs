using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace CATIATranslator
{
    class PartScript_Dll
    {
        public int TransStatus = 0;


        [DllImport("CATIApre_DLL.dll")]
        public static extern int CATIAPreStartService();
        [DllImport("CATIApre_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CATIAPreTranslate(string path);

        [DllImport("CATIApost_DLL.dll")]
        public static extern int CATIAPostStartService();
        [DllImport("CATIApost_DLL.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int CATIAPostTranslate(string path);
    }
}
