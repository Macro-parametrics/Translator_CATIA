
namespace CATIATranslator
{
    class FeaturePocket : Feature
    {
        public FeaturePocket(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.Pocket cPocket = (PARTITF.Pocket)cShape;
            
            FeatureSketch SketchManager = new FeatureSketch(PartManager);
            SketchManager.TranslateC2T(cPocket.Sketch);

            string name = cPocket.get_Name();
            PARTITF.Limit first = cPocket.FirstLimit;
            PARTITF.Limit second = cPocket.SecondLimit;

            TransCAD.StdExtrudeEndType firstCond = TransCAD.StdExtrudeEndType.Blind;
            TransCAD.StdExtrudeEndType secondCond = TransCAD.StdExtrudeEndType.Blind;

            if (first.LimitMode.ToString() == "catUpToLastLimit")
                firstCond = TransCAD.StdExtrudeEndType.ThroughAll;

            if (second.LimitMode.ToString() == "catUpToLastLimit")
                secondCond = TransCAD.StdExtrudeEndType.ThroughAll;

            bool isFlip = false;

            if (cPocket.DirectionOrientation == PARTITF.CatPrismOrientation.catRegularOrientation)
                isFlip = true;

            PartManager.tFeatures.AddNewSolidCutExtrudeFeature(name, SketchManager.tReference, 
                first.Dimension.Value, firstCond, second.Dimension.Value, secondCond, isFlip);
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidCutExtrudeFeature cutExtrude = (TransCAD.IStdSolidCutExtrudeFeature)tFeature;

            FeatureSketch SketchManager = new FeatureSketch(PartManager);
            SketchManager.TranslateT2C(PartManager.GetFeatureFromReference(cutExtrude.ProfileSketch));

            double sD = cutExtrude.StartDepth;
            TransCAD.StdExtrudeEndType sC = cutExtrude.StartCondition;
            double eD = cutExtrude.EndDepth;
            TransCAD.StdExtrudeEndType eC = cutExtrude.EndCondition;
            bool isFlip = cutExtrude.IsFlip;

            PARTITF.Pocket cPocket = PartManager.cShapeFactory.AddNewPocketFromRef(SketchManager.cReference, sD);
            cPocket.FirstLimit.Dimension.Value = sD;
            cPocket.SecondLimit.Dimension.Value = eD;

            if (sC == TransCAD.StdExtrudeEndType.Blind)
                cPocket.FirstLimit.LimitMode = PARTITF.CatLimitMode.catOffsetLimit;
            else if (sC == TransCAD.StdExtrudeEndType.ThroughAll)
                cPocket.FirstLimit.LimitMode = PARTITF.CatLimitMode.catUpToLastLimit;

            if (eC == TransCAD.StdExtrudeEndType.Blind)
                cPocket.SecondLimit.LimitMode = PARTITF.CatLimitMode.catOffsetLimit;
            else if (eC == TransCAD.StdExtrudeEndType.ThroughAll)
                cPocket.SecondLimit.LimitMode = PARTITF.CatLimitMode.catUpToLastLimit;

            if (isFlip == false)
                cPocket.DirectionOrientation = PARTITF.CatPrismOrientation.catInverseOrientation;
            else if (isFlip == true)
                cPocket.DirectionOrientation = PARTITF.CatPrismOrientation.catRegularOrientation;

            PartManager.cPart.UpdateObject(cPocket);
            PartManager.ReferenceManager.NameMap.Add(cutExtrude.Name, cPocket.get_Name());
        }
    }
}
