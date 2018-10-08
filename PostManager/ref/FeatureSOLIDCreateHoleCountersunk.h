#pragma once
#include "Feature.h"
namespace Post {
	class FeatureSOLIDCreateHoleCountersunk : public Feature
	{
	public:
		FeatureSOLIDCreateHoleCountersunk(Part * pPart, TransCAD::IFeaturePtr spFeature);
		virtual ~FeatureSOLIDCreateHoleCountersunk(void);

		virtual void GetInfo();
		virtual void ToCATIA();

	protected:
		double _Radius;
		double _Depth;
		double _sinkRadius;
		double _sinkAngle;
		double _headDepth;
		double _centerpoint[3];
		double _bottomAngle;
		string _featurename;
		string _referenceName;
	};
}