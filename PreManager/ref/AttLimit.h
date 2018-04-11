#pragma once
#include "AttributeItem.h"
namespace Pre {
	class AttLimit : public AttributeItem
	{
	public:
		AttLimit(Part * pPart, int type, string name);
		virtual ~AttLimit();

		virtual void GetInfo(ifstream &is);
		virtual void Modify(char * buffer);
		virtual void CheckAttribute(string name, double value, int type);

	protected:
		string limitMode;
	};

}