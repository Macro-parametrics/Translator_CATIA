
namespace CATIATranslator
{
    class FeatureSlot : Feature
    {
        public FeatureSlot(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.Slot cSlot = (PARTITF.Slot)cShape;
            
            FeatureSketch ProfileSketch = new FeatureSketch(PartManager);
            ProfileSketch.TranslateC2T(cSlot.Sketch);
            TransCAD.Reference profile = ProfileSketch.tReference;

            FeatureSketch GuideSketch = new FeatureSketch(PartManager);
            GuideSketch.TranslateC2T(cSlot.CenterCurve);
            TransCAD.Reference guide = GuideSketch.tReference;

            PartManager.tFeatures.AddNewSolidCutSweepFeature(cSlot.get_Name(), profile, guide);
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidCutSweepFeature cutSweep = (TransCAD.IStdSolidCutSweepFeature)tFeature;

            FeatureSketch ProfileSketch = new FeatureSketch(PartManager);
            ProfileSketch.TranslateT2C(PartManager.GetFeatureFromReference(cutSweep.ProfileSketch));
            INFITF.Reference profile = ProfileSketch.cReference;
            
            FeatureSketch GuideSketch = new FeatureSketch(PartManager);
            GuideSketch.TranslateT2C(PartManager.GetFeatureFromReference(cutSweep.GuideCurve));
            INFITF.Reference guide = GuideSketch.cReference;

            PARTITF.Slot cSlot = PartManager.cShapeFactory.AddNewSlotFromRef(null, null);
            cSlot.SetProfileElement(profile);
            cSlot.CenterCurveElement = guide;

            PartManager.cPart.UpdateObject(cSlot);
            PartManager.ReferenceManager.NameMap.Add(cutSweep.Name, cSlot.get_Name());
        }
    }
}
