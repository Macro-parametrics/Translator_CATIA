
namespace CATIATranslator
{
    class FeatureShaft : Feature
    {
        public FeatureShaft(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.Shaft cShaft = (PARTITF.Shaft)cShape;
            
            FeatureSketch SketchManager = new FeatureSketch(PartManager);
            SketchManager.TranslateC2T(cShaft.Sketch);

            KnowledgewareTypeLib.Angle first = cShaft.FirstAngle;
            KnowledgewareTypeLib.Angle second = cShaft.SecondAngle;
            
            double fA = first.Value;
            double sA = second.Value;

            PartManager.tFeatures.AddNewSolidProtrusionRevolveFeature(cShaft.get_Name(), SketchManager.tReference, fA, TransCAD.StdRevolveEndType.StdRevolveEndType_Blind, sA, TransCAD.StdRevolveEndType.StdRevolveEndType_Blind, false);
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidProtrusionRevolveFeature proRevolve = (TransCAD.IStdSolidProtrusionRevolveFeature)tFeature;

            FeatureSketch SketchManager = new FeatureSketch(PartManager);
            SketchManager.TranslateT2C(PartManager.GetFeatureFromReference(proRevolve.ProfileSketch));

            double sA = proRevolve.StartAngle;
            double eA = proRevolve.EndAngle;

            PARTITF.Shaft cShaft = PartManager.cShapeFactory.AddNewShaftFromRef(SketchManager.cReference);

            cShaft.FirstAngle.Value = sA;
            cShaft.SecondAngle.Value = eA;

            PartManager.cPart.UpdateObject(cShaft);
            PartManager.ReferenceManager.NameMap.Add(proRevolve.Name, cShaft.get_Name());
        }
    }
}
