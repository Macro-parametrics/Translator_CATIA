
namespace CATIATranslator
{
    class FeatureGroove : Feature
    {
        public FeatureGroove(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.Groove cGroove = (PARTITF.Groove)cShape;
            
            FeatureSketch SketchManager = new FeatureSketch(PartManager);
            SketchManager.TranslateC2T(cGroove.Sketch);

            KnowledgewareTypeLib.Angle first = cGroove.FirstAngle;
            KnowledgewareTypeLib.Angle second = cGroove.SecondAngle;

            double fA = first.Value;
            double sA = second.Value;

            PartManager.tFeatures.AddNewSolidCutRevolveFeature(cGroove.get_Name(), SketchManager.tReference, fA, TransCAD.StdRevolveEndType.StdRevolveEndType_Blind, sA, TransCAD.StdRevolveEndType.StdRevolveEndType_Blind, false);
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidCutRevolveFeature cutRevolve = (TransCAD.IStdSolidCutRevolveFeature)tFeature;

            FeatureSketch SketchManager = new FeatureSketch(PartManager);
            SketchManager.TranslateT2C(PartManager.GetFeatureFromReference(cutRevolve.ProfileSketch));

            double sA = cutRevolve.StartAngle;
            double eA = cutRevolve.EndAngle;

            PARTITF.Groove cGroove = PartManager.cShapeFactory.AddNewGrooveFromRef(SketchManager.cReference);

            cGroove.FirstAngle.Value = sA;
            cGroove.SecondAngle.Value = eA;

            PartManager.cPart.UpdateObject(cGroove);
            PartManager.ReferenceManager.NameMap.Add(cutRevolve.Name, cGroove.get_Name());
        }
    }
}
