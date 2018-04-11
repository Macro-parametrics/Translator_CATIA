#pragma once
#include "Feature.h"

class FDatumPlaneOffset : public Feature
{
public:
	FDatumPlaneOffset(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FDatumPlaneOffset(void);
	
	virtual void GetInfo();
	virtual	void ToCATIA();

	bool IsBrepReference() {return _isBrepReference;}

protected:
	double _distance;
	string _referenceName;
	TransCAD::ReferenceType _referenceType;
	bool _isFlip;
	bool _isBrepReference;
};