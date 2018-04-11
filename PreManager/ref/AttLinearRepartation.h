#pragma once
#include "AttributeItem.h"
namespace Pre {
	class AttLinearRepartation : public AttributeItem
	{
	public:
		AttLinearRepartation(Part * pPart, int type, string name);
		virtual ~AttLinearRepartation();

		virtual void GetInfo(ifstream &is);
		virtual void Modify(char * buffer);
		virtual void CheckAttribute(string name, double value, int type);
	};
}