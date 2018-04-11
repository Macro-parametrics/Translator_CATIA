using System;
using System.IO;
using System.Windows.Forms;
using System.Threading.Tasks;

namespace CATIATranslator
{
    static class Program
    {
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new Form1());
            }

            else
            {
                
                if (args[0] == "pre")
                {
                    FileInfo fi = new FileInfo(args[1]);

                    if (fi.Exists)
                    {
                        Part PartManager = new Part();

                        if (PartManager.InitializeCATIA(args[1], (int)0))
                        {
                            PartManager.InitializeTransCAD((int)0);

                            PartManager.TranslateC2T(); //pre

                            PartManager.UninitializeCATIA();
                            PartManager.UninitializeTransCAD();
                        }
                    }
                }
                else if (args[0] == "post")
                {
                    Part PartManager = new Part();

                    if (PartManager.InitializeTransCAD((int)1))
                    {
                        PartManager.InitializeCATIA(args[1], (int)1);

                        PartManager.TranslateT2C(); //post

                        PartManager.UninitializeTransCAD();
                        PartManager.UninitializeCATIA();

                        PartManager.SaveCATPartFile(args[1]);
                    }
                }
                else if (args[0] == "assypre")
                {
                    FileInfo fi = new FileInfo(args[1]);

                    if (fi.Exists)
                    {
                        Part PartManager = new Part();

                        if (PartManager.InitializeCATIA(args[1], (int)2))
                        {
                            PartManager.InitializeTransCAD(3);

                            PartManager.TranslateC2T(); //pre

                            PartManager.UninitializeCATIA();
                            PartManager.UninitializeTransCAD();
                        }
                    }
                }
                else if (args[0] == "assypost")
                {
                    Part PartManager = new Part();
                    Assembly AssemblyManager = new Assembly();


                    if (AssemblyManager.InitializeTransCAD((int)1))
                    {
                        AssemblyManager.InitializeCATIA(args[1], (int)1);   //카티아 실행, 최초 초기화 실행 시간이 걸림

                        //AssemblyManager.TranslatePartsT2C();         더이상 사용되지 않는 함수 : Backup ver5_06을 참조
                        //AssemblyManager.TranslateConstraintsT2C();   더이상 사용되지 않느 함수 : Backup ver5_06을 참조

                    }
                }
            }
            Console.Read();
        }
    }
}
