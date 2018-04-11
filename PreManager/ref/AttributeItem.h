#pragma once
#include "Part.h"
#include "Feature.h"
namespace Pre {
	class Part;

	class AttributeItem
	{
	public:
		AttributeItem(Part * pPart, int type, string name);
		virtual ~AttributeItem();

		Part *	GetPart() { return _pPart; }
		int GetId() { return _id; }
		int GetType() { return _type; }
		string GetName() { return _name; }

		virtual void GetInfo(ifstream & is);
		virtual void Modify(char * buffer);
		virtual void CheckAttribute(string name, double value, int type);

	protected:
		Part * _pPart;
		string _name;
		string _featureName;
		string _attributeName;
		double _value;
		int _id;
		int _type;  //0 : none 1: length 2: angle 3: 
	};
}