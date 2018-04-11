#pragma once
#include "Part.h"

class Part;

class Feature
{
public:
	Feature(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~Feature(void);

	Part*	GetPart() {return _pPart;}

	void	SetFeatureName(string featureName) {_strName = featureName;}
	size_t	GetFeatureIndex() {return _nIndex;}
	string	GetFeatureName() {return _strName;}
	
	TransCAD::IFeaturePtr GetTransCADFeature() {return _spFeature;}
	
	virtual void GetInfo() {}
	virtual void ToCATIA() {}

	virtual double GetStartDepth() { return 0; }
	virtual double GetEndDepth() { return 0; }

public:
	Part *	_pPart;
	size_t	_nIndex;
	string	_strName;
	TransCAD::IFeaturePtr _spFeature;
};