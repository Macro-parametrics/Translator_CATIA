#pragma once
#include "SKETCHItem.h"

class FSketch;

class SKETCHCreate2DCenterline : public SKETCHItem
{
public:
	SKETCHCreate2DCenterline(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
	virtual ~SKETCHCreate2DCenterline(void);

	void SetStartPoint(const Point3D & startPoint) {_startPoint = startPoint;}
	void SetEndPoint(const Point3D & endPoint) {_endPoint = endPoint;}

	virtual void GetInfo();
	virtual void ToCATIA();

public:
	string TransCADName;

private:
	Point3D _startPoint;
	Point3D _endPoint;
};
