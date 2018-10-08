#pragma once
#include "Feature.h"
namespace Post {
	class FeatureSOLIDCreatePatternRectangular : public Feature
	{
	public:
		FeatureSOLIDCreatePatternRectangular(Part * pPart, TransCAD::IFeaturePtr spFeature);
		virtual ~FeatureSOLIDCreatePatternRectangular(void);

		virtual void GetInfo();
		virtual void ToCATIA();
		void GetRectReferencePlane();

	protected:
		string _featurename;
		char _featurenameCATIA[1024];

		double _colspacing;
		double _rowspacing;
		int _colnumber;
		int _rownumber;
		double _coldir[3];
		double _rowdir[3];

		double coldir, rowdir;
		int _refdir[2];
		char _refPlane[1024];
	};
}