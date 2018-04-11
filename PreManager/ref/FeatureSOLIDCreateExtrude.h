#pragma once
#include "Feature.h"
namespace Pre {
	class FSketch;
	class ReferenceEntity;

	class FeatureSOLIDCreateExtrude : public Feature
	{

	public:
		FeatureSOLIDCreateExtrude(Part * pPart, int type, string name);
		virtual ~FeatureSOLIDCreateExtrude(void);

		FSketch * GetFeatureProfileSketch() { return _featureProfileSketch; }

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		double GetStartDepth() { return _startDepth; }
		double GetEndDepth() { return _endDepth; }

		virtual void Modify(char * buffer);
		virtual void CheckAttribute(string name, double value, int type);

	private:
		VARIANT_BOOL _flip;
		double	_startDepth;
		double	_endDepth;
		TransCAD::StdExtrudeEndType _startCondition, _endCondition;

	protected:
		FSketch * _featureProfileSketch;
		vector<ReferenceEntity *> _referenceEntityList;
	};
}