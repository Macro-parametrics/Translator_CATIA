#pragma once
#include "Feature.h"
namespace Pre {
	class FeatureSOLIDCreateSweep : public Feature
	{
	public:
		FeatureSOLIDCreateSweep(Part * pPart, int type, string name);
		virtual ~FeatureSOLIDCreateSweep();

		virtual void GetInfo(ifstream &is);
		virtual void ToTransCAD();

		virtual void CheckAttribute(string name, double value, int type);
		virtual void Modify(char * buffer);

	private:
		string sketch1;
		string sketch2;
	};

}