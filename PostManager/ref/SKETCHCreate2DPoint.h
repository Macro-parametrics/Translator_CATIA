#pragma once
#include "SKETCHItem.h"
namespace Post {
	class FSketch;

	class SKETCHCreate2DPoint : public SKETCHItem
	{
	public:
		SKETCHCreate2DPoint(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
		virtual ~SKETCHCreate2DPoint(void);

		void SetPoint(const Point3D & point) { _point = point; }
		void GetPoint(Point3D & point) { point = _point; }
		double X() { return _point.X(); }
		double Y() { return _point.Y(); }

		virtual void GetInfo();
		virtual void ToCATIA();

	public:
		string TransCADName;

	private:
		Point3D _point;
	};
}