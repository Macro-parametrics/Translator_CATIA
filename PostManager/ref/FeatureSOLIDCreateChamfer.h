#pragma once
#include "Feature.h"

class FeatureSOLIDCreateChamfer : public Feature
{
public:
	FeatureSOLIDCreateChamfer(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FeatureSOLIDCreateChamfer(void);

	virtual void GetInfo();
	virtual void ToCATIA();

protected:
	double	_length;
	int _edgeCount;
	vector<string> _edgeNames;
};
