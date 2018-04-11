#pragma once
#include "Feature.h"

class FSketch;

class FeatureSOLIDCreateProtrusionLoft : public Feature
{
public:
	FeatureSOLIDCreateProtrusionLoft(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FeatureSOLIDCreateProtrusionLoft(void);

	FSketch * GetProfileSketch() {return _profileSketch;}
	FSketch * GetGuideSketch() {return _guideSketch;}

	virtual void GetInfo();
	virtual void ToCATIA();

public:
	bool	_flip;
	double	_startDepth;
	double	_endDepth;

private:
	FSketch * _profileSketch;
	FSketch * _guideSketch;
};
