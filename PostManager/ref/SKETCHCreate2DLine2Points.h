#pragma once
#include "SKETCHItem.h"

class FSketch;

class SKETCHCreate2DLine2Points : public SKETCHItem
{
public:
	SKETCHCreate2DLine2Points(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
	virtual ~SKETCHCreate2DLine2Points(void);

	void SetStartPoint(const Point3D & startPoint) {_startPoint = startPoint;}
	void SetEndPoint(const Point3D & endPoint) {_endPoint = endPoint;}

	void GetStartPoint(Point3D & startPoint) {startPoint = _startPoint;}
	void GetEndPoint(Point3D & endPoint) {endPoint = _endPoint;}
	void GetPickingPoint(Point3D & pickPnt) {pickPnt = (_startPoint+_endPoint)*0.5;}

	virtual void GetInfo();
	virtual void ToCATIA();
	
public:
	char startPointName[1024], endPointName[1024];
	string TransCADName;

private:
	Point3D _startPoint;
	Point3D _endPoint;
};
