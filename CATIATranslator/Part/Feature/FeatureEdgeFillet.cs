using System.Collections;

namespace CATIATranslator
{
    class FeatureEdgeFillet : Feature
    {
        public FeatureEdgeFillet(Part PartManager)
        {
            this.PartManager = PartManager;
        }

        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.ConstRadEdgeFillet cFillet = (PARTITF.ConstRadEdgeFillet)cShape;

            double radius = cFillet.Radius.Value;
            TransCAD.PropagationType propagation = TransCAD.PropagationType.Tangency;

            if (cFillet.EdgePropagation == PARTITF.CatFilletEdgePropagation.catMinimalFilletEdgePropagation)
                propagation = TransCAD.PropagationType.Minimal;

            TransCAD.References refElements = PartManager.tPart.CreateReferences();

            for (int i = 1; i <= cFillet.ObjectsToFillet.Count; i++)
            {
                string elementName = cFillet.ObjectsToFillet.Item(i).DisplayName;
                elementName = PartManager.ReferenceManager.GetTransCADNameFromCATIAName(elementName);

                TransCAD.Reference refElement = PartManager.tPart.SelectBrepByName(elementName);

                refElements.Add(refElement);
            }

            PartManager.tFeatures.AddNewSolidFilletConstantFeature(cFillet.get_Name(), refElements, radius, propagation);
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidFilletConstantFeature tFillet = (TransCAD.IStdSolidFilletConstantFeature)tFeature;

            double radius = tFillet.Radius;
            TransCAD.PropagationType propagation = tFillet.Propagation;

            PARTITF.ConstRadEdgeFillet cFillet = PartManager.cShapeFactory.AddNewEdgeFilletWithConstantRadius(null, PARTITF.CatFilletEdgePropagation.catMinimalFilletEdgePropagation, radius);
            
            IEnumerator edges = tFillet.FilletEdges.GetEnumerator();

            while (edges.MoveNext())
            {
                TransCAD.Reference edge = (TransCAD.Reference)edges.Current;

                string cRefName = PartManager.ReferenceManager.GetCATIANameFromTransCADName(edge.ReferenceeName);
                INFITF.Reference cRef = PartManager.cPart.CreateReferenceFromBRepName(cRefName, PartManager.GetRecentFeature());

                cFillet.AddObjectToFillet(cRef);
            }

            if (propagation == TransCAD.PropagationType.Tangency)
                cFillet.EdgePropagation = PARTITF.CatFilletEdgePropagation.catTangencyFilletEdgePropagation;

            PartManager.cPart.UpdateObject(cFillet);
            PartManager.ReferenceManager.NameMap.Add(tFillet.Name, cFillet.get_Name());
        }
    }
}
