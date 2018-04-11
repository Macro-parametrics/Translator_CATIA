using System;
using System.Collections;
using System.Runtime.InteropServices; //For Marshal
using System.Windows.Forms;
using System.IO;


namespace CATIATranslator
{
    partial class Part
    {
        #region CATIA 변수모음
        public INFITF.Application cApp = null;
        public INFITF.Documents cDocs = null;
        
        public MECMOD.PartDocument cPartDoc = null;
        public MECMOD.Part cPart = null;
        public MECMOD.OriginElements cOriginalElements = null;
        public MECMOD.Bodies cBodies = null;
        public MECMOD.Body cCurrentBody = null;
        public MECMOD.Shapes cShapes = null;
        public MECMOD.Factory cFactory = null;
        public MECMOD.Sketches cSketches = null;

        public PARTITF.ShapeFactory cShapeFactory = null;
        public HybridShapeTypeLib.HybridShapeFactory cHybridShapeFactory = null;
        #endregion

        #region TransCAD 변수
        public TransCAD.Application tApp = null;
        public TransCAD.Documents tDocs = null;
        public TransCAD.IAssemDocument tAssemDoc = null;
        public TransCAD.Assem tAssem = null;
        public TransCAD.IComp tComp = null;
        public TransCAD.PartDocument tPartDoc = null;
        public TransCAD.Part tPart = null;
        public TransCAD.Features tFeatures = null;
        #endregion

        // Reference 관리자
        public Reference ReferenceManager = null;

        //Assembly Document 내의 Part 번역을 위한 보조 변수
        public static bool isSubAssemblyPart = false;
        public static int partNum=0;

        //// CATIA 관련 함수
        public bool InitializeCATIA(string filePath, int mode)
        {
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
                cPartDoc = (MECMOD.PartDocument)cDocs.Read(filePath);
            else if (mode == 1)
                cPartDoc = (MECMOD.PartDocument)cDocs.Add("Part");
                       
            cApp.Visible = true;

            cPart = cPartDoc.Part;
            cBodies = cPart.Bodies;

            cFactory = cPart.ShapeFactory;
            cShapeFactory = (PARTITF.ShapeFactory)cFactory;
            cHybridShapeFactory = (HybridShapeTypeLib.HybridShapeFactory)cPart.HybridShapeFactory;

            cCurrentBody = cBodies.Item(1);
            cShapes = cCurrentBody.Shapes;
            cSketches = cCurrentBody.Sketches;
            cOriginalElements = cPart.OriginElements;

            if (ReferenceManager == null)
                ReferenceManager = new Reference(this);

            return true;
        }

        public void UninitializeCATIA()
        {
            cPart = null;
            cBodies = null;
            cCurrentBody = null;
            cShapes = null;
            cFactory = null;
            cShapeFactory = null;
            cHybridShapeFactory = null;
            cSketches = null;
            cOriginalElements = null;
        }

        public void TranslateC2T() // From CATIA to TransCAD
        {
            IEnumerator cFeatureList = cShapes.GetEnumerator();
            
            while (cFeatureList.MoveNext())
            {
                MECMOD.Shape cShape = (MECMOD.Shape)cFeatureList.Current;

                if (cShape == null)
                {
                    // Sketch 혹은 DatumPlane만 있는 경우
                }
                else
                {
                    string cFeatureName = cShape.get_Name();

                    try
                    {
                        Feature pFeature = null;
                        
                        if (cFeatureName.Contains("Pad"))
                            pFeature = new FeaturePad(this);
                        else if (cFeatureName.Contains("Pocket"))
                            pFeature = new FeaturePocket(this);
                        else if (cFeatureName.Contains("EdgeFillet"))
                            pFeature = new FeatureEdgeFillet(this);
                        else if (cFeatureName.Contains("Rib"))
                            pFeature = new FeatureRib(this);
                        else if (cFeatureName.Contains("Slot"))
                            pFeature = new FeatureSlot(this);
                        else if (cFeatureName.Contains("Shaft"))
                            pFeature = new FeatureShaft(this);
                        else if (cFeatureName.Contains("Chamfer"))
                            pFeature = new FeatureChamfer(this);
                        else if (cFeatureName.Contains("Groove"))
                            pFeature = new FeatureGroove(this);
                        else if (cFeatureName.Contains("RectPattern"))
                            pFeature = new FeatureRectPattern(this);
                        else if (cFeatureName.Contains("CircPattern"))
                            pFeature = new FeatureCircPattern(this);
                        else if (cFeatureName.Contains("Hole"))
                            pFeature = new FeatureHole(this);

                        if (pFeature != null)
                            pFeature.TranslateC2T(cShape);
                    }
                    catch (Exception e)
                    {
                        MessageBox.Show("-Feature Name: " + cFeatureName + "\n-Error Message: " + e.Message, "Failed to create a feature!");
                    }
                }
            }
        }

        public void SaveCATPartFile(string filePath)
        {
           
            FileInfo fi = new FileInfo(filePath);

            // 파일이 이미 존재하면 제거
            if (fi.Exists)
                fi.Delete();

            // 파일 저장 및 문서 닫기
            try
            {
                cPartDoc.SaveAs(filePath);
            }
            catch
            {
                string currentDirec = System.Environment.CurrentDirectory;
                filePath = currentDirec + "\\" + filePath;
                cPartDoc.SaveAs(filePath);
            }

            cPartDoc.Close();
        }

        public INFITF.AnyObject GetRecentFeature()
        {
            // 가장 최근 생성된 특징형상 리턴
            MECMOD.Shape cShape = cShapes.Item(cShapes.Count - 1);

            if (cShape.get_Name().Contains("Pad"))
                return (PARTITF.Pad)cShape;
            else if (cShape.get_Name().Contains("Pocket"))
                return (PARTITF.Pocket)cShape;
            else if (cShape.get_Name().Contains("EdgeFillet"))
                return (PARTITF.ConstRadEdgeFillet)cShape;
            else if (cShape.get_Name().Contains("Chamfer"))
                return (PARTITF.Chamfer)cShape;
            else if (cShape.get_Name().Contains("Shaft"))
                return (PARTITF.Shaft)cShape;
            else if (cShape.get_Name().Contains("Groove"))
                return (PARTITF.Groove)cShape;
            else if (cShape.get_Name().Contains("Rib"))
                return (PARTITF.Rib)cShape;
            else if (cShape.get_Name().Contains("Slot"))
                return (PARTITF.Slot)cShape;
            else if (cShape.get_Name().Contains("Hole"))
                return (PARTITF.Hole)cShape;
            else if (cShape.get_Name().Contains("RectPattern"))
                return (PARTITF.RectPattern)cShape;
            else if (cShape.get_Name().Contains("CircPattern"))
                return (PARTITF.CircPattern)cShape;

            return null;
        }
        ////

        //// TransCAD 관련 함수
        public bool InitializeTransCAD(int mode)
        {
            try
            {
                tApp = (TransCAD.Application)Marshal.GetActiveObject("TransCAD.Application");
            }
            catch
            {
                tApp = (TransCAD.Application)Activator.CreateInstance(Type.GetTypeFromProgID("TransCAD.Application"));
            }

            if (tApp == null)
                return false;

            tDocs = tApp.Documents;

            if (mode == 0)  //pre일때 사용 PartDocument 추가 
                tPartDoc = tDocs.AddPartDocument();
            else if (mode == 1) //post일때 사용 이미 열려있는 것을 Active
                tPartDoc = (TransCAD.PartDocument)tApp.ActiveDocument;
            else if (mode == 2) //Assembly document에서 part 번역을 위한 initialization
            {
                tAssemDoc = (TransCAD.AssemDocument)tApp.ActiveDocument;
                isSubAssemblyPart = true;
                return true;
            }
                


                tApp.Visible = true;
            
            tPart = tPartDoc.Part;
            tFeatures = tPart.Features;

            if (ReferenceManager == null)
                ReferenceManager = new Reference(this);

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

        public void TranslateT2C() // From TransCAD to CATIA
        {
            IEnumerator tFeatureList = tFeatures.GetEnumerator();

            while (tFeatureList.MoveNext())
            {
                TransCAD.IFeature tFeature = (TransCAD.IFeature)tFeatureList.Current;

                try
                {
                    Feature pFeature = null;
                    
                    if (tFeature.Type == TransCAD.FeatureType.StdSolidProtrusionExtrudeFeature)
                        pFeature = new FeaturePad(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidCutExtrudeFeature)
                        pFeature = new FeaturePocket(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidFilletConstantFeature)
                        pFeature = new FeatureEdgeFillet(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidProtrusionSweepFeature)
                        pFeature = new FeatureRib(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidCutSweepFeature)
                        pFeature = new FeatureSlot(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidProtrusionRevolveFeature)
                        pFeature = new FeatureShaft(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidChamferFeature)
                        pFeature = new FeatureChamfer(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidCutRevolveFeature)
                        pFeature = new FeatureGroove(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidOperatePatternRectangularFeature)
                        pFeature = new FeatureRectPattern(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidOperatePatternCircularFeature)
                        pFeature = new FeatureCircPattern(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidHoleCounterboredFeature
                          || tFeature.Type == TransCAD.FeatureType.StdSolidHoleCountersunkFeature
                          || tFeature.Type == TransCAD.FeatureType.StdSolidHoleSimpleFeature)
                        pFeature = new FeatureHole(this);

                    if (pFeature != null)
                        pFeature.TranslateT2C(tFeature);
                }
                catch (Exception e)
                {
                    MessageBox.Show("-Feature Name: " + tFeature.Name + "\n-Error Message: " + e.Message, "Failed to create a feature!");
                }

                cPart.Update(); // CATIA Part 업데이트

            }
        }

        public void TranslateT2C(TransCAD.IPart tPart)
        {
            //IEnumerator tFeatureList = tPart.Features.GetEnumerator();
            TransCAD.Features tFeatures = tPart.Features;


            //TransCAD.IFeature tFeature = (TransCAD.IFeature)tFeatureList.Current;

            for (int i = 1; i<=tFeatures.Count; i++)
            {
                TransCAD.IFeature tFeature = (TransCAD.IFeature)tFeatures[i];
                try
                {
                    Feature pFeature = null;

                    if (tFeature.Type == TransCAD.FeatureType.StdSolidProtrusionExtrudeFeature)
                        pFeature = new FeaturePad(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidCutExtrudeFeature)
                        pFeature = new FeaturePocket(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidFilletConstantFeature)
                        pFeature = new FeatureEdgeFillet(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidProtrusionSweepFeature)
                        pFeature = new FeatureRib(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidCutSweepFeature)
                        pFeature = new FeatureSlot(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidProtrusionRevolveFeature)
                        pFeature = new FeatureShaft(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidChamferFeature)
                        pFeature = new FeatureChamfer(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidCutRevolveFeature)
                        pFeature = new FeatureGroove(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidOperatePatternRectangularFeature)
                        pFeature = new FeatureRectPattern(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidOperatePatternCircularFeature)
                        pFeature = new FeatureCircPattern(this);
                    else if (tFeature.Type == TransCAD.FeatureType.StdSolidHoleCounterboredFeature
                          || tFeature.Type == TransCAD.FeatureType.StdSolidHoleCountersunkFeature
                          || tFeature.Type == TransCAD.FeatureType.StdSolidHoleSimpleFeature)
                        pFeature = new FeatureHole(this);

                    if (pFeature != null)
                        pFeature.TranslateT2C(tFeature);
                }
                catch (Exception e)
                {
                    MessageBox.Show("-Feature Name: " + tFeature.Name + "\n-Error Message: " + e.Message, "Failed to create a feature!");
                }

                cPart.Update(); // CATIA Part 업데이트
            }


        }

        public TransCAD.IFeature GetFeatureFromReference(TransCAD.Reference reference)
        {
            // Reference 형식으로 저장 된 특징형상을 Feature 형식으로 리턴
            if (!isSubAssemblyPart)
            {
                IEnumerator tFeatureList = tFeatures.GetEnumerator();

                while (tFeatureList.MoveNext())
                {
                    TransCAD.IFeature tFeature = (TransCAD.IFeature)tFeatureList.Current;

                    if (tFeature.Name == reference.ReferenceeName)
                        return tFeature;
                }

            }else
            {
                tFeatures = tAssemDoc.Assem.GetComponent(1).GetPart(partNum).Features;

                for (int i = 1; i < tFeatures.Count; i++)
                {
                    TransCAD.IFeature tFeature = (TransCAD.IFeature)tFeatures[i];

                    if (tFeature.Name == reference.ReferenceeName)
                    {
                        return tFeature;
                    }
                }
            }

            return null;
        }

    }
}
