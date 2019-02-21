using System;
using System.Collections;
using System.Runtime.InteropServices; //For Marshal
using System.Windows.Forms;
using System.IO;

/*
 * Assembly Class에서 관리
 * 1. Assembly에 해당하는 각종 변수 및 Initialize 관리
 *      - TransCAD, CATIA API를 정의
 * 
 * 
 * 
 *   
     */


namespace CATIATranslator
{
    partial class Assembly
    {
        #region CATIA 변수
        public INFITF.Application cApp = null;
        public INFITF.Documents cDocs = null;
        public ProductStructureTypeLib.Product cProduct = null;
        public ProductStructureTypeLib.Products cProducts = null;
        public ProductStructureTypeLib.ProductDocument cProductDoc = null;
        public MECMOD.PartDocument cPartDoc = null;
        public MECMOD.Part cPart = null;
        public MECMOD.OriginElements cOriginalElements = null;
        public MECMOD.Bodies cBodies = null;
        public MECMOD.Body cCurrentBody = null;
        public MECMOD.Shapes cShapes = null;
        public MECMOD.Factory cFactory = null;
        public MECMOD.Sketches cSketches = null;

        public MECMOD.Constraints cConstraints = null; //hyebin
        public MECMOD.Constraint cConstraint = null;

        public PARTITF.ShapeFactory cShapeFactory = null;
        public HybridShapeTypeLib.HybridShapeFactory cHybridShapeFactory = null;
        #endregion

        #region TransCAD 변수
        public TransCAD.IApplication tApp = null;
        public TransCAD.Documents tDocs = null;
        public TransCAD.IAssemDocument tAssemDoc = null;
        public TransCAD.Assem tAssem = null;
        public TransCAD.IComp tComp = null;
        public TransCAD.PartDocument tPartDoc = null;
        public TransCAD.Part tPart = null;
        public TransCAD.Features tFeatures = null;
        public TransCAD.IStdAssemConstraints tConstraints = null;
        #endregion

        public string Save_Part_Path = "";
        // Reference 관리자
        public Reference ReferenceManager = null;
        //// CATIA 관련 함수
        public bool InitializeCATIA(string filePath, int mode)
        {
            /*
                어셈블리 특화 함수 구현    
            */

            try
            {
                cApp = (INFITF.Application)Marshal.GetActiveObject("CATIA.Application");
            }
            catch
            {
                cApp = (INFITF.Application)Activator.CreateInstance(Type.GetTypeFromProgID("CATIA.Application"));
            }

            if (cApp == null)
                return false;

            cDocs = cApp.Documents;

            if (mode == 0)
                cProductDoc = (ProductStructureTypeLib.ProductDocument)cDocs.Read(filePath); //CATProduct read
                //cProductDoc = null;
            else if (mode == 1)
                cProductDoc = (ProductStructureTypeLib.ProductDocument)cDocs.Add("Product"); //new CATProduct 생성

            cApp.Visible = true;

            cProduct = cProductDoc.Product;
            cProducts = cProduct.Products;

            cConstraints = (MECMOD.Constraints)cProduct.Connections("CATIAConstraints");

            return true;
        }

        public void UninitializeCATIA()
        {

            /*
                어셈블리 특화 함수 구현    
            */
            cProduct = null;
            cProducts = null;
            cProductDoc = null;

        }

        //// TransCAD 관련 함수
        public bool InitializeTransCAD(int mode)
        {
            try
            {
                tApp = (TransCAD.IApplication)Marshal.GetActiveObject("TransCAD.IApplication");
            }
            catch
            {
                tApp = (TransCAD.IApplication)Activator.CreateInstance(Type.GetTypeFromProgID("TransCAD.Application"));
            }

            if (tApp == null)
                return false;

            tDocs = tApp.Documents;


            if (mode == 0) //assembly Pre 일때 사용
                tAssemDoc = tDocs.AddAssemDocument();
            else if (mode == 1) //assembly Post 일때 사용
                tAssemDoc = (TransCAD.IAssemDocument)tApp.ActiveDocument;
            //_spAssemDoc = g_spApplication->ActiveDocument; C++

            /* 어셈블리 구조 추가 코드 Preprossor 일때 보통 적용할 수 잇을뜻.
            tAssemDoc = tDocs.AddAssemDocument();
            TransCAD.Assem tAssem = tAssemDoc.Assem;
            TransCAD.Component tComp = tAssem.CreateComponent();
            tComp.set_Name("Comp1");
            
            tPart = tAssem.CreatePart();
            tPart.set_Name("Part1");

            tComp.AddPart(tPart);
            tAssem.AddComponent(tComp);
            */


            //if (ReferenceManager == null)
            //    ReferenceManager = new Reference(this);

            //tAssemDoc.Update();

            return true;
        }

        public void UninitializeTransCAD()
        {
            tApp = null;
            tDocs = null;
            tPartDoc = null;
            tPart = null;
            tAssemDoc = null;
            tFeatures = null;
        }

    }
}
