#pragma once

#include "Feature.h"

class FDatumPlane : public Feature
{
public:
	FDatumPlane(Part * pPart, Point3D & origin, Direct3D & normal);
	FDatumPlane(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FDatumPlane(void);

	void GetOrigin(Point3D & origin) {origin=_origin;}
	void GetNormal(Direct3D & normal) {normal=_normal;}

	virtual void	GetInfo();
	virtual	void	ToCATIA();

protected:
	Point3D	_origin;
	Direct3D _normal;
};