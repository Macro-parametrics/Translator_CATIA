using System;

namespace CATIATranslator
{
    class SketchCircle2D
    {
        public FeatureSketch SketchManager = null;

        public SketchCircle2D(FeatureSketch SketchManager)
        {
            this.SketchManager = SketchManager;
        }
        
        // Pre
        public void TranslateC2T(MECMOD.GeometricElement cGeom)
        {
            MECMOD.Circle2D cCircle = (MECMOD.Circle2D)cGeom;
            
            string cCircleName = cCircle.get_Name();

            object[] StartCoords = new object[2];
            object[] EndCoords = new object[2];
            object[] CenterCoords = new object[2];
            
            cCircle.StartPoint.GetCoordinates(StartCoords);
            cCircle.EndPoint.GetCoordinates(EndCoords);
            cCircle.GetCenter(CenterCoords);

            double cRadius = cCircle.Radius;

            // 반올림 (TransCAD에 2D 제약조건이 없어서 현재는 반드시 필요)
            Tool.Round(CenterCoords);
            Tool.Round(StartCoords);
            Tool.Round(EndCoords);

            // Circle인 경우
            if (Math.Abs((double)StartCoords.GetValue(0) - (double)EndCoords.GetValue(0)) < 0.00001 
                && Math.Abs((double)StartCoords.GetValue(1) - (double)EndCoords.GetValue(1)) < 0.00001)
            {
                SketchManager.tSketchEditor.Create2DCircleCenterPoint(cCircleName, (double)CenterCoords.GetValue(0), (double)CenterCoords.GetValue(1), cRadius);
            }
            else // Arc인 경우
            {
                double param = 0.0;
                object[] tangency = new object[2];

                cCircle.GetTangent(param, tangency);

                if ((double)tangency.GetValue(1) > 0.0)
                {
                    SketchManager.tSketchEditor.Create2DArcCenterStartEnd(cCircleName, (double)CenterCoords.GetValue(0), (double)CenterCoords.GetValue(1),
                                                                                       (double)StartCoords.GetValue(0), (double)StartCoords.GetValue(1),
                                                                                       (double)EndCoords.GetValue(0), (double)EndCoords.GetValue(1));
                }
                else
                {
                    SketchManager.tSketchEditor.Create2DArcCenterStartEnd(cCircleName, (double)CenterCoords.GetValue(0), (double)CenterCoords.GetValue(1),
                                                                                       (double)EndCoords.GetValue(0), (double)EndCoords.GetValue(1),
                                                                                       (double)StartCoords.GetValue(0), (double)StartCoords.GetValue(1));
                }
            }
        }

        // Post
        public void TranslateT2C(TransCAD.IStdSketchGeometry tGeom)
        {
            if (tGeom.Type == TransCAD.StdSketchGeometryType.Circle) // Circle인 경우
            {
                TransCAD.IStdSketchCircle tCircle = (TransCAD.IStdSketchCircle)tGeom;
                TransCAD.StdSketchControlPoint tCenterPoint = (TransCAD.StdSketchControlPoint)tCircle.CenterPoint;

                double cX = tCircle.CenterPoint.X;
                double cY = tCircle.CenterPoint.Y;
                
                MECMOD.Point2D cCenterPoint = SketchManager.cFactory.CreatePoint(cX, cY);
                cCenterPoint.ReportName = ++SketchManager.cReportName;

                double tRadius = tCircle.Radius;

                MECMOD.Circle2D cCircle = SketchManager.cFactory.CreateClosedCircle(cX, cY, tRadius);

                cCircle.CenterPoint = cCenterPoint;
                cCircle.ReportName = ++SketchManager.cReportName;
            }
            else if (tGeom.Type == TransCAD.StdSketchGeometryType.CircularArc) // Arc인 경우
            {
                TransCAD.IStdSketchCircularArc tArc = (TransCAD.IStdSketchCircularArc)tGeom;
                TransCAD.StdSketchControlPoint tCenterPoint = (TransCAD.StdSketchControlPoint)tArc.CenterPoint;
                TransCAD.StdSketchControlPoint tStartPoint = (TransCAD.StdSketchControlPoint)tArc.StartPoint;
                TransCAD.StdSketchControlPoint tEndPoint = (TransCAD.StdSketchControlPoint)tArc.EndPoint;
                
                double cX = tArc.CenterPoint.X;
                double cY = tArc.CenterPoint.Y;
                MECMOD.Point2D cCenterPoint = SketchManager.cFactory.CreatePoint(cX, cY);
                cCenterPoint.ReportName = ++SketchManager.cReportName;

                double sX = tStartPoint.X;
                double sY = tStartPoint.Y;
                MECMOD.Point2D cStartPoint = SketchManager.cFactory.CreatePoint(sX, sY);
                cStartPoint.ReportName = ++SketchManager.cReportName;

                double eX = tEndPoint.X;
                double eY = tEndPoint.Y;
                MECMOD.Point2D cEndPoint = SketchManager.cFactory.CreatePoint(eX, eY);
                cEndPoint.ReportName = ++SketchManager.cReportName;

                double sA = tArc.StartAngle;
                double eA = tArc.EndAngle;
                double tRadius = tArc.Radius;

                MECMOD.Circle2D cCircle = SketchManager.cFactory.CreateCircle(cX, cY, tRadius, sA, eA);

                cCircle.CenterPoint = cCenterPoint;
                cCircle.StartPoint = cStartPoint;
                cCircle.EndPoint = cEndPoint;

                cCircle.ReportName = ++SketchManager.cReportName;
            }
        }
    }
}
