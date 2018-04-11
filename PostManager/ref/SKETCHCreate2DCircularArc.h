#pragma once
#include "SKETCHItem.h"

class FSketch;

class SKETCHCreate2DCircularArc : public SKETCHItem
{
public:
	SKETCHCreate2DCircularArc(FSketch* pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
	virtual ~SKETCHCreate2DCircularArc(void);

	void SetCenterPoint(const Point3D & centerPoint) {_centerPoint = centerPoint;}
	void SetStartPoint(const Point3D & startPoint) {_startPoint = startPoint;}
	void SetEndPoint(const Point3D & endPoint) {_endPoint = endPoint;}

	Point3D GetCenterPoint() {return _centerPoint;}
	Point3D GetStartPoint() {return _startPoint;}
	Point3D GetEndPoint() {return _endPoint;}

	virtual void GetInfo();
	virtual void ToCATIA();
public:
	string TransCADName;

private:
	Point3D _centerPoint;
	Point3D _startPoint;
	Point3D _endPoint;
	double _startAngle;
	double _endAngle;
	double _radius;
};
