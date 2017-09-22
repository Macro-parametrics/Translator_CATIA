
namespace CATIATranslator
{
    class FeatureRib : Feature
    {
        public FeatureRib(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.Rib cRib = (PARTITF.Rib)cShape;
            
            FeatureSketch ProfileSketch = new FeatureSketch(PartManager);
            ProfileSketch.TranslateC2T(cRib.Sketch);
            TransCAD.Reference profile = ProfileSketch.tReference;

            FeatureSketch GuideSketch = new FeatureSketch(PartManager);
            GuideSketch.TranslateC2T(cRib.CenterCurve);
            TransCAD.Reference guide = GuideSketch.tReference;

            PartManager.tFeatures.AddNewSolidProtrusionSweepFeature(cRib.get_Name(), profile, guide);
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidProtrusionSweepFeature proSweep = (TransCAD.IStdSolidProtrusionSweepFeature)tFeature;

            FeatureSketch ProfileSketch = new FeatureSketch(PartManager);
            ProfileSketch.TranslateT2C(PartManager.GetFeatureFromReference(proSweep.ProfileSketch));
            INFITF.Reference profile = ProfileSketch.cReference;

            FeatureSketch GuideSketch = new FeatureSketch(PartManager);
            GuideSketch.TranslateT2C(PartManager.GetFeatureFromReference(proSweep.GuideCurve));
            INFITF.Reference guide = GuideSketch.cReference;

            PARTITF.Rib cRib = PartManager.cShapeFactory.AddNewRibFromRef(null, null);
            cRib.SetProfileElement(profile);
            cRib.CenterCurveElement = guide;

            PartManager.cPart.UpdateObject(cRib);
            PartManager.ReferenceManager.NameMap.Add(proSweep.Name, cRib.get_Name());
        }
    }
}
