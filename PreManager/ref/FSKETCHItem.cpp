#include "stdafx.h"
#include "FSKETCHItem.h"

namespace Pre {
	FSKETCHItem::FSKETCHItem(FSketch * pFSketch, int type, string name)
	{
		_pFSketch = pFSketch;				// Set FSketch Ptr
		_name = name;						// Set Item Name
		_type = type;						// Set Item Type
		_centerLineInfo = 0;				// 일반 Line인 경우 0, centerLine인 경우 1. 나중에 데이터 입력
	}

	FSKETCHItem::~FSKETCHItem(void)
	{
	}

	void FSKETCHItem::GetInfo(ifstream & is)
	{
	}

	void FSKETCHItem::ToTransCAD()
	{
	}

	void FSKETCHItem::Modify(char * buffer)
	{
	}
}