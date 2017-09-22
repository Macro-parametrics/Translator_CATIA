using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices; //For Marshal
using System.IO;
using System.Threading.Tasks;

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

        public void TranslatePartsT2C()
        {
            tAssem = tAssemDoc.Assem;
            tConstraints = tAssem.Constraints;
            

            int compCnt = tAssem.GetSize();

            for (int i = 0; i < compCnt; i++)
            {
                //TransCAD::ICompPtr comp = _spAssem->GetComponent(i);
                tComp = tAssem.GetComponent(i);
                //cout << "Name of Component[ " << i << " ] : " << comp->get_Name() << endl;  //component의 이름 get.
                Console.WriteLine("Name of Component [ " + tComp.get_Name() + " ]");
                Console.WriteLine("The number of Parts [ " + tComp.GetSize() + " ]");
            }
            tComp = tAssem.GetComponent(1);
            int partCnt = tComp.GetSize();
            saveaspartname = new string[partCnt];
            Task[] TasksPartTranslation = new Task[partCnt];

            for (int i = 0; i < partCnt; i++)
            {
                TransCAD.IPart currentpart = tComp.GetPart(i);
                Console.WriteLine("Name of part [ " + currentpart.Name + " ]");
                Part PartManager = new Part();
                PartManager.InitializeTransCAD((int)2);
                if (ReferenceManager == null)
                    ReferenceManager = new Reference(PartManager);
                TasksPartTranslation[i] = Task.Factory.StartNew(() =>
                 {
                     PartManager.InitializeCATIA(" ", (int)1);
                     PartManager.TranslateT2C(currentpart); //1개에 대한 Part post 번역 실행
                     Console.WriteLine("Name of part [ " + currentpart.Name + " ] was Translated!");
                     saveaspartname[i] = System.Environment.CurrentDirectory + @"\TESTmodel\Product_Part0" + i + ".CATPart";
                     PartManager.SaveCATPartFile(saveaspartname[i]);
                     Console.WriteLine("Name of part [ " + currentpart.Name + " ] was saved as "+ saveaspartname[i]);
                     PartManager.UninitializeCATIA();
                 });
                TasksPartTranslation[i].Wait();
                Part.partNum++;
            }
        }

    }
}
