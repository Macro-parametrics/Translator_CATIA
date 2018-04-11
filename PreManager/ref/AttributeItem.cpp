#include "stdafx.h"
#include "AttributeItem.h"

namespace Pre {
	AttributeItem::AttributeItem(Part * pPart, int type, string name)
	{
		_pPart = pPart;
		_type = type;
		_name = name;
	}

	AttributeItem::~AttributeItem()
	{
	}

	void AttributeItem::GetInfo(ifstream & is)
	{
	}

	void AttributeItem::CheckAttribute(string name, double value, int type)
	{
	}

	void AttributeItem::Modify(char * buffer)
	{
	}
}