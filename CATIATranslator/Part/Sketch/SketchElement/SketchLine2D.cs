
namespace CATIATranslator
{
    class SketchLine2D
    {
        public FeatureSketch SketchManager = null;

        public SketchLine2D(FeatureSketch SketchManager)
        {
            this.SketchManager = SketchManager;
        }
        
        // Pre
        public void TranslateC2T(MECMOD.GeometricElement cGeom)
        {
            MECMOD.Line2D cLine = (MECMOD.Line2D)cGeom;
            
            object[] StartCoords = new object[2];
            object[] EndCoords = new object[2];

            string cLineName = cLine.get_Name();

            cLine.StartPoint.GetCoordinates(StartCoords);
            cLine.EndPoint.GetCoordinates(EndCoords);

            if (cLine == SketchManager.cCenterLine) // Centerline인 경우
                SketchManager.tSketchEditor.Create2DCenterline2Points(cLineName, (double)StartCoords.GetValue(0), (double)StartCoords.GetValue(1), (double)EndCoords.GetValue(0), (double)EndCoords.GetValue(1));
            else // 일반 Line인 경우
                SketchManager.tSketchEditor.Create2DLine2Points(cLineName, (double)StartCoords.GetValue(0), (double)StartCoords.GetValue(1), (double)EndCoords.GetValue(0), (double)EndCoords.GetValue(1));
        }

        // Post
        public void TranslateT2C(TransCAD.IStdSketchGeometry tGeom)
        {
            if (tGeom.Type == TransCAD.StdSketchGeometryType.Line) // 일반 Line인 경우
            {
                TransCAD.IStdSketchLine tLine = (TransCAD.IStdSketchLine)tGeom;
                TransCAD.StdSketchControlPoint tStartPoint = (TransCAD.StdSketchControlPoint)tLine.StartPoint;
                TransCAD.StdSketchControlPoint tEndPoint = (TransCAD.StdSketchControlPoint)tLine.EndPoint;

                double sX = tStartPoint.X;
                double sY = tStartPoint.Y;
                MECMOD.Point2D cStartPoint = SketchManager.cFactory.CreatePoint(sX, sY);
                cStartPoint.ReportName = ++SketchManager.cReportName;

                double eX = tEndPoint.X;
                double eY = tEndPoint.Y;
                MECMOD.Point2D cEndPoint = SketchManager.cFactory.CreatePoint(eX, eY);
                cEndPoint.ReportName = ++SketchManager.cReportName;

                MECMOD.Line2D cLine = SketchManager.cFactory.CreateLine(sX, sY, eX, eY);
                cLine.StartPoint = cStartPoint;
                cLine.EndPoint = cEndPoint;

                cLine.ReportName = ++SketchManager.cReportName;
            }
            else if (tGeom.Type == TransCAD.StdSketchGeometryType.Centerline) // Centerline인 경우
            {
                TransCAD.IStdSketchCenterline tLine = (TransCAD.IStdSketchCenterline)tGeom;
                TransCAD.StdSketchControlPoint tStartPoint = (TransCAD.StdSketchControlPoint)tLine.StartPoint;
                TransCAD.StdSketchControlPoint tEndPoint = (TransCAD.StdSketchControlPoint)tLine.EndPoint;

                double sX = tStartPoint.X;
                double sY = tStartPoint.Y;
                MECMOD.Point2D cStartPoint = SketchManager.cFactory.CreatePoint(sX, sY);
                cStartPoint.ReportName = ++SketchManager.cReportName;

                double eX = tEndPoint.X;
                double eY = tEndPoint.Y;
                MECMOD.Point2D cEndPoint = SketchManager.cFactory.CreatePoint(eX, eY);
                cEndPoint.ReportName = ++SketchManager.cReportName;

                MECMOD.Line2D cLine = SketchManager.cFactory.CreateLine(sX, sY, eX, eY);
                cLine.StartPoint = cStartPoint;
                cLine.EndPoint = cEndPoint;

                cLine.ReportName = ++SketchManager.cReportName;

                SketchManager.cCenterLine = cLine;
            }
        }
    }
}
