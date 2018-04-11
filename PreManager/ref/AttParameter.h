#pragma once
#include "AttributeItem.h"
namespace Pre {
	class AttParameter : public AttributeItem
	{
	public:
		AttParameter(Part * pPart, int type, string name);
		virtual ~AttParameter();

		virtual void GetInfo(ifstream &is);
		virtual void Modify(char * buffer);
		virtual void CheckAttribute(string name, double value, int type);

		std::string GetFeatureTypeInLowerString(std::string _featureType);
	};

}