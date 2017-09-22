
namespace CATIATranslator
{
    class Feature
    {
        public Part PartManager = null;

        public virtual void TranslateC2T(MECMOD.Shape cShape)
        {
            // Do nothing
        }

        public virtual void TranslateT2C(TransCAD.IFeature tFeature)
        {
            // Do nothing
        }
    }
}
