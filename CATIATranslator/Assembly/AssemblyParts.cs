using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices; //For Marshal
using System.IO;
using System.Threading.Tasks;

/*
 * Assembly Class에서 관리
 * 1. 어셈블리를 하기 위해서 필요한 TransCAD Part 파일들을 CATIA part 파일들로 변한
 *      - API를 사용하는 방법과 Referenece Class에서 정의된 Script를 통한 변환 방법을 정의
 * 
 * 
 *   
     */




namespace CATIATranslator
{
    partial class Assembly
    {
        
        string[] saveaspartname;

        public void TranslateC2T() // From CATIA to TransCAD
        {
            /*
                Assebmly Pre 함수 구현    
            */
        }

        //CATPart 파일들을 저장할 filepath와 part를 인자로 줘서 그곳에 변환한 파일을 저장하게 함 //현재는 그냥 Part를 줬지만 itemNum을 이용하여 Assem변수에서 Part를 가져와서 할수도 있음.
        //오버로딩 함수 : CATPart파일들을 저장할 filepath들만 인자로 줘서 그곳에 변환하여 저장하게 하는 함수 : Backup ver5_06을 참조
        //오버로딩 함수 : CATPart파일들을 저장할 filepath를 임의로 다 저장해놓은 상태에서 변환을 하는 함수 : Backup ver5_06을 참조
        public void TranslatePartsT2C(string filepath,int itemNum,TransCAD.Part part)
        {
            //일단 itemNum은 사용하지 않음
            //savespartname은 예전에 사용했던 변수임

            saveaspartname = new string[1]; //실제로 사용되는지는 모르겠다.
            Task[] TasksPartTranslation = new Task[1];
            
            TransCAD.IPart currentpart = part;

            Console.WriteLine("Name of part [ " + currentpart.Name + " ]");
            Part PartManager = new Part();
            PartManager.InitializeTransCAD((int)2);
            if (ReferenceManager == null)
                    ReferenceManager = new Reference(PartManager);
            TasksPartTranslation[0] = Task.Factory.StartNew(() =>
            {
                    PartManager.InitializeCATIA(" ", (int)1);
                    PartManager.TranslateT2C(currentpart); //1개에 대한 Part post 번역 실행
                    Console.WriteLine("Name of part [ " + currentpart.Name + " ] was Translated!");
                    saveaspartname[0] = filepath;
                    PartManager.SaveCATPartFile(saveaspartname[0]);
                    Console.WriteLine("Name of part [ " + currentpart.Name + " ] was saved as " + saveaspartname[0]);
                    PartManager.UninitializeCATIA();
                });
                TasksPartTranslation[0].Wait();
                Part.partNum++;
         }


        //각 part 들을 .CATScript로 변한하여 path에 저장함
        public void TranslatePartsT2C_Script(int CompNum, int PartNum, string path)
        {
            ReferenceClass.ref_Post m_refer = new ReferenceClass.ref_Post();
            m_refer.ExtentionFuc(CompNum, PartNum, path);//CompNum,PartNum,filePath : 몇번째 Comp의 몇번째 Part를 filePath에 .CATScript로 변환해서 넣겠다.


        }

    }
}

