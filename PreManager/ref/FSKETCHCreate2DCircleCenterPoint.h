#pragma once
#include "FSKETCHItem.h"
namespace Pre {
	class FSKETCHCreate2DCircleCenterPoint : public FSKETCHItem
	{
	public:
		FSKETCHCreate2DCircleCenterPoint(FSketch * pFSketch, int type, string name);
		virtual ~FSKETCHCreate2DCircleCenterPoint(void);

		void GetCenterPoint(Point3D & cP) { cP.SetXYZ(center[0], center[1], center[2]); }
		double GetRadius() const { return _radius; }
		void SetStartPoint(Point3D sP) { startPoint[0] = sP.X(); startPoint[1] = sP.Y(); startPoint[2] = sP.Z(); }
		void SetEndPoint(Point3D eP) { endPoint[0] = eP.X(); endPoint[1] = eP.Y(); endPoint[2] = eP.Z(); }
		Point3D GetStartPoint() { return startPoint; }
		Point3D GetEndPoint() { return endPoint; }
		string GetStartPointName() { return _startPoint; }
		string GetEndPointName() { return _endPoint; }

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void Modify(char * buffer);
		virtual void ConvertRadToDeg(double x, double y);

	protected:
		double center[3];
		double _radius;
		double startAngle, _startAngle;
		double endAngle, _endAngle;
		double startPoint[3], endPoint[3];
		int circleType;	// 0: none  1: closedCircle 2: nonClosedCircle, Arc

		string _center;
		string _startPoint;
		string _endPoint;
	};
}