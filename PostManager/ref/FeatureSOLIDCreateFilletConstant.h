 #pragma once
#include "Feature.h"

class FeatureSOLIDCreateFilletConstant : public Feature
{
public:
	FeatureSOLIDCreateFilletConstant(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FeatureSOLIDCreateFilletConstant(void);

	virtual void GetInfo();
	virtual void ToCATIA();

protected:
	double	_radius;					// Fillet Radius
	TransCAD::PropagationType _propagationType;	// Propagation Type
	int _edgeCount;
	vector<string> _edgeNames;
};

