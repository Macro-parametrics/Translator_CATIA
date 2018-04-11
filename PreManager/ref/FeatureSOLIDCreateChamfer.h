#pragma once
#include "Feature.h"
namespace Pre {
	class ReferenceEntity;

	class FeatureSOLIDCreateChamfer : public Feature
	{

	public:
		FeatureSOLIDCreateChamfer(Part * pPart, int type, string name);
		virtual ~FeatureSOLIDCreateChamfer(void);

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void CheckAttribute(string name, double value, int type);
		virtual void Modify(char * buffer);

	private:
		int _propagation;
		int _mode;
		int _orientation;
		double  _length1;
		double  _length2;
		vector<ReferenceEntity *> _referenceEntityList;
	};

}