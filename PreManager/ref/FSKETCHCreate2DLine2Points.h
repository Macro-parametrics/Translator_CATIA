#pragma once
#include "FSKETCHItem.h"
namespace Pre {
	class FSKETCHCreate2DLine2Points : public FSKETCHItem
	{
	public:
		FSKETCHCreate2DLine2Points(FSketch * pFSketch, int type, string name);
		virtual ~FSKETCHCreate2DLine2Points(void);

		Point3D GetStartPoint() { return startPoint; }
		Point3D GetEndPoint() { return endPoint; }
		void SetStartPoint(Point3D sP) { startPoint[0] = sP.X(); startPoint[1] = sP.Y(); startPoint[2] = sP.Z(); }
		void SetEndPoint(Point3D eP) { endPoint[0] = eP.X(); endPoint[1] = eP.Y(); endPoint[2] = eP.Z(); }
		string GetStartPointName() { return _startPoint; }
		string GetEndPointName() { return _endPoint; }

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void Modify(char * buffer);
		bool IsLocalAxis() { return _isLocalAxis; }

	protected:
		double startPoint[3];
		double endPoint[3];
		string _startPoint;
		string _endPoint;
		int centerLineNum;
		bool _isLocalAxis;
	};
}