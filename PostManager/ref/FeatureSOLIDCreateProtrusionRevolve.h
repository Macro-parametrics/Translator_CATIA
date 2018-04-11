#pragma once
#include "Feature.h"

class FSketch;

class FeatureSOLIDCreateProtrusionRevolve : public Feature
{
public:
	FeatureSOLIDCreateProtrusionRevolve(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FeatureSOLIDCreateProtrusionRevolve(void);

	FSketch * GetFeatureProfileSketch() {return _featureProfileSketch;}

	virtual void GetInfo();
	virtual void ToCATIA();

private:
	bool	_flip;
	double	_startAngle;
	double	_endAngle;
	
protected:
	FSketch * _featureProfileSketch;
};
