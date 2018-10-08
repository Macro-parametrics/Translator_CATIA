#pragma once
#include "Feature.h"
namespace Post {
	class FSketch;

	class FeatureSOLIDCreateProtrusionExtrude : public Feature
	{
	public:
		FeatureSOLIDCreateProtrusionExtrude(Part * pPart, TransCAD::IFeaturePtr spFeature);
		virtual ~FeatureSOLIDCreateProtrusionExtrude(void);

		FSketch * GetFeatureProfileSketch() { return _featureProfileSketch; }

		virtual void GetInfo();
		virtual void ToCATIA();

		double GetStartDepth() { return _startDepth; }
		double GetEndDepth() { return _endDepth; }

	private:
		bool	_flip;
		double	_startDepth;
		double	_endDepth;
		TransCAD::StdExtrudeEndType	_startCondition;
		TransCAD::StdExtrudeEndType	_endCondition;

	protected:
		FSketch * _featureProfileSketch;
	};
}