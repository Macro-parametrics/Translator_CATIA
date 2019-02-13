using System;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Text;
using TransCAD;

//using System.Diagnostics;

//part information struct 
public struct PartsInfo
{
    public string address; 
    public string catname; 
    public string transname;
    public string ins_name;

};

//constraint information struct
public struct ConstraintInfo
{
    public string name;
    public string type;
    public string master_ref;
    public string slave_ref;
};

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
            int constraint_num = 0;
            ConstraintInfo[] a1_constraint = new ConstraintInfo[100]; //조만간 제거할거임.

            //Assembly file: bring CATScript and address
            string CATAssem = CATScriptOpenDialog(); //Assembly CATScript address   -> 수정필요
            if (CATAssem == "") return;
            string folder_address = CATAssem.Substring(0, CATAssem.LastIndexOf("\\"));
            Console.WriteLine(folder_address); //Assembly folder address.

            string line;
            string search2 = "AddBiEltCst";

            //Part file: bring address of parts folder
            string CATAssem2 = CATScriptOpenDialog();  //-> 수정 필요
            if (CATAssem2 == "") return;
            string folder_address_parts = CATAssem2.Substring(0, CATAssem2.LastIndexOf("\\"));
            Console.WriteLine(folder_address_parts); //part folder address.
             
              //constraint type parsing  //조만간 지울거임.
            using (StreamReader sr = new StreamReader(CATAssem, System.Text.Encoding.Default))
            {
                while ((line = sr.ReadLine()) != null)
                {

                    if (line.Length > 10)  //수정 가능
                    {

                        if (line.IndexOf(search2) != -1)
                        {
                            //constraint type parsing
                            a1_constraint[constraint_num].type = line.Substring(line.IndexOf("(") + 1, line.IndexOf(",") - line.IndexOf("(") - 1);
                            //Console.WriteLine(a1_constraint[constraint_num].type);
                            //master_ref parsing(numbering)
                            a1_constraint[constraint_num].master_ref = line.Substring(line.IndexOf(",") + 2, line.LastIndexOf(",") - line.IndexOf(",") - 2);
                            //Console.WriteLine(a1_constraint[constraint_num].master_ref);
                            //slave_ref parsing(numbering)
                            a1_constraint[constraint_num].slave_ref = line.Substring(line.LastIndexOf(",") + 2, line.IndexOf(")") - line.LastIndexOf(",") - 2);
                            //Console.WriteLine(a1_constraint[constraint_num].slave_ref);

                            //number of constraints
                            constraint_num++;
                        }
                    }

                }
            }

            // reparsing master_ref, slave_ref(contents)  //조만간 지울거임.
            using (StreamReader sr = new StreamReader(CATAssem, System.Text.Encoding.Default))
            {
                while ((line = sr.ReadLine()) != null)
                {
                    for (int n = 0; n < constraint_num; n++)
                    {
                        if (line.IndexOf("CreateReference") != -1)
                        {
                            if (line.IndexOf(a1_constraint[n].master_ref) != -1)
                            {
                                a1_constraint[n].master_ref = line.Substring(line.IndexOf("\"") + 1, line.LastIndexOf("\"") - line.IndexOf("\"") - 1);
                                //Console.WriteLine(a1_constraint[n].master_ref);
                            }
                            else if (line.IndexOf(a1_constraint[n].slave_ref) != -1)
                            {
                                a1_constraint[n].slave_ref = line.Substring(line.IndexOf("\"") + 1, line.LastIndexOf("\"") - line.IndexOf("\"") - 1);
                                //Console.WriteLine(a1_constraint[n].slave_ref);
                            }
                        }
                    }
                }
            }


            //part information storage
            List<PartsInfo> part_collect = new List<PartsInfo>();
            List<ConstraintInfo> const_collect = new List<ConstraintInfo>();

            //CATProduct location
            Assembly Product1 = new Assembly();
            string product_address = folder_address + "\\A2.CATProduct"; //수정 필요.
            Console.WriteLine(product_address);

            //CATProduct 열어서 part number, instance name, address 가져오기.
            //constraint 갯수, 종류, master ref, slave ref 구하기.
            int part_numb = 0;
            int const_numb = 0;

            if (Product1.InitializeCATIA(product_address, (int)0))
            {
                Console.WriteLine("working!");
                Console.WriteLine(Product1.cProduct.get_Name()); //product name

                part_numb = Product1.cProducts.Count; // # of parts
                Console.WriteLine(part_numb);

                const_numb = Product1.cConstraints.Count; // # of constraints
                Console.WriteLine(const_numb);

                if (part_numb > 0)  //파트 정보
                {

                    for (int i = 1; i <= part_numb; i++)
                    {
                        Console.WriteLine("");

                        string first = Product1.cProducts.Item(i).get_Name(); //instance name
                        Console.WriteLine(first);

                        string first_part = Product1.cProducts.Item(i).get_PartNumber(); //part number
                        Console.WriteLine(first_part);

                        string first_address = Product1.cProducts.Item(i).ReferenceProduct.GetMasterShapeRepresentationPathName(); //address of part 수정 필요!!!!
                        Console.WriteLine(first_address);

                        string first_transcad= first_address.Substring(first_address.LastIndexOf("\\")+1, first_address.LastIndexOf(".")- first_address.LastIndexOf("\\")-1);
                        Console.WriteLine(first_transcad);

                        PartsInfo temp = new PartsInfo();
                        temp.ins_name = first; temp.catname = first_part; temp.address = first_address; temp.transname = first_transcad;

                        part_collect.Add(temp);

                    }
                } 

                if (const_numb > 0)  //구속 정보
                {
                    Console.WriteLine("");
                    string second = Product1.cConstraints.Name;
                    Console.WriteLine(second);

                    for (int i = 1; i <= const_numb; i++)
                    {
                        Console.WriteLine("");
                        ConstraintInfo temp = new ConstraintInfo();

                        //constraint name
                        string second2 = Product1.cConstraints.Item(i).get_Name();
                        temp.name = second2;
                        Console.WriteLine(second2);

                        //constraint type
                        string second2t = Product1.cConstraints.Item(i).GetType().Name;
                        temp.type = second2t;
                        //Console.WriteLine(second2t);

                        //master ref
                        //slave ref

                    }
                }

                Product1.UninitializeCATIA();
            }

            ///////////////////////
            //파트 번역..CATParts//
            ///////////////////////


            //parts information
            PreStack stack = new PreStack();
            stack.Clear();
            for (int i = 0; i < part_numb; i++)
            {
                stack.StackItem(part_collect[i].address, part_collect[i].catname, part_collect[i].transname);
            }

            int modeya = 0; //constraint 번역 못하게 막음.

            if(modeya != 0)
            {
                //constraint information
                ReferenceClass.ref_Pre m_refer = new ReferenceClass.ref_Pre(stack);
                //Set constraint1 = constraints1.AddBiEltCst(catCstTypeOn, reference1, reference2)
                m_refer.SetConstraint(stack, stack.GetSize(), a1_constraint[0].type, a1_constraint[0].master_ref, a1_constraint[0].slave_ref, "move", 0); //수정 필요. "move"???
                m_refer.SetConstraint(stack, stack.GetSize(), a1_constraint[1].type, a1_constraint[1].master_ref, a1_constraint[1].slave_ref, "", 0); //수정 필요.

            }


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
                ReferenceClass.ref_Pre.ImportPart_Script(filePath);

                /*
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
                */

            }
            
        }

        // Post-Processor CATScript
        private void button6_Click_1(object sender, EventArgs e)
        {
            // Chiho : CATIA Translator postprocessor.dll
            string filePath = CATScriptSaveDialog();

            ReferenceClass.ref_Post.ExportPart(filePath);


            /*
            PartScript_Dll.CATIAPostStartService();
            int status = PartScript_Dll.CATIAPostTranslate(filePath);
            if (status == 0)
            {
                MessageBox.Show("Make TransCAD Model First");
            }
            else {
                m_DLL.TransStatus = 1;
            }*/
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
