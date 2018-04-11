#pragma once
#include "Feature.h"

class FSketch;

class FeatureSOLIDCreateProtrusionSweep : public Feature
{
public:
	FeatureSOLIDCreateProtrusionSweep(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FeatureSOLIDCreateProtrusionSweep(void);

	FSketch * GetProfileSketch() {return _profileSketch;}
	FSketch * GetGuideSketch() {return _guideSketch;}

	virtual void GetInfo();
	virtual void ToCATIA();

private:
	FSketch * _profileSketch;
	FSketch * _guideSketch;
};
