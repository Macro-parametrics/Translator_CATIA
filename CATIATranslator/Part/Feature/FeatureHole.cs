using System;

namespace CATIATranslator
{
    class FeatureHole : Feature
    {
        public FeatureHole(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.Hole hole = (PARTITF.Hole)cShape;
            
            double diameter = 0.0;
            double depth = 0.0;
            double bottomAngle = 180.0;
            double headDiameter = 0.0;
            double headAngle = 0.0;
            double headDepth = 0.0;
            
            object[] org = new object[3];
            hole.GetOrigin(org);

            diameter = hole.Diameter.Value;
            depth = hole.BottomLimit.Dimension.Value;

            if (hole.BottomType == PARTITF.CatHoleBottomType.catVHoleBottom)
                bottomAngle = hole.BottomAngle.Value;

            if (hole.Type == PARTITF.CatHoleType.catCounterboredHole)
            {
                headDiameter = hole.HeadDiameter.Value;
                headDepth = hole.HeadDepth.Value;

                PartManager.tFeatures.AddNewSolidHoleCounterboredFeature(hole.get_Name(), GetTargetFace(hole), (double)org[0], (double)org[1], (double)org[2], headDiameter / 2, headDepth, diameter / 2, depth - headDepth, bottomAngle);
            }
            else if (hole.Type == PARTITF.CatHoleType.catCountersunkHole)
            {
                headDepth = hole.HeadDepth.Value;
                headAngle = hole.HeadAngle.Value;

                headDiameter = diameter + 2 * Math.Tan(headAngle / 2 * Math.PI / 180) * headDepth;

                PartManager.tFeatures.AddNewSolidHoleCountersunkFeature(hole.get_Name(), GetTargetFace(hole), (double)org[0], (double)org[1], (double)org[2], diameter / 2, depth, headDiameter / 2, headAngle, bottomAngle);
            }
            else if (hole.Type == PARTITF.CatHoleType.catSimpleHole)
            {
                PartManager.tFeatures.AddNewSolidHoleSimpleFeature(hole.get_Name(), GetTargetFace(hole), (double)org[0], (double)org[1], (double)org[2], diameter / 2, depth);
            }
        }

        private TransCAD.Reference GetTargetFace(PARTITF.Hole hole)
        {
            object[] axis = new object[9];

            hole.Sketch.GetAbsoluteAxisData(axis);

            TransCAD.Reference targetFace = PartManager.tPart.SelectPlaneByAxis((double)axis[0], (double)axis[1], (double)axis[2],
                                                                           (double)axis[3], (double)axis[4], (double)axis[5],
                                                                           (double)axis[6], (double)axis[7], (double)axis[8]);

            return targetFace;
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            double diameter = 0.0;
            double depth = 0.0;
            double bottomAngle = 180.0;
            double headDiameter = 0.0;
            double headAngle = 0.0;
            double headDepth = 0.0;

            double[] org = new double[3];

            PARTITF.Hole cHole = null;

            if (tFeature.Type == TransCAD.FeatureType.StdSolidHoleSimpleFeature)
            {
                TransCAD.IStdSolidHoleSimpleFeature tHoleSimple = (TransCAD.IStdSolidHoleSimpleFeature)tFeature;
                tHoleSimple.CenterPoint(out org[0], out org[1], out org[2]);
                depth = tHoleSimple.Depth;
                diameter = tHoleSimple.Radius * 2;
                INFITF.Reference targetFace = GetTargetFace(tHoleSimple.TargetFace.ReferenceeName);

                cHole = PartManager.cShapeFactory.AddNewHole(targetFace, 10.0);
                cHole.Type = PARTITF.CatHoleType.catSimpleHole;
                cHole.SetOrigin(org[0], org[1], org[2]);
                cHole.BottomLimit.Dimension.Value = depth;
                cHole.Diameter.Value = diameter;

                PartManager.ReferenceManager.NameMap.Add(tHoleSimple.Name, cHole.get_Name());
            }
            else if (tFeature.Type == TransCAD.FeatureType.StdSolidHoleCounterboredFeature)
            {
                TransCAD.StdSolidHoleCounterboredFeature tHoleCounterbored = (TransCAD.StdSolidHoleCounterboredFeature)tFeature;
                tHoleCounterbored.CenterPoint(out org[0], out org[1], out org[2]);
                headDepth = tHoleCounterbored.StartDepth;
                headDiameter = tHoleCounterbored.StartRadius * 2;
                depth = tHoleCounterbored.EndDepth + headDepth;
                diameter = tHoleCounterbored.EndRadius * 2;
                bottomAngle = tHoleCounterbored.Angle;
                INFITF.Reference targetFace = GetTargetFace(tHoleCounterbored.TargetFace.ReferenceeName);

                cHole = PartManager.cShapeFactory.AddNewHole(targetFace, 10.0);
                cHole.Type = PARTITF.CatHoleType.catCounterboredHole;
                cHole.SetOrigin(org[0], org[1], org[2]);
                cHole.HeadDepth.Value = headDepth;
                cHole.HeadDiameter.Value = headDiameter;
                cHole.BottomLimit.Dimension.Value = depth;
                cHole.Diameter.Value = diameter;

                PartManager.ReferenceManager.NameMap.Add(tHoleCounterbored.Name, cHole.get_Name());
            }
            else if (tFeature.Type == TransCAD.FeatureType.StdSolidHoleCountersunkFeature)
            {
                TransCAD.IStdSolidHoleCountersunkFeature tHoleCountersunk = (TransCAD.IStdSolidHoleCountersunkFeature)tFeature;
                tHoleCountersunk.CenterPoint(out org[0], out org[1], out org[2]);
                headAngle = tHoleCountersunk.SinkAngle;
                headDiameter = tHoleCountersunk.SinkRadius * 2;
                depth = tHoleCountersunk.Depth;
                diameter = tHoleCountersunk.Radius * 2;
                headDepth = 0.5 * (headDiameter - diameter) / Math.Tan(0.5 * headAngle * Math.PI / 180);

                bottomAngle = tHoleCountersunk.BottomAngle;
                INFITF.Reference targetFace = GetTargetFace(tHoleCountersunk.TargetFace.ReferenceeName);

                cHole = PartManager.cShapeFactory.AddNewHole(targetFace, 10.0);
                cHole.Type = PARTITF.CatHoleType.catCountersunkHole;
                cHole.CounterSunkMode = PARTITF.CatCSHoleMode.catCSModeAngleDiameter;
                cHole.SetOrigin(org[0], org[1], org[2]);
                cHole.HeadAngle.Value = headAngle;
                cHole.HeadDepth.Value = headDepth;
                cHole.BottomLimit.Dimension.Value = depth;
                cHole.Diameter.Value = diameter;

                PartManager.ReferenceManager.NameMap.Add(tHoleCountersunk.Name, cHole.get_Name());
            }

            if (bottomAngle != 180.0)
            {
                cHole.BottomType = PARTITF.CatHoleBottomType.catVHoleBottom;
                cHole.BottomAngle.Value = bottomAngle;
            }

            PartManager.cPart.UpdateObject(cHole);
        }

        private INFITF.Reference GetTargetFace(string tFaceName)
        {
            string targetFeatureName = tFaceName.Remove(tFaceName.IndexOf(","));
            MECMOD.Shape targetFeature = PartManager.cShapes.Item(PartManager.ReferenceManager.NameMap[targetFeatureName]);

            string targetFaceName = PartManager.ReferenceManager.GetCATIANameFromTransCADNameForHole(tFaceName);
            INFITF.Reference targetFace = PartManager.cPart.CreateReferenceFromBRepName(targetFaceName, targetFeature);

            return targetFace;
        }
    }
}
