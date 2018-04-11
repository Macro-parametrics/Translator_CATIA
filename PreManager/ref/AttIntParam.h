#pragma once
#include "AttributeItem.h"
namespace Pre {
	class AttIntParam : public AttributeItem
	{
	public:
		AttIntParam(Part * pPart, int type, string name);
		virtual ~AttIntParam();

		virtual void GetInfo(ifstream &is);
		virtual void Modify(char * buffer);
	};
}