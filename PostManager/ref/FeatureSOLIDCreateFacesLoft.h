#pragma once
#include "Feature.h"
namespace Post {
	class FSketch;

	class FeatureSOLIDCreateFacesLoft : public Feature
	{
	public:
		FeatureSOLIDCreateFacesLoft(Part * pPart, TransCAD::IFeaturePtr spFeature);
		virtual ~FeatureSOLIDCreateFacesLoft(void);

		FSketch * GetProfileSketch() { return _profileSketch; }
		FSketch * GetGuideSketch() { return _guideSketch; }

		virtual void GetInfo();
		virtual void ToCATIA();

	public:
		//	bool	_flip;
		//	double	_startDepth;
		//	double	_endDepth;

	private:
		FSketch * _profileSketch;
		FSketch * _guideSketch;
	};
}