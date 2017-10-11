
namespace CATIATranslator
{
    class FeaturePad : Feature
    {
        public FeaturePad(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.Pad cPad = (PARTITF.Pad)cShape;
            
            // 스케치 변환
            FeatureSketch SketchManager = new FeatureSketch(PartManager);
            SketchManager.TranslateC2T(cPad.Sketch);

            string name = cPad.get_Name();
            PARTITF.Limit first = cPad.FirstLimit;
            PARTITF.Limit second = cPad.SecondLimit;

            TransCAD.StdExtrudeEndType firstCond = TransCAD.StdExtrudeEndType.Blind;
            TransCAD.StdExtrudeEndType secondCond = TransCAD.StdExtrudeEndType.Blind;

            if (first.LimitMode.ToString() == "catUpToLastLimit")
                firstCond = TransCAD.StdExtrudeEndType.ThroughAll;

            if (second.LimitMode.ToString() == "catUpToLastLimit")
                secondCond = TransCAD.StdExtrudeEndType.ThroughAll;

            bool isFlip = false;

            if (cPad.DirectionOrientation == PARTITF.CatPrismOrientation.catInverseOrientation)
                isFlip = true;

            PartManager.tFeatures.AddNewSolidProtrusionExtrudeFeature(name, SketchManager.tReference,
                first.Dimension.Value, firstCond, second.Dimension.Value, secondCond, isFlip);
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidProtrusionExtrudeFeature proExtrude = (TransCAD.IStdSolidProtrusionExtrudeFeature)tFeature;

            // 스케치 변환
            FeatureSketch SketchManager = new FeatureSketch(PartManager);
            TransCAD.IFeature profilesketch = PartManager.GetFeatureFromReference(proExtrude.ProfileSketch);
            SketchManager.TranslateT2C(profilesketch);
            
            double sD = proExtrude.StartDepth;
            TransCAD.StdExtrudeEndType sC = proExtrude.StartCondition;
            double eD = proExtrude.EndDepth;
            TransCAD.StdExtrudeEndType eC = proExtrude.EndCondition;
            bool isFlip = proExtrude.IsFlip;

            PARTITF.Pad cPad = PartManager.cShapeFactory.AddNewPadFromRef(SketchManager.cReference, sD);
            cPad.FirstLimit.Dimension.Value = sD;
            cPad.SecondLimit.Dimension.Value = eD;

            if (sC == TransCAD.StdExtrudeEndType.Blind)
                cPad.FirstLimit.LimitMode = PARTITF.CatLimitMode.catOffsetLimit;
            else if (sC == TransCAD.StdExtrudeEndType.ThroughAll)
                cPad.FirstLimit.LimitMode = PARTITF.CatLimitMode.catUpToLastLimit;
            
            if (eC == TransCAD.StdExtrudeEndType.Blind)
                cPad.SecondLimit.LimitMode = PARTITF.CatLimitMode.catOffsetLimit;
            else if (eC == TransCAD.StdExtrudeEndType.ThroughAll)
                cPad.SecondLimit.LimitMode = PARTITF.CatLimitMode.catUpToLastLimit;

            if (isFlip == false)
                cPad.DirectionOrientation = PARTITF.CatPrismOrientation.catRegularOrientation;
            else if (isFlip == true)
                cPad.DirectionOrientation = PARTITF.CatPrismOrientation.catInverseOrientation;

            PartManager.cPart.UpdateObject(cPad);
            PartManager.ReferenceManager.NameMap.Add(proExtrude.Name, cPad.get_Name());
        }
}
}
