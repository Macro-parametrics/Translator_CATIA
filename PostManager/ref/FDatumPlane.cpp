#include "stdafx.h"
#include "FDatumPlane.h"


FDatumPlane::FDatumPlane(Part * pPart, Point3D& origin, Direct3D& normal) : Feature(pPart, NULL)
{
	_origin = origin;
	_normal = normal;
}

FDatumPlane::FDatumPlane(Part * pPart, TransCAD::IFeaturePtr spFeature) :
	Feature(pPart, spFeature)
{
}

FDatumPlane::~FDatumPlane(void)
{
}

void FDatumPlane::GetInfo()
{
	string featureName = GetFeatureName();

	_origin = Point3D(0,0,0);

	if (featureName==string("XYPlane"))		
		_normal = Direct3D(0,0,1);
	else if (featureName==string("YZPlane")) 
		_normal = Direct3D(1,0,0);
	else if (featureName==string("ZXPlane"))
		_normal = Direct3D(0,1,0);
}

void FDatumPlane::ToCATIA()
{
	cout << "  FixedDatumPlane " << GetFeatureName() << endl;	
}
