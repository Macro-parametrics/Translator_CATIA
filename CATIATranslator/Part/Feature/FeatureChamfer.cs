using System.Collections;

namespace CATIATranslator
{
    class FeatureChamfer : Feature
    {
        public FeatureChamfer(Part PartManager)
        {
            this.PartManager = PartManager;
        }
        
        // Pre
        public override void TranslateC2T(MECMOD.Shape cShape)
        {
            PARTITF.Chamfer cChamfer = (PARTITF.Chamfer)cShape;

            double length = cChamfer.Length1.Value;

            TransCAD.References refElements = PartManager.tPart.CreateReferences();

            for (int i = 1; i <= cChamfer.ElementsToChamfer.Count; i++)
            {
                string elementName = cChamfer.ElementsToChamfer.Item(i).DisplayName;
                elementName = PartManager.ReferenceManager.GetTransCADNameFromCATIAName(elementName);

                TransCAD.Reference refElement = PartManager.tPart.SelectBrepByName(elementName);
                refElements.Add(refElement);
            }

            PartManager.tFeatures.AddNewSolidChamferFeature(cChamfer.get_Name(), refElements, length);
        }

        // Post
        public override void TranslateT2C(TransCAD.IFeature tFeature)
        {
            TransCAD.IStdSolidChamferFeature tChamfer = (TransCAD.IStdSolidChamferFeature)tFeature;

            double length = tChamfer.Length;
            TransCAD.ChamferType chamferType = tChamfer.ChamferType;

            PARTITF.Chamfer cChamfer = PartManager.cShapeFactory.AddNewChamfer(null, PARTITF.CatChamferPropagation.catMinimalChamfer, PARTITF.CatChamferMode.catTwoLengthChamfer, PARTITF.CatChamferOrientation.catNoReverseChamfer, length, length);

            IEnumerator edges = tChamfer.SelectedEdges.GetEnumerator();

            while (edges.MoveNext())
            {
                TransCAD.Reference edge = (TransCAD.Reference)edges.Current;

                string cRefName = PartManager.ReferenceManager.GetCATIANameFromTransCADName(edge.ReferenceeName);
                INFITF.Reference cRef = PartManager.cPart.CreateReferenceFromBRepName(cRefName, PartManager.GetRecentFeature());

                cChamfer.AddElementToChamfer(cRef);
            }

            PartManager.cPart.UpdateObject(cChamfer);
            PartManager.ReferenceManager.NameMap.Add(tChamfer.Name, cChamfer.get_Name());
        }
    }
}
