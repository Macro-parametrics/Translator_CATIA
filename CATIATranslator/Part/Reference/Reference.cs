using System;
using System.Collections;
using System.Collections.Generic;

namespace CATIATranslator
{
    class Reference
    {
        public Part PartManager = null;
        public Dictionary<string, string> NameMap = new Dictionary<string, string>(); // Post-Processor에서 필요
        private Assembly assembly;

        public Reference(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        public Reference(Assembly assembly)
        {
            this.assembly = assembly;
        }

        //// Pre-Processor
        public string GetTransCADNameFromCATIAName(string cName)
        {
            string tName = null;

            if (cName.Contains("Selection_RSur") || cName.Contains("RSur") || cName.Contains("FSur")) // Face인 경우
            {
                string brepName = cName.Remove(cName.LastIndexOf(";None"));

                brepName = brepName.Replace("Selection_:", "");
                brepName = brepName.Replace("RSur:", "");
                brepName = brepName.Replace("FSur:", "");
                brepName = brepName.Replace("Face:", "");
                brepName = brepName.Replace("Brp:", "");
                brepName = brepName.Replace("(", "");
                brepName = brepName.Replace(")", "");
                brepName = brepName.Replace("0:", "");

                string[] cFaceName = brepName.Split(';');

                tName = GetTransCADBasicName(cFaceName) + ":0,0:0;0";
            }
            else if (cName.Contains("REdge")) // Edge인 경우
            {
                string brepName = cName.Remove(0, cName.IndexOf("REdge:"));
                brepName = brepName.Remove(brepName.LastIndexOf(";None"));

                brepName = brepName.Replace("REdge:", "");
                brepName = brepName.Replace("Edge:", "");
                brepName = brepName.Replace("Brp:", "");
                brepName = brepName.Replace(";None:();Cf11:())", "");
                brepName = brepName.Replace("(", "");
                brepName = brepName.Replace(")", "");
                brepName = brepName.Replace("0:", "");

                string cName1 = brepName.Remove(brepName.LastIndexOf("Face:") - 1, brepName.Length - brepName.LastIndexOf("Face:") + 1);
                cName1 = cName1.Replace("Face:", "");
                string cName2 = brepName.Remove(0, brepName.LastIndexOf("Face:"));
                cName2 = cName2.Replace("Face:", "");
                
                string[] cFaceName1 = cName1.Split(';');
                string[] cFaceName2 = cName2.Split(';');

                string tFaceName1 = GetTransCADBasicName(cFaceName1) + ":0,0:0;0";
                string tFaceName2 = GetTransCADBasicName(cFaceName2) + ":0,0:0;0";

                tName = tFaceName1 + "#" + tFaceName2;
            }

            return tName;
        }

        public string GetTransCADBasicName(string[] cName)
        {
            string tName = null;

            if (cName.Length == 2) // 시작 면 혹은 끝 면인 경우
            {
                tName = cName[0] + ",0,-" 
                    + cName[1] + ",0,0,0," 
                    + GetTransCADFeatureType(cName[0]);
            }
            else if (cName.Length == 3) // 옆 면인 경우 (일반 특징형상)
            {
                tName = cName[0] + "," 
                    + cName[1] + "," 
                    + GetSketchElementNameByReportName(cName[1], cName[2]) + ",0,0,0," 
                    + GetTransCADFeatureType(cName[0]);
            }
            else // 옆 면인 경우 (Sweep 특징형상)
            {
                if (cName.Length == 5)
                {
                    // 먼저 생성된 스케치 요소가 앞에 위치해야 함
                    if (Int32.Parse(cName[1].Replace("Sketch.", "")) < Int32.Parse(cName[3].Replace("Sketch.", "")))
                        tName = cName[0] + "," 
                            + cName[1] + "," 
                            + GetSketchElementNameByReportName(cName[1], cName[2]) + "," 
                            + cName[3] + "," 
                            + GetSketchElementNameByReportName(cName[3], cName[4]) + ",0," 
                            + GetTransCADFeatureType(cName[0]);
                    else
                        tName = cName[0] + "," 
                            + cName[3] + "," 
                            + GetSketchElementNameByReportName(cName[3], cName[4]) + "," 
                            + cName[1] + "," 
                            + GetSketchElementNameByReportName(cName[1], cName[2]) + ",0," 
                            + GetTransCADFeatureType(cName[0]);
                }
                else
                {
                    // Sweep에서 Edge 선택 시, 한쪽 사이드가 다 수의 면으로 구성 된 경우. 
                    // 향 후 고려
                }
            }

            return tName;
        }

        public string GetSketchElementNameByReportName(string cSketchName, string cReportName)
        {
            string cSketchElementName = null;

            for (int i = 1; i <= PartManager.cSketches.Count; ++i)
            {
                MECMOD.Sketch cSketch = PartManager.cSketches.Item(i);

                // 해당 스케치면 진입
                if (cSketch.get_Name() == cSketchName)
                {
                    // 해당 스케치의 스케치 요소 탐색
                    for (int j = 1; j <= cSketch.GeometricElements.Count; ++j)
                    {
                        MECMOD.GeometricElement cGeom = cSketch.GeometricElements.Item(j);

                        string cTempReportName = null;

                        if (cGeom.GeometricType == MECMOD.CatGeometricType.catGeoTypeLine2D)
                            cTempReportName = ((MECMOD.Line2D)cGeom).ReportName.ToString();
                        else if (cGeom.GeometricType == MECMOD.CatGeometricType.catGeoTypeCircle2D)
                            cTempReportName = ((MECMOD.Circle2D)cGeom).ReportName.ToString();
                        else if (cGeom.GeometricType == MECMOD.CatGeometricType.catGeoTypeEllipse2D)
                            cTempReportName = ((MECMOD.Ellipse2D)cGeom).ReportName.ToString();
                        else if (cGeom.GeometricType == MECMOD.CatGeometricType.catGeoTypeHyperbola2D)
                            cTempReportName = ((MECMOD.Hyperbola2D)cGeom).ReportName.ToString();
                        else if (cGeom.GeometricType == MECMOD.CatGeometricType.catGeoTypeParabola2D)
                            cTempReportName = ((MECMOD.Parabola2D)cGeom).ReportName.ToString();
                        else if (cGeom.GeometricType == MECMOD.CatGeometricType.catGeoTypeSpline2D)
                            cTempReportName = ((MECMOD.Spline2D)cGeom).ReportName.ToString();

                        // ReportName이 동일하면 해당 요소의 이름을 리턴
                        if (cTempReportName == cReportName)
                        {
                            cSketchElementName = cGeom.get_Name();
                            break;
                        }
                    }

                    break;
                }
            }

            return cSketchElementName;
        }

        public string GetTransCADFeatureType(string cFeatureName)
        {
            string tFeatureType = null;

            if (cFeatureName.Contains("Pad") || cFeatureName.Contains("Pocket"))
                tFeatureType = "ExtrudeFeature";
            else if (cFeatureName.Contains("Shaft") || cFeatureName.Contains("Groove"))
                tFeatureType = "RevolveFeature";
            else if (cFeatureName.Contains("Rib") || cFeatureName.Contains("Slot"))
                tFeatureType = "SweepFeature";
            else if (cFeatureName.Contains("RectPattern") || cFeatureName.Contains("CircPattern"))
                tFeatureType = "PatternFeature";
            else if (cFeatureName.Contains("EdgeFillet"))
                tFeatureType = "FilletFeature";
            else if (cFeatureName.Contains("Chamfer"))
                tFeatureType = "ChamferFeature";

            return tFeatureType;
        }

        public TransCAD.Reference GetTransCADSketchReference(MECMOD.Sketch cSketch)
        {
            object[] axis = new object[9];
            cSketch.GetAbsoluteAxisData(axis);
            Tool.Round(axis);

            string cSketchName = cSketch.get_Name();
            TransCAD.Reference tRef = null;

            // CATIA 스케치 좌표계로부터 TransCAD에서 알맞은 레퍼런스를 선택
            // 만약, CATIA 스케치 레퍼런스 이름을 얻어올 수 있으면 다른 방식으로 처리 가능
            if (GetTransCADPlaneType(axis) == 1)// YZ 평면
            {
                if (axis.GetValue(0).Equals(0.0)) // X 좌표가 0일 때
                {
                    tRef = PartManager.tPart.SelectObjectByName("YZPlane");
                }
                else
                {
                    // 면 선택
                    tRef = PartManager.tPart.SelectPlaneByAxis((double)axis.GetValue(0), (double)axis.GetValue(1), (double)axis.GetValue(2),
                                                (double)axis.GetValue(3), (double)axis.GetValue(4), (double)axis.GetValue(5),
                                                (double)axis.GetValue(6), (double)axis.GetValue(7), (double)axis.GetValue(8));
                    // 새평면 생성
                    if (tRef == null)
                    {
                        tRef = PartManager.tPart.SelectObjectByName("YZPlane");

                        double offset = (double)axis.GetValue(0);

                        TransCAD.StdDatumPlaneOffsetFeature tDatum = null;

                        if (offset > 0.0)
                            tDatum = PartManager.tFeatures.AddNewDatumPlaneOffsetFeature("Plane for " + cSketchName, tRef, offset, false);
                        else
                            tDatum = PartManager.tFeatures.AddNewDatumPlaneOffsetFeature("Plane for " + cSketchName, tRef, Math.Abs(offset), true);

                        tRef = PartManager.tPart.SelectObjectByName(tDatum.Name);
                    }
                }
            }
            else if (GetTransCADPlaneType(axis) == 0)    // XY 평면
            {
                if (axis.GetValue(2).Equals(0.0)) // Z좌표가 0일 때
                {
                    tRef = PartManager.tPart.SelectObjectByName("XYPlane");
                }
                else
                {
                    // 면 선택
                    tRef = PartManager.tPart.SelectPlaneByAxis((double)axis.GetValue(0), (double)axis.GetValue(1), (double)axis.GetValue(2),
                                                (double)axis.GetValue(3), (double)axis.GetValue(4), (double)axis.GetValue(5),
                                                (double)axis.GetValue(6), (double)axis.GetValue(7), (double)axis.GetValue(8));

                    // 새평면 생성
                    if (tRef == null)
                    {
                        tRef = PartManager.tPart.SelectObjectByName("XYPlane");

                        double offset = (double)axis.GetValue(2);

                        TransCAD.StdDatumPlaneOffsetFeature tDatum = null;

                        if (offset > 0.0)
                            tDatum = PartManager.tFeatures.AddNewDatumPlaneOffsetFeature("Plane for " + cSketchName, tRef, offset, false);
                        else
                            tDatum = PartManager.tFeatures.AddNewDatumPlaneOffsetFeature("Plane for " + cSketchName, tRef, Math.Abs(offset), true);

                        tRef = PartManager.tPart.SelectObjectByName(tDatum.Name);
                    }
                }
            }
            else if (GetTransCADPlaneType(axis) == 2)    // ZX 평면
            {
                if (axis.GetValue(1).Equals(0.0)) // Y좌표가 0일 때
                {
                    tRef = PartManager.tPart.SelectObjectByName("ZXPlane");
                }
                else
                {
                    // 면 선택
                    tRef = PartManager.tPart.SelectPlaneByAxis((double)axis.GetValue(0), (double)axis.GetValue(1), (double)axis.GetValue(2),
                                                (double)axis.GetValue(3), (double)axis.GetValue(4), (double)axis.GetValue(5),
                                                (double)axis.GetValue(6), (double)axis.GetValue(7), (double)axis.GetValue(8));
                    // 새평면 생성
                    if (tRef == null)
                    {
                        tRef = PartManager.tPart.SelectObjectByName("ZXPlane");

                        double offset = (double)axis.GetValue(1);

                        TransCAD.StdDatumPlaneOffsetFeature tDatum = null;

                        if (offset > 0.0)
                            tDatum = PartManager.tFeatures.AddNewDatumPlaneOffsetFeature("Plane for " + cSketchName, tRef, offset, false);
                        else
                            tDatum = PartManager.tFeatures.AddNewDatumPlaneOffsetFeature("Plane for " + cSketchName, tRef, Math.Abs(offset), true);

                        tRef = PartManager.tPart.SelectObjectByName(tDatum.Name);
                    }
                }
            }
            else // 이 외의 경우
            {
                tRef = PartManager.tPart.SelectPlaneByAxis((double)axis.GetValue(0), (double)axis.GetValue(1), (double)axis.GetValue(2),
                                                           (double)axis.GetValue(3), (double)axis.GetValue(4), (double)axis.GetValue(5),
                                                           (double)axis.GetValue(6), (double)axis.GetValue(7), (double)axis.GetValue(8));
                // 새 평면 생성
                if (tRef == null)
                {
                    TransCAD.StdDatumPlaneOffsetFeature tDatum = PartManager.tFeatures.AddNewDatumPlaneOffsetFeature2("Plane for " + cSketchName,
                                                (double)axis.GetValue(0), (double)axis.GetValue(1), (double)axis.GetValue(2),
                                                (double)axis.GetValue(3), (double)axis.GetValue(4), (double)axis.GetValue(5),
                                                (double)axis.GetValue(6), (double)axis.GetValue(7), (double)axis.GetValue(8), false);

                    tRef = PartManager.tPart.SelectObjectByName(tDatum.Name);
                }
            }

            return tRef;
        }

        public int GetTransCADPlaneType(object[] axis)
        {
            int type;

            double[] localX = new double[3];
            double[] localY = new double[3];
            double[] localZ = new double[3];

            for (int i = 0; i < 3; i++)
            {
                localX[i] = (double)axis[i + 3];
                localY[i] = (double)axis[i + 6];
            }

            // 평면의 법선 벡터
            localZ[0] = localX[1] * localY[2] - localX[2] * localY[1];
            localZ[1] = localX[2] * localY[0] - localX[0] * localY[2];
            localZ[2] = localX[0] * localY[1] - localX[1] * localY[0];

            if (localZ[0] == 0.0 && localZ[1] == 0.0 && localZ[2] == 1.0)       // XY
                type = 0;
            else if (localZ[0] == 1.0 && localZ[1] == 0.0 && localZ[2] == 0.0)  // YZ
                type = 1;
            else if (localZ[0] == 0.0 && localZ[1] == 1.0 && localZ[2] == 0.0)  // ZX
                type = 2;
            else    // 기타
                type = 3;

            return type;
        }
        ////

        //// Post-Processor
        public string GetCATIANameFromTransCADName(string tName)
        {
            string cName = null;
            string cFeatureName = null;

            if (tName.Contains("#")) // Edge인 경우
            {
                string[] tEdgeName = tName.Split('#');
                string[] tFaceName1 = tEdgeName[0].Split(',');
                string[] tFaceName2 = tEdgeName[1].Split(',');

                string cFaceName1 = GetCATIABasicName(tFaceName1);
                string cFaceName2 = GetCATIABasicName(tFaceName2);

                cName = "REdge:(Edge:(Face:(Brp:("
                    + cFaceName1
                    + ");None:();Cf11:());Face:(Brp:("
                    + cFaceName2
                    + ");None:();Cf11:());None:(Limits1:();Limits2:());Cf11:());WithTemporaryBody;WithoutBuildError;WithSelectingFeatureSupport;MFBRepVersion_CXR15)";
            }
            else // Face인 경우
            {
                string[] tFaceName = tName.Split(',');
                string cFaceName = GetCATIABasicName(tFaceName);
                cFeatureName = NameMap[tFaceName[0]];

                cName = "Selection_RSur:(Face:(Brp:("
                    + cFaceName
                    + ");None:();Cf11:());"
                    + cFeatureName
                    + "_ResultOUT;Z0;G4074)";
            }
            
            return cName;
        }

        public string GetCATIANameFromTransCADNameForHole(string tName)
        {
            // Hole 생성할 때 선택한 면의 이름과 일반 특징형상 생성할 때 선택한 면의 이름과 다름
            string cName = null;
            
            string[] tFaceName = tName.Split(',');
            string cFaceName = GetCATIABasicName(tFaceName);

            cName = "FSur:(Face:(Brp:("
                    + cFaceName
                    + ");None:();Cf11:());WithTemporaryBody;WithoutBuildError;WithInitialFeatureSupport;MonoFond;MFBRepVersion_CXR15)";

            return cName;
        }

        public string GetCATIABasicName(string[] tName)
        {
            string catName = null;

            string cFeatureName = NameMap[tName[0]];

            if (tName[1] == "0") // 시작 면 또는 끝 면인 경우
            {
                if (tName[2] == "-1")
                    catName = cFeatureName + ";1";
                else if (tName[2] == "-2")
                    catName = cFeatureName + ";2";
            }
            else // 옆 면인 경우
            {
                string sketchName1 = NameMap[tName[1]];
                string sketchElementName1 = GetReportNameBySketchElementName(tName[1], tName[2]);

                if (tName[3] == "0") // 스케치 1개의 경우
                {
                    catName = cFeatureName + ";0:(Brp:("
                        + sketchName1 + ";"
                        + sketchElementName1 + "))";
                }
                else // 스케치 2개의 경우 (ex. Sweep)
                {
                    string sketchName2 = NameMap[tName[3]];
                    string sketchElementName2 = GetReportNameBySketchElementName(tName[3], tName[4]);

                    catName = cFeatureName + ";0:(Brp:("
                        + sketchName1 + ";"
                        + sketchElementName1 + ");Brp:("
                        + sketchName2 + ";"
                        + sketchElementName2 + "))";
                }
            }

            return catName;
        }
        
        public string GetReportNameBySketchElementName(string tSketchName, string tSketchElementName)
        {
            string cReportName = null;

            IEnumerator tFeatureList = PartManager.tFeatures.GetEnumerator();

            while (tFeatureList.MoveNext())
            {
                TransCAD.IFeature tFeature = (TransCAD.IFeature)tFeatureList.Current;

                if (tFeature.Name == tSketchName)
                {
                    TransCAD.IStdSketchFeature tSketch = (TransCAD.IStdSketchFeature)tFeature;

                    IEnumerator tGeoms = tSketch.Geometries.GetEnumerator();
                    int count = 2; // localX, localY 포함

                    while (tGeoms.MoveNext())
                    {
                        TransCAD.IStdSketchGeometry tGeom = (TransCAD.IStdSketchGeometry)tGeoms.Current;
                        count++;

                        if (tGeom.Name == tSketchElementName)
                        {
                            cReportName = count.ToString();
                            break;
                        }
                    }

                    break;
                }
            }

            return cReportName;
        }

        public INFITF.Reference GetCATIASketchReference(TransCAD.IStdSketchFeature tSketch)
        {
            INFITF.Reference cRef = null;
            string tRefName = tSketch.SketchPlane.ReferenceeName;

            if (tSketch.SketchPlane.Type == TransCAD.ReferenceType.ExplicitModelObject) // 평면인 경우
            {
                if (tRefName == "XYPlane")
                    cRef = (INFITF.Reference)PartManager.cOriginalElements.PlaneXY;
                else if (tRefName == "YZPlane")
                    cRef = (INFITF.Reference)PartManager.cOriginalElements.PlaneYZ;
                else if (tRefName == "ZXPlane")
                    cRef = (INFITF.Reference)PartManager.cOriginalElements.PlaneZX;
                else
                {
                    // DatumPlaneOffset
                }
            }
            else if (tSketch.SketchPlane.Type == TransCAD.ReferenceType.Brep) // B-rep 면인 경우
            {
                string cRefName = GetCATIANameFromTransCADName(tRefName);

                cRef = PartManager.cPart.CreateReferenceFromName(cRefName);
            }

            return cRef;
        }
        ////
    }
}
