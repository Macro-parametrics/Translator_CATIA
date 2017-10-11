using System.Collections;

namespace CATIATranslator
{
    class FeatureSketch
    {
        public Part PartManager = null;

        public FeatureSketch(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // CATIA 변수
        public INFITF.Reference cReference = null;
        public MECMOD.Factory2D cFactory = null;
        public MECMOD.Line2D cCenterLine = null;
        public int cReportName = 0;

        // TransCAD 변수
        public TransCAD.Reference tReference = null;
        public TransCAD.SketchEditor tSketchEditor = null;
        
        // Pre
        public void TranslateC2T(MECMOD.Sketch cSketch)
        {
            string sketchName = cSketch.get_Name();
            MECMOD.GeometricElements cGeoms = cSketch.GeometricElements;

            cCenterLine = cSketch.CenterLine;
            
            object[] cCoords = new object[9];
            cSketch.GetAbsoluteAxisData(cCoords);

            TransCAD.Reference tSktReference = PartManager.ReferenceManager.GetTransCADSketchReference(cSketch);
            TransCAD.StdSketchFeature tSketch = PartManager.tFeatures.AddNewSketchFeature(sketchName, tSktReference);

            tSketch.SetCoordinateSystem((double)cCoords.GetValue(0), (double)cCoords.GetValue(1), (double)cCoords.GetValue(2),
                                       (double)cCoords.GetValue(3), (double)cCoords.GetValue(4), (double)cCoords.GetValue(5),
                                       (double)cCoords.GetValue(6), (double)cCoords.GetValue(7), (double)cCoords.GetValue(8));

            tSketchEditor = tSketch.OpenEditorEx(false);

            for (int i = 1; i <= cGeoms.Count; i++)
            {
                MECMOD.GeometricElement cGeom = cGeoms.Item(i);

                if (cGeom.GeometricType == MECMOD.CatGeometricType.catGeoTypeLine2D)
                {
                    SketchLine2D line2D = new SketchLine2D(this);
                    line2D.TranslateC2T(cGeom);
                }
                else if (cGeom.GeometricType == MECMOD.CatGeometricType.catGeoTypeCircle2D)
                {
                    SketchCircle2D circle2D = new SketchCircle2D(this);
                    circle2D.TranslateC2T(cGeom);
                }
            }

            tSketchEditor.Close();

            tReference = PartManager.tPart.SelectObjectByName(sketchName);
        }

        // Post
        public void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSketchFeature tSketch = (TransCAD.IStdSketchFeature)tFeature;

            string sketchName = tSketch.Name;
            TransCAD.IStdSketchGeometries tGeoms = tSketch.Geometries;

            double[] coords = new double[9];
            object[] tCoords = new object[9];

            tSketch.GetCoordinateSystem(out coords[0], out coords[1], out coords[2],
                                        out coords[3], out coords[4], out coords[5],
                                        out coords[6], out coords[7], out coords[8]);

            for (int i = 0; i < 9; ++i)
                tCoords[i] = coords[i];

            INFITF.Reference cSktReference = PartManager.ReferenceManager.GetCATIASketchReference(tSketch);
            MECMOD.Sketch cSketch = PartManager.cSketches.Add(cSktReference);
            cSketch.SetAbsoluteAxisData(tCoords);
            
            cFactory = cSketch.OpenEdition();

            // 스케치 로컬 좌표 축 설정
            MECMOD.GeometricElement axis = cSketch.GeometricElements.Item("AbsoluteAxis");
            INFITF.CATBaseDispatch localX = axis.GetItem("HDirection");
            MECMOD.Line2D locX = (MECMOD.Line2D)localX;
            locX.ReportName = ++cReportName;

            INFITF.CATBaseDispatch localY = axis.GetItem("VDirection");
            MECMOD.Line2D locY = (MECMOD.Line2D)localY;
            locX.ReportName = ++cReportName;

            IEnumerator tGeomList = tGeoms.GetEnumerator();

            while (tGeomList.MoveNext())
            {
                TransCAD.IStdSketchGeometry tGeom = (TransCAD.IStdSketchGeometry)tGeomList.Current;

                if (tGeom.Type == TransCAD.StdSketchGeometryType.Line
                    || tGeom.Type == TransCAD.StdSketchGeometryType.Centerline)
                {
                    SketchLine2D line2D = new SketchLine2D(this);
                    line2D.TranslateT2C(tGeom);

                    if (tGeom.Type == TransCAD.StdSketchGeometryType.Centerline)
                        cSketch.CenterLine = cCenterLine;
                }
                else if (tGeom.Type == TransCAD.StdSketchGeometryType.Circle
                    || tGeom.Type == TransCAD.StdSketchGeometryType.CircularArc)
                {
                    SketchCircle2D circle2D = new SketchCircle2D(this);
                    circle2D.TranslateT2C(tGeom);
                }
            }

            cSketch.CloseEdition();

            cReference = PartManager.cPart.CreateReferenceFromObject(cSketch);

            PartManager.cPart.InWorkObject = cSketch;
            PartManager.cPart.UpdateObject(cSketch);

            PartManager.ReferenceManager.NameMap.Add(tSketch.Name, cSketch.get_Name());
        }
    }
}
