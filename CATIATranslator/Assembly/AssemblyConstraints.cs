using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

/*
 * Assembly Class에서 관리
 * 1. 최종적을 어셈블리를 Script로 변환하기 위한 함수 및 변수들을 정의
 * 
 * 
 * 
 *   
     */


namespace CATIATranslator
{


    partial class Assembly
    {

   //     enum CTYPE { Coaxial,Incidence }
      enum CTYPE { Coaxial, Incidence, Distance, Angle } // Mutahar 19-02-28
        
        
        //Form2에서 사용할 떄 사용 최종 버전 : Assembly Script를 저장할 filepath와 Part파일들이 저장된 path를 받아와서 Assembly.CATScript파일 생성
        //오버로딩 함수 : Program.cs에서 바로 실행할 떄 쓰임 : Backup ver5_06을 참조
        //오버로딩 함수 :Form1에서 버튼을 눌렀을 떄 API만 사용할 떄 filepath만 줘서 사용한다 : Backup ver5_06을 참조
        public void TranslateConstraintsT2C(string filepath,string path) // From TransCAD to CATIA
        {
            tAssem = tAssemDoc.Assem;
            tConstraints = tAssem.Constraints;
            TransCAD.IStdAssemConstraint tConstraint;

            
            /*
             * 번역 추가 해야할 사항 17-09-15
             * Slave part에서 Constrained reference name
             * Master part 에서 Reference name
             * Part 번호 Get
             * Product 번호 Get
             * 기존 Reference name에 Part번호 / Product 번호 붙이기
             */
            string catPartfilepath;
            string[] catPartNum;
            string catProdNum;
            string referenceNameFromPart;
            using (StreamWriter mywriter = new StreamWriter(filepath))
            {
                //mywriter.Write(hexHash + " ");
                //mywriter.WriteLine(Path.GetFileName(dataFile.ToString()));

                //Step1 : Product Document 생성
                mywriter.WriteLine("Language=\"VBSCRIPT\"");
                mywriter.WriteLine("Sub CATMain()");

                mywriter.WriteLine("Dim documents1 As Documents");
                mywriter.WriteLine("Set documents1 = CATIA.Documents");

                mywriter.WriteLine("Dim productDocument1 As Document");
                mywriter.WriteLine("Set productDocument1 = documents1.Add(\"Product\")");

                mywriter.WriteLine("Dim product1 As Product");
                mywriter.WriteLine("Set product1 = productDocument1.Product");


                ///////////////파트 추가하는 부분///////////////////
                int cnt = 0;
                
                for (int s = 1; s < tAssem.GetSize(); s++)
                {
                    mywriter.WriteLine("Dim products" + (s).ToString() + " As Products");
                    mywriter.WriteLine("Set products" + (s).ToString() + " = product1.Products");

                    
                    //Step2 : CATPart 두개 Loading
                    for (int w = 0; w < tAssem.GetComponent(s).GetSize(); w++)
                    {
                        
                        mywriter.WriteLine("Dim arrayOfVariantOfBSTR" + (cnt+ w + 1).ToString() + "(0)");
                        //partpath = "\"C:\\Users\\Imgyu Kim\\Documents\\Visual Studio 2015\\Repos\\transcatia\\TransCAT(C#)\\CATIATranslator\\bin\\Debug\\TESTModel\\A1_01.CATPart\"";
                        catPartfilepath = path + "\\" + tAssem.GetComponent(s).GetPart(w).Name + ".CATPart";
                        mywriter.WriteLine("arrayOfVariantOfBSTR" + (cnt+ w + 1).ToString() + "(0) = \"{0}\"", catPartfilepath);
                        mywriter.WriteLine("products" + (s).ToString() + ".AddComponentsFromFiles arrayOfVariantOfBSTR" + (cnt + w + 1).ToString() + ", \"All\"");

                        //PartName 
                        //Body.Part1.1
                        //RotationPart.Part1.2
                        
                    }

                    cnt += tAssem.GetComponent(s).GetSize();
                }

                

                #region..............................................Constraints 초기화

                //Contraints 개수 및 각Constraints들의 TYPE을 몰라서 하나 하나 해줌
                //다음에는 size랑 각 type은 transcad쪽에서 가져올 수 있게 하면 됨
                int constraint_size = tConstraints.Count;
                //CTYPE[] ttype = new CTYPE[2] { CTYPE.COAXIAL, CTYPE.INCIDENCE };
                 

                string Sproduct = "";
                string Sconstraints = "constraints1";                       //기본적인부분
                string Sconstraint = "";
                string Smaster_ref = "";
                string Sslave_ref = "";
                refCommand m_ref;

          //    string Sangle = "";
          //    string Sdistance = "";
                
                
                mywriter.WriteLine("Dim " + Sconstraints + " As Collection");
                #endregion

                #region .............................................각 Constraint 실행
                for (int i = 1; i < constraint_size + 1; i++)
                {
                    
                    tConstraint = tConstraints.Constraint[i];                          //TransCAD에서 받아온 tConstraint
                    m_ref = TestRefer(tConstraint, (CTYPE)Enum.Parse(typeof(CTYPE),tConstraint.Type.ToString()));             //reference name translate
                    Sproduct = m_ref.MasterPart_Ref.Substring(m_ref.MasterPart_Ref.IndexOf("\"") + 1, m_ref.MasterPart_Ref.IndexOf("/") - 1);
                    Sconstraint = "constraint" + (i).ToString();            //constraint1
                    Smaster_ref = "reference" + (2 * i - 1).ToString();     //reference1
                    Sslave_ref = "reference" + (2 * i).ToString();          //reference2

                    //constraints 설정
                    mywriter.WriteLine("Set " + Sconstraints + " = " + Sproduct + ".Connections(\"CATIAConstraints\")");
                    //master refer 생성
                    mywriter.WriteLine("Dim " + Smaster_ref + " As Reference");
                    mywriter.WriteLine("Set " + Smaster_ref + " = " + Sproduct + ".CreateReferenceFromName(" + m_ref.MasterPart_Ref + ")");
                    //slaver refer 생성
                    mywriter.WriteLine("Dim " + Sslave_ref + " As Reference");
                    mywriter.WriteLine("Set " + Sslave_ref + " = " + Sproduct + ".CreateReferenceFromName(" + m_ref.SlavePart_Ref + ")");
                    //Constraint[1] 실행
                    mywriter.WriteLine("Dim " + Sconstraint + " As Constraint");
                    mywriter.WriteLine("Set " + Sconstraint + " = " + Sconstraints + ".AddBiEltCst(" + m_ref.param + ", " + Smaster_ref + ", " + Sslave_ref + ")");

                    
                  // to be used in the case of angle and distance constriants
                    string const_inst = m_ref.param;
                    string const_ang = "catCstTypeAngle";
                    string const_dist = "catCstTypeDistance";
                    string Sangle = "";
                    string Sdistance = "";
                    bool ang = const_inst.Equals(const_ang);
                    bool dist = const_inst.Equals(const_dist);
                    int d = 1;

                    int a = 1;
                  
                   
                    if (ang)        // Additional portion of angle constraint
                    {                         
                              Sangle = "angle" + (a).ToString();

                              mywriter.WriteLine("Dim" + Sangle + "As Dimension");
                              mywriter.WriteLine("Set" + Sangle + "=" + Sconstraint + ".Dimension");
                              mywriter.WriteLine(Sangle + ".Value" + 0.000000);
                            mywriter.WriteLine(Sconstraint + ".AngleSector" + "=" + "catCstAngleSector0"); // "catCstAngleSector0" is hard coded until we have a function to read it
                                a = a + 1;
                    }
                    else if (dist)  // Additional portion of distance constraint
                    {                    
                            Sdistance = "distance" + (d).ToString();

                            mywriter.WriteLine("Dim" + Sdistance + "As Dimension");
                            mywriter.WriteLine("Set" + Sdistance + "=" + Sconstraint + ".Dimension");
                            mywriter.WriteLine(Sdistance + ".Value" + 10.000000);
                            mywriter.WriteLine(Sconstraint + ".Orientation" + "=" + "catCstOrientSame"); // "catCstOrientSame" is hard coded until we have a function to read it
                            d = d + 1;
                    }                 
                }
                #endregion
                mywriter.WriteLine(Sproduct + ".Update ");
                mywriter.WriteLine("End Sub");
            }

        }

        


        //각 Constraint에서 사용하는 Master/Slave Part의 ReferenceName을 refCommand 구조체로 리턴 
        private refCommand TestRefer(TransCAD.IStdAssemConstraint tConstraint, CTYPE i) //constrain 받아오기
        {
            CTYPE type = i;
            refCommand para = new refCommand();
            refCommand result = new refCommand();
            ReferenceClass.ref_Post m_refer = new ReferenceClass.ref_Post();


            switch (type)
            {
                case CTYPE.Coaxial:
                    TransCAD.IStdAssemConstraintCoaxial coax = (TransCAD.IStdAssemConstraintCoaxial)tConstraint;
                    Console.WriteLine("Loading Coaxial......................................");

                    para.command = (int)type;
                    para.param = "Axis" + ":(";
                    para.MasterPart = coax.ConstrainedPart.Name;
                    para.SlavePart = coax.ReferencePart.Name;
                    para.MasterPart_Ref = coax.ConstrainedGeometry.ReferenceeName;
                    para.SlavePart_Ref = coax.ReferenceGeometry.ReferenceeName;

                    break;
                case CTYPE.Incidence:

                    TransCAD.IStdAssemConstraintIncidence coin = (TransCAD.IStdAssemConstraintIncidence)tConstraint;
                    Console.WriteLine("Loading INCIDENCE......................................");

                    para.command = (int)type;
                    para.param = "";
                    para.MasterPart = coin.ConstrainedPart.Name;
                    para.SlavePart = coin.ReferencePart.Name;
                    para.MasterPart_Ref = coin.ConstrainedGeometry.ReferenceeName;
                    para.SlavePart_Ref = coin.ReferenceGeometry.ReferenceeName;
       

                    break;
                    /* Mutahar 18-10-09
                 case CTYPE.Angle: //(Surface-Surface)
                     TransCAD.IStdAssemConstraintAngle ang = (TransCAD.IStdAssemConstraintAngle)tConstraint;
                     Console.WriteLine("Loading Angle......................................");

                     para.command = (int)type;
                     para.param = "";
                     para.MasterPart = ang.ConstrainedPart.Name;
                     para.SlavePart = ang.ReferencePart.Name;
                     para.MasterPart_Ref = ang.ConstrainedGeometry.ReferenceeName;
                     para.SlavePart_Ref = ang.ReferenceGeometry.ReferenceeName;
                     Angle Value?

                   break;
                 case CTYPE.Distance: //(Surface-Surface)
                     TransCAD.IStdAssemConstraintCoaxial Dist = (TransCAD.IStdAssemConstraintDistance)tConstraint;
                     Console.WriteLine("Loading Distance......................................");

                     para.command = (int)type;
                     para.param = "";
                     para.MasterPart = dist.ConstrainedPart.Name;
                     para.SlavePart = dist.ReferencePart.Name;
                     para.MasterPart_Ref = dist.ConstrainedGeometry.ReferenceeName;
                     para.SlavePart_Ref = dist.ReferenceGeometry.ReferenceeName;
                     Offset Value?

                   break;
                 */

                default:
                    Console.WriteLine("Anything........");
                    break;
  

            }
            Console.WriteLine("////////////////////////////////////////////////////////////////////////");
            result = m_refer.ConvertRefPost(para);

            Console.WriteLine("////////////////////////////////////////////////////////////////////////");
            Console.WriteLine("ConsNum : " + result.command.ToString());
            Console.WriteLine("master : " + result.MasterPart_Ref);
            Console.WriteLine("slave : " + result.SlavePart_Ref);
            Console.WriteLine("////////////////////////////////////////////////////////////////////////");

            return result;
        }


        //Test Comments

    }
}
