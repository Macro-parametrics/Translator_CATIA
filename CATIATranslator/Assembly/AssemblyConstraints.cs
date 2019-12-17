using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Runtime.InteropServices; //For Marshal

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
      enum CTYPE { StdAssemblyConstraintType_Coaxial, StdAssemblyConstraintType_Coincidence, StdAssemblyConstraintType_Angle, StdAssemblyConstraintType_Distance,  StdAssemblyConstraintType_Fix, StdAssemblyConstraintType_Parallel, StdAssemblyConstraintType_Perpendicular } // Mutahar 19-02-28 //Chiho 19-11-20
        
        
        //Form2에서 사용할 떄 사용 최종 버전 : Assembly Script를 저장할 filepath와 Part파일들이 저장된 path를 받아와서 Assembly.CATScript파일 생성
        //오버로딩 함수 : Program.cs에서 바로 실행할 떄 쓰임 : Backup ver5_06을 참조
        //오버로딩 함수 :Form1에서 버튼을 눌렀을 떄 API만 사용할 떄 filepath만 줘서 사용한다 : Backup ver5_06을 참조
        public void TranslateConstraintsT2C(string filepath,string path) // From TransCAD to CATIA
        {
            //Get Assem
            tAssem = tAssemDoc.Assem;

            //tConstraints = tAssem.Constraints; //Remove TransCAD 7.0

            TransCAD.StdAssemConstraint tConstraint;

            
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
                
                //Catia init
                try
                {
                    cApp = (INFITF.Application)Marshal.GetActiveObject("CATIA.Application");
                }
                catch
                {
                    cApp = (INFITF.Application)Activator.CreateInstance(Type.GetTypeFromProgID("CATIA.Application"));
                }
                if (cApp == null)
                    return;

                cDocs = cApp.Documents;

                
                
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
                PreStack stack = new PreStack();
                stack.Clear();

                for (int s = 0; s < tAssem.ComponentCount; s++) // Trans CAD 7.0 : SubAssembly = Component 
                {
                    mywriter.WriteLine("Dim products" + (s+1).ToString() + " As Products");
                    mywriter.WriteLine("Set products" + (s+1).ToString() + " = product1.Products");

                    
                    //Step2 : CATPart 두개 Loading
                    for (int w = 1; w < tAssem.Component[s].PartCount; w++)
                    {
                        
                        mywriter.WriteLine("Dim arrayOfVariantOfBSTR" + (cnt+ w).ToString() + "(0)");
                        catPartfilepath = path + "\\" + tAssem.Component[s].Part[w].Name + ".CATPart";
                        mywriter.WriteLine("arrayOfVariantOfBSTR" + (cnt+ w).ToString() + "(0) = \"{0}\"", catPartfilepath);
                        mywriter.WriteLine("products" + (s+1).ToString() + ".AddComponentsFromFiles arrayOfVariantOfBSTR" + (cnt + w).ToString() + ", \"All\"");


                        /* Example Feature Approach TransCAD 7.0
                        TransCAD.Features features = part.Features;
                        int nFeature = features.Count;

                        IEnumerator e = features.GetEnumerator();
                        while (e.MoveNext())
                        {
                            TransCAD.Feature feature = (TransCAD.Feature)e.Current;
                            TransCAD.FeatureType type = feature.Type;
                            string featureName = feature.Name;

                            // Like this, you can get all information in this assembly document
                        }
                        */

                        //Check Debugging
                        Console.WriteLine("Import Parts..........................................");
                        string name = tAssem.Component[s].Name;
                        string name2 = tAssem.Component[s].Part[w].Name;
                        Console.WriteLine("ComponentName : " + name + "     PartName : " + name2); 
                        Console.WriteLine("Import Parts Finish...................................");


                        //Get CATPart Real Name -> Part1 or Part2... or anyNames
                        //CATIA 에서 CATPart 파일이 어떤 인스턴스 네임을 가지고 있는지 검색하는 기능
                        cPartDoc = (MECMOD.PartDocument)cDocs.Read(catPartfilepath);
                        cPart = cPartDoc.Part;
                        string CATIA_inner_name = cPart.get_Name();
                        stack.StackItem(catPartfilepath, CATIA_inner_name, tAssem.Component[s].Part[w].Name);

                    }

                    cnt += tAssem.Component[s].PartCount;
                }



                #region..............................................Constraints 초기화

                //Contraints 개수 및 각Constraints들의 TYPE을 몰라서 하나 하나 해줌
                //다음에는 size랑 각 type은 transcad쪽에서 가져올 수 있게 하면 됨
                //int constraint_size = tConstraints.Count;
                int constraint_size = tAssem.ConstraintCount;
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

                int d = 1;
                int a = 1;


                for (int i = 0; i < constraint_size; i++)
                {
                    
                    tConstraint = tAssem.Constraint[i];                          //TransCAD에서 받아온 tConstraint
                    m_ref = TestRefer(tConstraint, (CTYPE)Enum.Parse(typeof(CTYPE),tConstraint.Type.ToString()), stack);             //reference name translate
                    Sproduct = m_ref.MasterPart_Ref.Substring(m_ref.MasterPart_Ref.IndexOf("\"") + 1, m_ref.MasterPart_Ref.IndexOf("/") - 1);
                    Sconstraint = "constraint" + (i+1).ToString();            //constraint1
                                                                            //Constraint is so exciting truement server caster most of 
                    Smaster_ref = "reference" + (2 * i + 1).ToString();     //reference1
                    Sslave_ref = "reference" + (2 * i + 2).ToString();          //reference2

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

                  
                   
                    if (ang)        // Additional portion of angle constraint
                    {                         
                              Sangle = "angle" + (a++).ToString();

                              mywriter.WriteLine("Dim " + Sangle + " As Dimension");
                              mywriter.WriteLine("Set " + Sangle + " = " + Sconstraint + ".Dimension");
                              mywriter.WriteLine(Sangle + ".Value = " + m_ref.option.ToString());
                            mywriter.WriteLine(Sconstraint + ".AngleSector" + "=" + "catCstAngleSector0"); // "catCstAngleSector0" is hard coded until we have a function to read it
                                a = a + 1;
                    }
                    else if (dist)  // Additional portion of distance constraint
                    {                    
                            Sdistance = "distance" + (d++).ToString();

                            mywriter.WriteLine("Dim " + Sdistance + " As Dimension");
                            mywriter.WriteLine("Set " + Sdistance + " = " + Sconstraint + ".Dimension");
                            mywriter.WriteLine(Sdistance + ".Value = " + m_ref.option.ToString());
                            mywriter.WriteLine(Sconstraint + ".Orientation" + "=" + "catCstOrientSame"); // "catCstOrientSame" is hard coded until we have a function to read it
                            d = d + 1;
                    }                 
                }
                #endregion
                mywriter.WriteLine(Sproduct + ".Update ");
                mywriter.WriteLine("End Sub");
            }

        }


        string _debug = "";

        //각 Constraint에서 사용하는 Master/Slave Part의 ReferenceName을 refCommand 구조체로 리턴 
        private refCommand TestRefer(TransCAD.StdAssemConstraint tConstraint, CTYPE i,PreStack _stack) //constrain 받아오기
        {
            CTYPE type = i;
            refCommand para = new refCommand();
            refCommand result = new refCommand();
            ReferenceClass.ref_Post m_refer = new ReferenceClass.ref_Post();
            m_refer.Set_stack_info(_stack);
            double value = 0;
            //Type Define TransCAD 7.0
            /*
            TransCAD.StdAssemblyConstraintType.StdAssemblyConstraintType_Angle;
            TransCAD.StdAssemblyConstraintType.StdAssemblyConstraintType_Coaxial;
            TransCAD.StdAssemblyConstraintType.StdAssemblyConstraintType_Coincidence;
            TransCAD.StdAssemblyConstraintType.StdAssemblyConstraintType_Distance;
            TransCAD.StdAssemblyConstraintType.StdAssemblyConstraintType_Fix;
            TransCAD.StdAssemblyConstraintType.StdAssemblyConstraintType_Parallel;
            TransCAD.StdAssemblyConstraintType.StdAssemblyConstraintType_Perpendicular;
            */


            switch (type)
            {
                case CTYPE.StdAssemblyConstraintType_Coaxial:
                    
                    TransCAD.StdAssemConstraintCoaxial coax = (TransCAD.StdAssemConstraintCoaxial)tConstraint;
                    Console.WriteLine("Loading Coaxial......................................");

                    _debug = coax.Name;
                    para.command = (int)type;
                    para.param = "Axis" + ":(";
                    para.MasterPart = coax.ConstrainedPart.Name;
                    para.SlavePart = coax.ReferencePart.Name;

                    para.MasterPart_Ref = coax.ConstrainedEntity.ReferenceeName;
                    _debug = coax.ConstrainedEntity.Name;

                    
                    ///[Edit]For test A1 TestRally 20190712
                    {
                        //TransCAD.Reference tempref = coax.ConstrainedEntity;
                        //bool res = tempref.IsPlanarFace;

                        //if we get edge name we have to find surface that define axis
                        if (para.MasterPart_Ref.IndexOf('#') != -1)
                        {
                            string[] _split = para.MasterPart_Ref.Split('#'); 
                            TransCAD.Part ptr = coax.ConstrainedPart;

                            for (int s = 0; s < _split.Length; s++) {
                                string temp = _split[s].Substring(_split[s].IndexOf(',', _split[s].IndexOf(',') + 1) + 1);
                                TransCAD.Reference tempref = ptr.SelectBrepByName(temp);
                                if (!tempref.IsPlanar) {
                                    para.MasterPart_Ref = _split[s];
                                    break;
                                }
                                para.MasterPart_Ref = _split[s];

                            }
                            

                        }
                    }


                    para.SlavePart_Ref = coax.ReferenceEntity.ReferenceeName;
                    _debug = coax.ReferenceEntity.Name;

                    ///[Edit]For test A1 TestRally 20190712
                    {
                        //if we get edge name we have to find surface that define axis
                        if (para.SlavePart_Ref.IndexOf('#') != -1)
                        {
                            string[] _split = para.SlavePart_Ref.Split('#');
                            //para.SlavePart_Ref = _split[1];

                            //Method 2 : Find by name of 'Sketch'
                            for(int j=0;j<_split.Length;j++){
                                if(_split[j].IndexOf("Sketch")>0){
                                      para.SlavePart_Ref = _split[j];      
                                }
                            }    
                            
                        }
                        if (para.SlavePart_Ref.IndexOf('#') != -1)
                        {
                            string[] _split = para.SlavePart_Ref.Split('#');
                            TransCAD.Part ptr = coax.ReferencePart;

                            for (int s = 0; s < _split.Length; s++)
                            {
                                string temp = _split[s].Substring(_split[s].IndexOf(',', _split[s].IndexOf(',') + 1) + 1);
                                TransCAD.Reference tempref = ptr.SelectBrepByName(temp);
                                if (!tempref.IsPlanar)
                                {
                                    para.SlavePart_Ref = _split[s];
                                    break;
                                }
                                para.SlavePart_Ref = _split[s];

                            }


                        }
                    }



                    Console.WriteLine(coax.ConstrainedEntity.ReferenceeName + ".&&."+ coax.ReferenceEntity.ReferenceeName);
                    break;



                case CTYPE.StdAssemblyConstraintType_Coincidence:

                    TransCAD.StdAssemConstraintCoincidence coin = (TransCAD.StdAssemConstraintCoincidence)tConstraint;
                    Console.WriteLine("Loading COINCIDENCE......................................");

                    _debug = coin.Name;
                    para.command = (int)type;
                    para.param = "";
                    para.MasterPart = coin.ConstrainedPart.Name;
                    para.SlavePart = coin.ReferencePart.Name;
                    para.MasterPart_Ref = coin.ConstrainedEntity.ReferenceeName;
                    para.SlavePart_Ref = coin.ReferenceEntity.ReferenceeName;
                    Console.WriteLine(coin.ConstrainedEntity.ReferenceeName + ".&&." + coin.ReferenceEntity.ReferenceeName);

                    break;
                 // Mutahar 18-10-09
                 case CTYPE.StdAssemblyConstraintType_Angle: //(Surface-Surface)
                     TransCAD.StdAssemConstraintAngle ang = (TransCAD.StdAssemConstraintAngle)tConstraint;
                     Console.WriteLine("Loading Angle......................................");

                    _debug = ang.Name;
                     para.command = (int)type;
                     para.param = "";
                     para.MasterPart = ang.ConstrainedPart.Name;
                     para.SlavePart = ang.ReferencePart.Name;
                     para.MasterPart_Ref = ang.ConstrainedEntity.ReferenceeName;
                     para.SlavePart_Ref = ang.ReferenceEntity.ReferenceeName;
                    value = ang.Angle;
                    Console.WriteLine(ang.ConstrainedEntity.ReferenceeName + ".&&." + ang.ReferenceEntity.ReferenceeName);
                    //Angle Value?

                    break;
                 case CTYPE.StdAssemblyConstraintType_Distance: //(Surface-Surface)
                     TransCAD.StdAssemConstraintDistance dist = (TransCAD.StdAssemConstraintDistance)tConstraint;
                     Console.WriteLine("Loading Distance......................................");

                    _debug = dist.Name;
                     para.command = (int)type;
                     para.param = "";
                     para.MasterPart = dist.ConstrainedPart.Name;
                     para.SlavePart = dist.ReferencePart.Name;
                     para.MasterPart_Ref = dist.ConstrainedEntity.ReferenceeName;
                     para.SlavePart_Ref = dist.ReferenceEntity.ReferenceeName;
                    Console.WriteLine(dist.ConstrainedEntity.ReferenceeName + ".&&." + dist.ReferenceEntity.ReferenceeName);
                    value = dist.Distance;
                    //Offset Value?

                    break;
                 

                default:
                    Console.WriteLine("Anything........");
                    break;
  

            }
            Console.WriteLine("////////////////////////////////////////////////////////////////////////");
            result = m_refer.ConvertRefPost(para);
            result.option = value;
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
