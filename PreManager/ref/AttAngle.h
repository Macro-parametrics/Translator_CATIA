#pragma once
#include "AttributeItem.h"

namespace Pre {

	class AttAngle : public AttributeItem
	{
	public:
		AttAngle(Part * pPart, int type, string name);
		virtual ~AttAngle();

		virtual void GetInfo(ifstream &is);
		virtual void Modify(char * buffer);
	};

}