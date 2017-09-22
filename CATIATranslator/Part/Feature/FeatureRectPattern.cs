using System;
using System.Collections;

namespace CATIATranslator
{
    class FeatureRectPattern : Feature
    {
        public FeatureRectPattern(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.RectPattern cRectPattern = (PARTITF.RectPattern)cShape;
            
            object[] firstDir = new object[3];
            object[] secondDir = new object[3];

            GetDirections(cRectPattern, firstDir, secondDir);

            int firstCount = cRectPattern.FirstDirectionRepartition.InstancesCount.Value;
            double firstSpacing = Math.Abs(cRectPattern.FirstDirectionRepartition.Spacing.Value);

            int secondCount = cRectPattern.SecondDirectionRepartition.InstancesCount.Value;
            double secondSpacing = Math.Abs(cRectPattern.SecondDirectionRepartition.Spacing.Value);

            TransCAD.References refElements = PartManager.tPart.CreateReferences();
            TransCAD.Reference refElement = PartManager.tPart.SelectFeatureByName(cRectPattern.ItemToCopy.get_Name());
            
            if (refElement != null)
                refElements.Add(refElement);

            PartManager.tFeatures.AddNewSolidOperatePatternRectangularFeature(cRectPattern.get_Name(), refElements, firstSpacing, (double)firstDir[0], (double)firstDir[1], (double)firstDir[2],
                                                                              firstCount, secondSpacing, (double)secondDir[0], (double)secondDir[1], (double)secondDir[2], secondCount);
        }

        private void GetDirections(PARTITF.RectPattern cRectPattern, object[] firstDir, object[] secondDir)
        {
            cRectPattern.GetFirstDirection(firstDir);
            cRectPattern.GetSecondDirection(secondDir);

            if (cRectPattern.FirstOrientation == false || cRectPattern.FirstDirectionRepartition.Spacing.Value < 0.0)
            {
                firstDir[0] = (-1) * (double)firstDir[0];
                firstDir[1] = (-1) * (double)firstDir[1];
                firstDir[2] = (-1) * (double)firstDir[2];
            }

            if (cRectPattern.SecondOrientation == false || cRectPattern.SecondDirectionRepartition.Spacing.Value < 0.0)
            {
                secondDir[0] = (-1) * (double)secondDir[0];
                secondDir[1] = (-1) * (double)secondDir[1];
                secondDir[2] = (-1) * (double)secondDir[2];
            }
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidOperatePatternRectangularFeature tRectPattern = (TransCAD.IStdSolidOperatePatternRectangularFeature)tFeature;

            double[] colDir = new double[3];
            double[] rowDir = new double[3];

            tRectPattern.ColumnDirection(out colDir[0], out colDir[1], out colDir[2]);
            tRectPattern.RowDirection(out rowDir[0], out rowDir[1], out rowDir[2]);
            
            int colInstance = tRectPattern.ColumnNumber;
            double colSpacing = tRectPattern.ColumnSpacing;
            int rowInstance = tRectPattern.RowNumber;
            double rowSpacing = tRectPattern.RowSpacing;

            PARTITF.RectPattern cRectPattern = PartManager.cShapeFactory.AddNewRectPattern(null, 2, 1, 20.0, 20.0, 1, 1, null, null, true, true, 0.0);

            IEnumerator tTargetFeatures = tRectPattern.TargetFeatures.GetEnumerator();

            while (tTargetFeatures.MoveNext())
            {
                TransCAD.Reference tTargetFeature = (TransCAD.Reference)tTargetFeatures.Current;

                cRectPattern.ItemToCopy = PartManager.cShapes.Item(PartManager.ReferenceManager.NameMap[tTargetFeature.ReferenceeName]);
            }
            
            cRectPattern.FirstRectangularPatternParameters = PARTITF.CatRectangularPatternParameters.catInstancesandSpacing;
            cRectPattern.SecondRectangularPatternParameters = PARTITF.CatRectangularPatternParameters.catInstancesandSpacing;

            INFITF.Reference referencePlane = GetReferencePlane(cRectPattern, colDir, rowDir);
            cRectPattern.SetFirstDirection(referencePlane);
            cRectPattern.SetSecondDirection(referencePlane);

            cRectPattern.FirstDirectionRepartition.InstancesCount.Value = colInstance;
            cRectPattern.FirstDirectionRepartition.Spacing.Value = colSpacing;

            cRectPattern.SecondDirectionRepartition.InstancesCount.Value = rowInstance;
            cRectPattern.SecondDirectionRepartition.Spacing.Value = rowSpacing;

            PartManager.cPart.UpdateObject(cRectPattern);
            PartManager.ReferenceManager.NameMap.Add(tRectPattern.Name, cRectPattern.get_Name());
        }

        private INFITF.Reference GetReferencePlane(PARTITF.RectPattern cRectPattern, double[] firstDir, double[] secondDir)
        {
            INFITF.Reference refPlane = null;

            // XY 평면
            if ((Math.Abs(firstDir[0]) == 1.0 && Math.Abs(secondDir[1]) == 1.0)
                || (Math.Abs(firstDir[1]) == 1.0 && Math.Abs(secondDir[0]) == 1.0))
            {
                refPlane = (INFITF.Reference)PartManager.cOriginalElements.PlaneXY;

                if (firstDir[0] < 0.0 || firstDir[1] < 0.0)
                    cRectPattern.FirstOrientation = false;

                if (secondDir[1] < 0.0 || secondDir[0] < 0.0)
                    cRectPattern.SecondOrientation = false;
            }
            // YZ 평면
            else if ((Math.Abs(firstDir[1]) == 1.0 && Math.Abs(secondDir[2]) == 1.0)
                || (Math.Abs(firstDir[2]) == 1.0 && Math.Abs(secondDir[1]) == 1.0))
            {
                refPlane = (INFITF.Reference)PartManager.cOriginalElements.PlaneYZ;

                if (firstDir[1] < 0.0 || firstDir[2] < 0.0)
                    cRectPattern.FirstOrientation = false;

                if (secondDir[2] < 0.0 || secondDir[1] < 0.0)
                    cRectPattern.SecondOrientation = false;
            }
            // ZX 평면
            else if ((Math.Abs(firstDir[2]) == 1.0 && Math.Abs(secondDir[0]) == 1.0)
                || (Math.Abs(firstDir[0]) == 1.0 && Math.Abs(secondDir[2]) == 1.0))
            {
                refPlane = (INFITF.Reference)PartManager.cOriginalElements.PlaneZX;

                if (firstDir[2] < 0.0 || firstDir[0] < 0.0)
                    cRectPattern.FirstOrientation = false;

                if (secondDir[0] < 0.0 || secondDir[2] < 0.0)
                    cRectPattern.SecondOrientation = false;
            }
            // 기타 경우. 고려 필요
            else
            {

            }

            return refPlane;
        }
    }
}

