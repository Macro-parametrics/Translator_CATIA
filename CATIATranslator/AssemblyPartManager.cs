using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using TransCAD;

using System.IO;

/*
 * Form1에서 Assembly Post를 위하여 Part관리 및 Translate를 구현
 * (현재는 하나의 Component를 통해서만 Part들을 불러옴)
 * 1. Assembly Class를 생성하여 Assembly Post관련 변수 및 함수 사용
 *      - TransCAD 초기화
 *      - CATIA 초기화
 *      - Part파일들을 API를 통한 CATPart파일들 반환
 *      - Part파일들을 Reference Class를 통한 CATScript 파일들 반환
 *      - Assembly를 .CATSript로 변환 
 *      
 * 2. List를 통하여 Part들을 관리
 *      - List Subitem 0 : TransCAD에 포함된 Part
 *      - List Subitem 1 : 지정된 디렉토리에 같은이름의 .CATScript들의 존재여부 판단
 *      - List Subitem 2 : 지정된 디렉토리에 같은이름의 .CATPart들의 존재여부 판단
 *   
     */


namespace CATIATranslator
{
    public partial class AssemblyPartManager : Form
    {
        //현재는 하나의 Component단위에 대해서만 Assembly Part들을 다룬다. : 추후에 업데이트
        string path = "";      //Assembly .CATSript 경로에서 추출한 각 part를 저장할 파일 경로
        string fullpath = "";  //Assembly .CATSript 경로
        Assembly AssemblyManager = new Assembly();//전체적인 TransCAD변수 및 CATIA API 변수들을 가지고 있고 그에따른 함수를 제공
        TransCAD.Assem _tAssem = null;

        //생성자 함수
        //Assem에 어떤 Part들이 있는지 Listup
        public AssemblyPartManager(string filePath)
        {
            InitializeComponent();
            AssemblyManager.InitializeTransCAD((int)1);    //TransCAD초기화 부분(mod = 1 :Post, mod = 0 : Pre)


            fullpath = filePath;
            path = filePath.Substring(0,filePath.LastIndexOf('\\'));
            this.label1.Text = path;

            if (AssemblyManager.tAssemDoc == null) { this.label1.Text = "NULL"; return; }
            _tAssem = AssemblyManager.tAssemDoc.Assem;

            TransCAD.Component comp = _tAssem.GetComponent(1);
                //Part들을 넣을 path 및 폴더 생성
                path = path + "\\" +comp.get_Name();
                if (!System.IO.Directory.Exists(path)){System.IO.Directory.CreateDirectory(path);}

            TransCAD.Part parts;

            
            ListViewItem lvi;
            ListViewItem.ListViewSubItem lvsi;

            for (int i = 0; i < comp.GetSize(); i++) {

                parts = comp.GetPart(i);

                lvi = new ListViewItem();
                lvi.Text = parts.Name;

                lvsi = new ListViewItem.ListViewSubItem();
                lvsi.Text = "None";
                lvi.SubItems.Add(lvsi);

                lvsi = new ListViewItem.ListViewSubItem();
                lvsi.Text = "None";
                lvi.SubItems.Add(lvsi);

                this.listView1.Items.Add(lvi);

            }
            this.listView1.EndUpdate();
            Updatelist();
        }

        //현재 지정된 파일경로에 각 List에 해당하는 파일이 있는지 업데이트해주는 함수
        public void Updatelist() {

            try
            {
                ListViewItem lvi;
                
                System.IO.DirectoryInfo dir = new System.IO.DirectoryInfo(path+"\\");
                
                DirectoryInfo[] dirs = dir.GetDirectories();
                FileInfo[] files = dir.GetFiles();

                

                //this.listView1.Items.Clear();

                //this.listView1.BeginUpdate();
                int cnt = 0;
                
                while (cnt< this.listView1.Items.Count) {
                    lvi = new ListViewItem();
                    lvi = this.listView1.Items[cnt];
                    string text = lvi.Text;
                    
                    if (text != "")
                    {
                    
                        foreach (System.IO.FileInfo fi in files)
                        {

                            if (text == fi.Name.Substring(0, fi.Name.LastIndexOf('.')))
                            {
                                string type = fi.Name.Substring(fi.Name.LastIndexOf('.'), fi.Name.Length- fi.Name.LastIndexOf('.'));
                                
                                if (type == ".CATScript")
                                {
                                    this.listView1.Items[cnt].SubItems[1].Text = text;
                                    this.listView1.Items[cnt].Checked = true;
                                   
                                }
                                else if (type == ".CATPart")
                                {

                                    this.listView1.Items[cnt].SubItems[2].Text = text;
                                    this.listView1.Items[cnt].Checked = true;
                                }

                            }


                        }

                    }
                    else { break; }
                    cnt++;
                }
   
    
            }
            catch (System.Exception err)
            {
                MessageBox.Show("Error: " + err.Message);
            }

            this.listView1.EndUpdate();


        }


        //Click Update 
        //Path에 Assembly에서 읽은 파트파일들이 있는지 List를 업데이트
        private void button1_Click(object sender, EventArgs e)
        {
            Updatelist();
        }
        

        //Click Translate Parts by Part
        //List에서 선택되지 않은 파트 파일을 API를 이용하여 .CATPart로 변환
        private void button2_Click(object sender, EventArgs e)
        {
            


            AssemblyManager.InitializeCATIA(fullpath, (int)1);//API를 사용하기 위해 CATIA변수 초기화


            for (int i = 0; i < this.listView1.Items.Count; i++) {

                if (this.listView1.Items[i].Checked == false) {
                    //Part 변환 하기~
                    string curr_filepath = path + "\\" + this.listView1.Items[i].Text + ".CATPart";
                    TransCAD.Component comp = _tAssem.GetComponent(1);
                    TransCAD.Part parts = comp.GetPart(i);
                    
                    AssemblyManager.TranslatePartsT2C(curr_filepath,i, parts);
                }

            }
            Updatelist();
            MessageBox.Show("Finish");

        }

        //Click Translate Parts by Script
        //List에서 선택되지 않은 파트 파일을 Dll을 이용하여 .CATScript로 변한
        private void button4_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < this.listView1.Items.Count; i++)
            {

                if (this.listView1.Items[i].Checked == false)
                {
                    //Part 변환 하기~
                    string curr_filepath = path + "\\" + this.listView1.Items[i].Text + ".CATScript";
                    AssemblyManager.TranslatePartsT2C_Script(1, i, curr_filepath);//CompNum,PartNum,filePath : 몇번째 Comp의 몇번째 Part를 filePath에 .CATScript로 변환해서 넣겠다.

                }

            }
            Updatelist();
            MessageBox.Show("Finish");
        }

        //Click Start
        //모든 파트파일들이 변환되어 .CATPart로 존재하면 Assembly를 CATScript로 변환
        private void button3_Click(object sender, EventArgs e)
        {
            //Assembly에 존재하는 모든 Part 파일들이 CATPart로 존재하는지 파악
            for (int i = 0; i < this.listView1.Items.Count; i++)
            {

                if (this.listView1.Items[i].Text != this.listView1.Items[i].SubItems[2].Text)
                {
                    MessageBox.Show("Please Translate All Parts");
                    return;
                }
                

            }

            MessageBox.Show("Start Translate");
            //Assembly 변환하기~
            AssemblyManager.TranslateConstraintsT2C(fullpath, path);  //Assembly를 .CATScript로 만들기 (fullpath : Assembly 생성할 파일 경로, path : Part 참고할 파일 경로)
                                                                      //현재는 fullpath와 path가 같은 경로를 가리킨다(Assembly Script가 있는 곳에 CATPart들도 있어야한다는 말)
            MessageBox.Show("Finish");
            this.Close();
        }

       
    }

   

}


