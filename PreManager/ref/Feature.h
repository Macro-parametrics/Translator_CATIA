#pragma once
#include "Part.h"
namespace Pre {
	class Part;

	class Feature
	{
	public:
		Feature(Part * pPart, int type, string name);
		virtual ~Feature(void);

		Part *	GetPart() { return _pPart; }
		string	GetFeatureName() { return _name; }
		int	GetFeatureType() const { return _type; }

		void SetFeatureName(string featureName) { _name = featureName; }
		void SetFeatureType(int featureType) { _type = featureType; }

		virtual void GetInfo(ifstream & is) {}
		virtual void ToTransCAD() {}

		virtual void CheckAttribute(string name, double value, int type);
		virtual void Modify(char * buffer);

	public:
		Part *	_pPart;
		string	_name;
		int _type;
	};
}