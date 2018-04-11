#pragma once
#include "FSKETCHItem.h"
namespace Pre {
	class FSKETCHCreate2DPoint : public FSKETCHItem
	{
	public:
		FSKETCHCreate2DPoint(FSketch * pFSketch, int type, string name);
		virtual ~FSKETCHCreate2DPoint(void);

		void GetPoint(Point3D & point) { point = _point; }

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void Modify(char * buffer);

	private:
		double _point[3];
	};

}