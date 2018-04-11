#pragma once
#include "AttributeItem.h"
namespace Pre {
	class AttLength : public AttributeItem
	{
	public:
		AttLength(Part * pPart, int type, string name);
		virtual ~AttLength();

		virtual void GetInfo(ifstream &is);
		virtual void Modify(char * buffer);
	};
}