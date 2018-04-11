#pragma once
#include "Feature.h"

class FeatureSOLIDCreatePatternCircular : public Feature
{
public:
	FeatureSOLIDCreatePatternCircular(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FeatureSOLIDCreatePatternCircular(void);

	virtual void GetInfo();
	virtual void ToCATIA();

protected:
	string _featurename;
	char _featurenameCATIA[1024];

	char _angleincrement[MAX_CHAR_LIMIT];
	char  _patternnumber[MAX_CHAR_LIMIT];
	double _rowspacing;
	bool _isradial;
	double _ptorg[3], _ptvec[3];
	
	char _refPlane[1024];
	void GetCircReferencePlane();
};
