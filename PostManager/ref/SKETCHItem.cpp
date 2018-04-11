#include "stdafx.h"
#include "SKETCHItem.h"


SKETCHItem::SKETCHItem(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
{
	_pFSketch = pFSketch;					// Set FSketch Ptr
	_spItem = spItem;								// Set TransCAD SketchItem Ptr;
	_nIndex = pFSketch->GetSketchItemsSize();	// Set index
	_strName = spItem->Name;					// Set Item Name

	cout << "  " << "Geometry Item Name : " << _strName;	//DEBUG
}

SKETCHItem::~SKETCHItem(void)
{
}

void SKETCHItem::GetInfo()
{
}

void SKETCHItem::ToCATIA()
{
}

