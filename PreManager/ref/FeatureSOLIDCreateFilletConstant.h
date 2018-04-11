#pragma once
#include "Feature.h"
namespace Pre {
	class ReferenceEntity;

	class FeatureSOLIDCreateFilletConstant : public Feature
	{
	public:
		FeatureSOLIDCreateFilletConstant(Part * pPart, int type, string name);
		virtual ~FeatureSOLIDCreateFilletConstant(void);

		double GetRadius() const { return _radius; }
		TransCAD::PropagationType GetPropagationType() { return _propagationType; }

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void CheckAttribute(string name, double value, int type);
		virtual void Modify(char * buffer);

	private:
		double	_radius;					// Fillet Radius
		TransCAD::PropagationType _propagationType;	// Propagation Type
		int _propagation;
		vector<ReferenceEntity *> _referenceEntityList;
	};

}