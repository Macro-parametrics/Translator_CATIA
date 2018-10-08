#pragma once
#include "Feature.h"
namespace Post {
	class FSketch;

	class FeatureSOLIDCreateCutRevolve : public Feature
	{
	public:
		FeatureSOLIDCreateCutRevolve(Part * pPart, TransCAD::IFeaturePtr spFeature);
		virtual ~FeatureSOLIDCreateCutRevolve(void);

		FSketch * GetFeatureProfileSketch() { return _featureProfileSketch; }

		virtual void GetInfo();
		virtual void ToCATIA();

	private:
		bool	_flip;
		double	_startAngle;
		double	_endAngle;

	protected:
		FSketch * _featureProfileSketch;
	};
}