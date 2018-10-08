#pragma once
#include "FSketch.h"
namespace Post {
	class FSketch;

	class SKETCHItem
	{
	public:
		SKETCHItem(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
		~SKETCHItem(void);

		FSketch * GetFeatureSketch() { return _pFSketch; }
		string	GetSketchItemName() { return _strName; }
		TransCAD::IStdSketchGeometryPtr GetTransCADGeometryItem() { return _spItem; }
		TransCAD::StdSketchGeometryType GetType() { return _spItem->Type; }

		virtual void GetInfo();
		virtual void ToCATIA();

	protected:
		FSketch * _pFSketch;
		string		_strName;
		TransCAD::IStdSketchGeometryPtr _spItem;
		size_t		_nIndex;
	};
}