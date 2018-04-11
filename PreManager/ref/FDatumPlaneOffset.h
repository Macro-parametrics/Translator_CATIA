#pragma once
#include "Feature.h"
namespace Pre {
	class ReferenceEntity;

	class FDatumPlaneOffset : public Feature
	{
	public:
		FDatumPlaneOffset(Part * pPart, int type, string name);
		virtual ~FDatumPlaneOffset();

		virtual void GetInfo(ifstream &is);
		virtual void ToTransCAD();

		virtual void Modify(char * buffer);
		virtual void CheckAttribute(string name, double value, int type);

	protected:
		double _distance;
		int _flip;
		vector<ReferenceEntity *> _referenceEntityList;
	};
}