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
            string filePath = CATPartOpenDialog();

            if (filePath != "")
            {
                Assembly AssemblyManager = new Assembly();

                //if (PartManager.InitializeCATIA(filePath, (int)0))
                //{
                //    PartManager.InitializeTransCAD((int)0);

                //    PartManager.TranslateC2T();

                //    PartManager.UninitializeCATIA();
                //    PartManager.UninitializeTransCAD();
                //}
            }
        }

        // Post-Processor Assembly
        private void button4_Click_1(object sender, EventArgs e)
        {
            string filePath = CATAssemblyOpenDialog();

            if (filePath != "")
            {
                Assembly AssemblyManager = new Assembly();

                //if (PartManager.InitializeCATIA(filePath, (int)0))
                //{
                //    PartManager.InitializeTransCAD((int)0);

                //    PartManager.TranslateC2T();

                //    PartManager.UninitializeCATIA();
                //    PartManager.UninitializeTransCAD();
                //}
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
