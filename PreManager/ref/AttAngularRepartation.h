#pragma once
#include "AttributeItem.h"
namespace Pre {
	class AttAngularRepartation : public AttributeItem
	{
	public:
		AttAngularRepartation(Part * pPart, int type, string name);
		virtual ~AttAngularRepartation();

		virtual void GetInfo(ifstream &is);
		virtual void Modify(char * buffer);
		virtual void CheckAttribute(string name, double value, int type);
	};
}