using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Text;
using TransCAD;

namespace CATIATranslator
{

    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
/*****************************************************************************************/
/*****************************************************************************************/

        // Pre-Processor CATPart
        private void button1_Click(object sender, EventArgs e)
        {
            string filePath = CATPartOpenDialog();

            if (filePath != "")
            {
                Part PartManager = new Part();

                if (PartManager.InitializeCATIA(filePath, (int)0))
                {
                    PartManager.InitializeTransCAD((int)0);

                    PartManager.TranslateC2T();

                    PartManager.UninitializeCATIA();
                    PartManager.UninitializeTransCAD();
                }
            }
        }

        // Post-Processor CATPart
        private void button2_Click(object sender, EventArgs e)
        {
            string filePath = CATPartSaveDialog();

            if (filePath != "")
            {
                Part PartManager = new Part();

                if (PartManager.InitializeTransCAD((int)1))
                {
                    PartManager.InitializeCATIA(filePath, (int)1);

                    PartManager.TranslateT2C();

                    PartManager.UninitializeTransCAD();
                    PartManager.UninitializeCATIA();

                    PartManager.SaveCATPartFile(filePath);
                }
            }
        }
/*****************************************************************************************/
/*****************************************************************************************/

        // Pre-Processor Assembly
        private void button3_Click_1(object sender, EventArgs e)
        {


            //string filePath = CATPartOpenDialog();


            /*
           파일 경로
           "D:\project\Macro\TestAssem\SubAssembly1\RotationPart.CATPart"
           "D:\project\Macro\TestAssem\SubAssembly1\Body.CATPart"

           coaxial - catCstTypeOn
           "Product1/Part1.1/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))"
           "Product1/Part1.2/!Axis:(Selection_RSur:(Face:(Brp:(Pad.2;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pad.2_ResultOUT;Z0;G3563))"
           incidence - catCstTypeSurfContact
           "Product1/Part1.1/!Selection_RSur:(Face:(Brp:(Pad.1;1);None:();Cf11:());Pad.1_ResultOUT;Z0;G3563))"
           "Product1/Part1.2/!Selection_RSur:(Face:(Brp:(Pad.1;2);None:();Cf11:());Pad.1_ResultOUT;Z0;G3563))"
            */

            // List<PreBuffer> stack = new List<PreBuffer>();
            // stack.Add(new PreBuffer("D:\\project\\Macro\\TestAssem\\SubAssembly1\\RotationPart.CATPart","Part1","Rotation"));
            // stack.Add(new PreBuffer("D:\\project\\Macro\\TestAssem\\SubAssembly1\\Body.CATPart","Part1","Body"));
            PreStack stack = new PreStack();
            stack.Clear();
            //  stack.StackItem("D:\\project\\Macro\\TestAssem\\SubAssembly1\\RotationPart.CATPart", "Part1", "RotationPart");
            //  stack.StackItem("D:\\project\\Macro\\TestAssem\\SubAssembly1\\Body.CATPart", "Part1", "Body");
            stack.StackItem("D:\\project\\Macro\\TestAssem\\SubAssembly1\\RotationPart.CATPart", "Part1", "RotationPart");
            stack.StackItem("D:\\project\\Macro\\TestAssem\\SubAssembly1\\Body.CATPart", "Part1", "Body");

            ReferenceClass.ref_Pre m_refer = new ReferenceClass.ref_Pre();
            //m_refer.PreTest(stack,stack.GetSize(), "catCstTypeOn", "Product1/Part1.1/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))", "Product1/Part1.2/!Axis:(Selection_RSur:(Face:(Brp:(Pad.2;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pad.2_ResultOUT;Z0;G3563))");
             m_refer.SetConstraint(stack, stack.GetSize(), "catCstTypeOn", "Product1/Part1.1/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))", "Product1/Part1.2/!Axis:(Selection_RSur:(Face:(Brp:(Pad.2;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pad.2_ResultOUT;Z0;G3563))", "move", 0);
             m_refer.SetConstraint(stack, stack.GetSize(), "catCstTypeSurfContact", "Product1/Part1.1/!Selection_RSur:(Face:(Brp:(Pad.1;1);None:();Cf11:());Pad.1_ResultOUT;Z0;G3563))", "Product1/Part1.2/!Selection_RSur:(Face:(Brp:(Pad.1;2);None:();Cf11:());Pad.1_ResultOUT;Z0;G3563))", "", 0);
            // m_refer.SetConstraint(stack, stack.GetSize(), "catCstTypeOn", "Product1/Part1.2/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());EdgeFillet.1_ResultOUT;Z0;G4162))", "Product1/Part1.1/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());EdgeFillet.1_ResultOUT;Z0;G4162))", "move", 0);
            //m_refer.SetConstraint(stack, stack.GetSize(), "catCstTypeSurfContact", "Product1/Part1.2/!Selection_RSur:(Face:(Brp:(Pad.1;0:(Brp:(Sketch.1;8)));None:();Cf11:());EdgeFillet.1_ResultOUT;Z0;G4162)", "Product1/Part1.1/!Selection_RSur:(Face:(Brp:(Pad.1;0:(Brp:(Sketch.1;8)));None:();Cf11:());EdgeFillet.1_ResultOUT;Z0;G4162)", "same", 0);
            //m_refer.SetConstraint(stack, stack.GetSize(), "catCstTypeSurfContact", "Product1/Part1.2/!Selection_RSur:(Face:(Brp:(Pad.1;0:(Brp:(Sketch.1;8)));None:();Cf11:());EdgeFillet.1_ResultOUT;Z0;G4162)", "Product1/Part1.1/!Selection_RSur:(Face:(Brp:(Pad.1;0:(Brp:(Sketch.1;8)));None:();Cf11:());EdgeFillet.1_ResultOUT;Z0;G4162)", "", 0);
            //m_refer.SetConstraint(stack, stack.GetSize(), "catCstTypeSurfContact", "Product1/Part1.2/!Selection_RSur:(Face:(Brp:(Pad.1;2);None:();Cf11:());EdgeFillet.1_ResultOUT;Z0;G4162)", "Product1/Part1.1/!Selection_RSur:(Face:(Brp:(Pad.1;2);None:();Cf11:());EdgeFillet.1_ResultOUT;Z0;G4162)", "same", 0);
            
        }

        // Post-Processor Assembly
        private void button4_Click_1(object sender, EventArgs e)
        {
            //string filePath = CATAssemblyOpenDialog();
            string filePath = CATScriptSaveDialog();

            string EventMethod = "Use Script";//"Use Script"

            if (EventMethod == "Use API")
            {
                //더이상 사용되지 않는 코드
                /*
                if (filePath != "")
                {
                    Part PartManager = new Part();
                    Assembly AssemblyManager = new Assembly();


                    if (AssemblyManager.InitializeTransCAD((int)1))
                    {
                        AssemblyManager.InitializeCATIA(filePath, (int)1);   //카티아 실행, 최초 초기화 실행 시간이 걸림

                        AssemblyManager.TranslatePartsT2C(filePath);
                        AssemblyManager.TranslateConstraintsT2C(filePath);

                    }
                }
                */
            }
            else if (EventMethod == "Use Script")
            {

                    //API를 사용하는 방법 및 Script를 사용하는 방법 통합 버전
                    AssemblyPartManager dlg = new AssemblyPartManager(filePath);
                    dlg.ShowDialog();
            }

        }


        /*****************************************************************************************/
        /*****************************************************************************************/

        PartScript_Dll m_DLL = new PartScript_Dll();

        // Pre-Processor CATScript
        private void button5_Click_1(object sender, EventArgs e)
        {
            // Chiho : CATIA Translator preprocessor.dll
              
            //get path
            string filePath = CATScriptOpenDialog();
            
            if (filePath != "")
            {
                //////////////////Run TransCAD//////////////////   
                if (m_DLL.TransStatus == 0)
                {
                    TransCAD.IApplication g_spApplication = new TransCAD.Application();
                    g_spApplication.Visible = true;
                    m_DLL.TransStatus = 1;
                }
                 
                //Part PartManager = new Part();
                //PartManager.InitializeTransCAD((int)0);

                ////////////////Translate/////////////////////////
                PartScript_Dll.CATIAPreStartService();
                PartScript_Dll.CATIAPreTranslate(filePath);

                //PartManager.UninitializeTransCAD();


            }
            
        }

        // Post-Processor CATScript
        private void button6_Click_1(object sender, EventArgs e)
        {
            // Chiho : CATIA Translator postprocessor.dll
            string filePath = CATScriptSaveDialog();

            PartScript_Dll.CATIAPostStartService();
            int status = PartScript_Dll.CATIAPostTranslate(filePath);
            if (status == 0)
            {
                MessageBox.Show("Make TransCAD Model First");
            }
            else {
                m_DLL.TransStatus = 1;
            }
        }



/*****************************************************************************************/
/*****************************************************************************************/


        public string CATPartOpenDialog()
        {
            //파일 열기창 생성 및 설정
            OpenFileDialog ofd = new OpenFileDialog();
            
            ofd.Title = "CATIA Part 파일 열기";
            ofd.FileName = "";
            ofd.Filter = "CATIA Part 파일 (*.CATPart) | *.CATPart";

            //파일 열기창 로드
            DialogResult dr = ofd.ShowDialog();

            //OK버튼 클릭 시
            if (dr == DialogResult.OK)
            {
                //File경로와 File명을 모두 가지고 온다.
                string filePath = ofd.FileName;

                return filePath;
            }
            //취소 버튼 클릭 시 또는 ESC키로 파일 창을 종료 했을 경우
            else if (dr == DialogResult.Cancel)
            {
                return "";
            }

            return "";
        }

        public string CATPartSaveDialog()
        {
            //파일 저장창 생성 및 설정
            SaveFileDialog ofd = new SaveFileDialog();
            
            ofd.Title = "CATIA Part 파일 저장";
            ofd.FileName = "";
            ofd.Filter = "CATIA Part 파일 (*.CATPart) | *.CATPart";

            //파일 저장창 로드
            DialogResult dr = ofd.ShowDialog();

            //OK버튼 클릭 시
            if (dr == DialogResult.OK)
            {
                //File경로와 File명을 모두 가지고 온다.
                string filePath = ofd.FileName;

                return filePath;
            }
            //취소 버튼 클릭 시 또는 ESC키로 파일 창을 종료 했을 경우
            else if (dr == DialogResult.Cancel)
            {
                return "";
            }

            return "";
        }

        public string CATScriptOpenDialog()
        {
            //파일 열기창 생성 및 설정
            OpenFileDialog ofd = new OpenFileDialog();

            ofd.Title = "CATIA Script 파일 열기";
            ofd.FileName = "";
            ofd.Filter = "CATIA Script 파일 (*.CATScript) | *.CATScript";

            //파일 열기창 로드
            DialogResult dr = ofd.ShowDialog();

            //OK버튼 클릭 시
            if (dr == DialogResult.OK)
            {
                //File경로와 File명을 모두 가지고 온다.
                string filePath = ofd.FileName;

                return filePath;
            }
            //취소 버튼 클릭 시 또는 ESC키로 파일 창을 종료 했을 경우
            else if (dr == DialogResult.Cancel)
            {
                return "";
            }

            return "";
        }

        public string CATScriptSaveDialog()
        {
            //파일 저장창 생성 및 설정
            SaveFileDialog ofd = new SaveFileDialog();

            ofd.Title = "CATIA Script 파일 열기";
            ofd.FileName = "";
            ofd.Filter = "CATIA Script 파일 (*.CATScript) | *.CATScript";

            //파일 저장창 로드
            DialogResult dr = ofd.ShowDialog();

            //OK버튼 클릭 시
            if (dr == DialogResult.OK)
            {
                //File경로와 File명을 모두 가지고 온다.
                string filePath = ofd.FileName;

                return filePath;
            }
            //취소 버튼 클릭 시 또는 ESC키로 파일 창을 종료 했을 경우
            else if (dr == DialogResult.Cancel)
            {
                return "";
            }

            return "";
        }

        public string CATAssemblyOpenDialog()
        {
            ////파일 열기창 생성 및 설정
            //OpenFileDialog ofd = new OpenFileDialog();

            //ofd.Title = "CATIA Part 파일 열기";
            //ofd.FileName = "";
            //ofd.Filter = "CATIA Part 파일 (*.CATPart) | *.CATPart";

            ////파일 열기창 로드
            //DialogResult dr = ofd.ShowDialog();

            ////OK버튼 클릭 시
            //if (dr == DialogResult.OK)
            //{
            //    //File경로와 File명을 모두 가지고 온다.
            //    string filePath = ofd.FileName;

            //    return filePath;
            //}
            ////취소 버튼 클릭 시 또는 ESC키로 파일 창을 종료 했을 경우
            //else if (dr == DialogResult.Cancel)
            //{
            //    return "";
            //}

            return "";
        }

        public string CATAssemblySaveDialog()
        {
            ////파일 저장창 생성 및 설정
            //SaveFileDialog ofd = new SaveFileDialog();

            //ofd.Title = "CATIA Part 파일 저장";
            //ofd.FileName = "";
            //ofd.Filter = "CATIA Part 파일 (*.CATPart) | *.CATPart";

            ////파일 저장창 로드
            //DialogResult dr = ofd.ShowDialog();

            ////OK버튼 클릭 시
            //if (dr == DialogResult.OK)
            //{
            //    //File경로와 File명을 모두 가지고 온다.
            //    string filePath = ofd.FileName;

            //    return filePath;
            //}
            ////취소 버튼 클릭 시 또는 ESC키로 파일 창을 종료 했을 경우
            //else if (dr == DialogResult.Cancel)
            //{
            //    return "";
            //}

            return "";
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
        
        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }
    }
}
