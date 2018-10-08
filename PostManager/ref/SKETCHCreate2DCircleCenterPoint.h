#pragma once
#include "SKETCHItem.h"
namespace Post {
	class FSketch;

	class SKETCHCreate2DCircleCenterPoint :
		public SKETCHItem
	{
	public:
		SKETCHCreate2DCircleCenterPoint(FSketch* pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
		virtual ~SKETCHCreate2DCircleCenterPoint(void);

		void SetCenterPoint(const Point3D & centerPoint) { _centerPoint = centerPoint; }
		void SetRadius(const double rad) { _rad = rad; }

		Point3D GetCenterPoint() { return _centerPoint; }
		double GetRadius() { return _rad; }

		virtual void GetInfo();
		virtual void ToCATIA();

	public:
		string TransCADName;

	private:
		Point3D _centerPoint;
		double _rad;
	};
}