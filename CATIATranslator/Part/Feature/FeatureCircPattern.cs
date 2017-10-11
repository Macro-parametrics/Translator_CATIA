using System;
using System.Collections;

namespace CATIATranslator
{
    class FeatureCircPattern : Feature
    {
        public FeatureCircPattern(Part PartManager)
        {
            this.PartManager = PartManager;
        }
        
        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.CircPattern cCircPattern = (PARTITF.CircPattern)cShape;
            
            object[] center = new object[3];
            object[] axis = new object[3];

            cCircPattern.GetRotationCenter(center);
            cCircPattern.GetRotationAxis(axis);

            int angularCount = cCircPattern.AngularRepartition.InstancesCount.Value;
            double angularSpacing = cCircPattern.AngularRepartition.AngularSpacing.Value;

            int radialCount = cCircPattern.RadialRepartition.InstancesCount.Value;
            double radialSpacing = cCircPattern.RadialRepartition.Spacing.Value;

            bool isRadiallyAligned = cCircPattern.RadialAlignment;

            TransCAD.References refElements = PartManager.tPart.CreateReferences();
            TransCAD.Reference refElement = PartManager.tPart.SelectFeatureByName(cCircPattern.ItemToCopy.get_Name());

            if (refElement != null)
                refElements.Add(refElement);

            PartManager.tFeatures.AddNewSolidOperatePatternCircularFeature(cCircPattern.get_Name(), refElements, (double)center[0], (double)center[1], (double)center[2],
                                                        (double)axis[0], (double)axis[1], (double)axis[2], angularCount, angularSpacing, radialCount, radialSpacing, isRadiallyAligned);
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidOperatePatternCircularFeature tCircPattern = (TransCAD.IStdSolidOperatePatternCircularFeature)tFeature;

            double[] center = new double[3];
            double[] axis = new double[3];

            tCircPattern.CenterAxis(out center[0], out center[1], out center[2], out axis[0], out axis[1], out axis[2]);

            int angInstance = tCircPattern.AngleNumber;
            double angSpacing = tCircPattern.AngleIncrement;
            int radInstance = tCircPattern.RadialNumber;
            double radSpacing = tCircPattern.RadialSpacing;

            PARTITF.CircPattern cCircPattern = PartManager.cShapeFactory.AddNewCircPattern(null, 1, 2, 20.0, 45.0, 1, 1, null, null, true, 0.0, true);

            IEnumerator tTargetFeatures = tCircPattern.TargetFeatures.GetEnumerator();

            while (tTargetFeatures.MoveNext())
            {
                TransCAD.Reference tTargetFeature = (TransCAD.Reference)tTargetFeatures.Current;

                cCircPattern.ItemToCopy = PartManager.cShapes.Item(PartManager.ReferenceManager.NameMap[tTargetFeature.ReferenceeName]);
            }

            cCircPattern.CircularPatternParameters = PARTITF.CatCircularPatternParameters.catInstancesandAngularSpacing;
            
            INFITF.Reference rotationAxis = GetRotationAxis(cCircPattern, center, axis);
            cCircPattern.SetRotationAxis(rotationAxis);

            cCircPattern.AngularRepartition.InstancesCount.Value = angInstance;
            cCircPattern.AngularRepartition.AngularSpacing.Value = angSpacing;

            cCircPattern.RadialRepartition.InstancesCount.Value = radInstance;
            cCircPattern.RadialRepartition.Spacing.Value = radSpacing;

            PartManager.cPart.UpdateObject(cCircPattern);
            PartManager.ReferenceManager.NameMap.Add(tCircPattern.Name, cCircPattern.get_Name());
        }

        private INFITF.Reference GetRotationAxis(PARTITF.CircPattern cCircPattern, double[] center, double[] axis)
        {
            INFITF.Reference rotAxis = null;

            if (Math.Abs(axis[0]) == 1.0)
            {
                rotAxis = (INFITF.Reference)PartManager.cOriginalElements.PlaneYZ;

                if (axis[0] == -1.0)
                    cCircPattern.RotationOrientation = false;
            }
            else if (Math.Abs(axis[1]) == 1.0)
            {
                rotAxis = (INFITF.Reference)PartManager.cOriginalElements.PlaneZX;

                if (axis[1] == -1.0)
                    cCircPattern.RotationOrientation = false;
            }
            else if (Math.Abs(axis[2]) == 1.0)
            {
                rotAxis = (INFITF.Reference)PartManager.cOriginalElements.PlaneXY;

                if (axis[2] == -1.0)
                    cCircPattern.RotationOrientation = false;
            }
            else
            {
            }

            return rotAxis;
        }
    }
}
