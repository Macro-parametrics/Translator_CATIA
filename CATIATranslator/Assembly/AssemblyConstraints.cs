using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace CATIATranslator
{
    partial class Assembly
    {

        public void TranslateConstraintsT2C() // From TransCAD to CATIA
        {
            tAssem = tAssemDoc.Assem;
            tConstraints = tAssem.Constraints;
            TransCAD.IStdAssemConstraint tConstraint = tConstraints[1]; //Constraint는 numbering 1이 시작 (0 아님)
            TransCAD.IStdAssemConstraintCoaxial coax = (TransCAD.IStdAssemConstraintCoaxial)tConstraint;

            Console.WriteLine("Coaxial constraint information");
            Console.WriteLine("Constraint name is " + coax.Name);
            Console.WriteLine("Constrained Part is " + coax.ConstrainedPart.Name);
            Console.WriteLine("Constrained geometry is " + coax.ConstrainedGeometry.ReferenceeName);
            Console.WriteLine("Reference part is " + coax.ReferencePart.Name);
            Console.WriteLine("Reference geometry is " + coax.ReferenceGeometry.ReferenceeName);

            // Incidence constraint
            tConstraint = tConstraints[2]; //Constraint는 numbering 1이 시작 (0 아님)
            TransCAD.IStdAssemConstraintIncidence coin = (TransCAD.IStdAssemConstraintIncidence)tConstraint;

            Console.WriteLine("Incidence constraint information");
            Console.WriteLine("Constraint name is " + coin.Name);
            Console.WriteLine("Constrained Part is " + coin.ConstrainedPart.Name);
            Console.WriteLine("Constrained geometry is " + coin.ConstrainedGeometry.ReferenceeName);
            Console.WriteLine("Reference part is " + coin.ReferencePart.Name);
            Console.WriteLine("Reference geometry is " + coin.ReferenceGeometry.ReferenceeName);


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
            using (StreamWriter mywriter = new StreamWriter(@"TESTModel\Product_Assem_00.CATScript"))
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

                mywriter.WriteLine("Dim products1 As Products");
                mywriter.WriteLine("Set products1 = product1.Products");

                //Step2 : CATPart 두개 Loading
                mywriter.WriteLine("Dim arrayOfVariantOfBSTR1(0)");
                //partpath = "\"C:\\Users\\Imgyu Kim\\Documents\\Visual Studio 2015\\Repos\\transcatia\\TransCAT(C#)\\CATIATranslator\\bin\\Debug\\TESTModel\\A1_01.CATPart\"";
                catPartfilepath = saveaspartname[0];
                mywriter.WriteLine("arrayOfVariantOfBSTR1(0) = \"{0}\"", catPartfilepath);
                mywriter.WriteLine("products1.AddComponentsFromFiles arrayOfVariantOfBSTR1, \"All\"");

                mywriter.WriteLine("Dim arrayOfVariantOfBSTR2(0)");
                //partpath = "\"C:\\Users\\Imgyu Kim\\Documents\\Visual Studio 2015\\Repos\\transcatia\\TransCAT(C#)\\CATIATranslator\\bin\\Debug\\TESTModel\\A1_02.CATPart\"";
                catPartfilepath = saveaspartname[1];
                mywriter.WriteLine("arrayOfVariantOfBSTR2(0) = \"{0}\"", catPartfilepath);
                mywriter.WriteLine("products1.AddComponentsFromFiles arrayOfVariantOfBSTR2, \"All\"");

                //Step3 : Constraint[1]에 대한 Reference1, 2
                mywriter.WriteLine("Dim constraints1 As Collection");
                mywriter.WriteLine("Set constraints1 = product1.Connections(\"CATIAConstraints\")");

                mywriter.WriteLine("Dim reference1 As Reference");
                referenceNameFromPart = "\"Product1/Part1.1/!Selection_RSur:(Face:(Brp:(Pad.1;1);None:();Cf11:());Pocket.1_ResultOUT;Z0;G4162)\"";
                mywriter.WriteLine("Set reference1 = product1.CreateReferenceFromName({0})", referenceNameFromPart);

                mywriter.WriteLine("Dim reference2 As Reference");
                referenceNameFromPart = "\"Product1/Part1.2/!Selection_RSur:(Face:(Brp:(Pad.1;2);None:();Cf11:());Pad.2_ResultOUT;Z0;G4162)\"";
                mywriter.WriteLine("Set reference2 = product1.CreateReferenceFromName({0})", referenceNameFromPart);

                //Step4 : Constraint[1] 실행
                mywriter.WriteLine("Dim constraint1 As Constraint");
                mywriter.WriteLine("Set constraint1 = constraints1.AddBiEltCst(catCstTypeSurfContact, reference1, reference2)");

                mywriter.WriteLine("product1.Update ");

                mywriter.WriteLine("Set constraints1 = product1.Connections(\"CATIAConstraints\")");    //?

                //Step5 : Constraint[2] 에 대한 Reference3,4
                mywriter.WriteLine("Dim reference3 As Reference");
                referenceNameFromPart = "\"Product1/Part1.1/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;1)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G4162))\"";
                mywriter.WriteLine("Set reference3 = product1.CreateReferenceFromName({0})", referenceNameFromPart);

                mywriter.WriteLine("Dim reference4 As Reference");
                referenceNameFromPart = "\"Product1/Part1.2/!Axis:(Selection_RSur:(Face:(Brp:(Pad.2;0:(Brp:(Sketch.2;1)));None:();Cf11:());Pad.2_ResultOUT;Z0;G4162))\"";
                mywriter.WriteLine("Set reference4 = product1.CreateReferenceFromName({0})", referenceNameFromPart);

                //Step6 : Constraint[2] 실행
                mywriter.WriteLine("Dim constraint2 As Constraint");
                mywriter.WriteLine("Set constraint2 = constraints1.AddBiEltCst(catCstTypeOn, reference3, reference4)");

                mywriter.WriteLine("product1.Update ");

                mywriter.WriteLine("End Sub");
            }

        }
    }
}
