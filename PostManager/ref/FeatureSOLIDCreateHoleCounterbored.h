#pragma once
#include "Feature.h"

class FeatureSOLIDCreateHoleCounterbored : public Feature
{
public:
	FeatureSOLIDCreateHoleCounterbored(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FeatureSOLIDCreateHoleCounterbored(void);

	virtual void GetInfo();
	virtual void ToCATIA();

protected:
	double _startRadius; 
	double _endRadius;
	double _startDepth; 
	double _endDepth; 
	double _bottomAngle;
	double _centerpoint[3];
	string _featurename;
	string _referenceName;
};
