#pragma once
#include "Feature.h"
namespace Post {
	class FSketch;

	class FeatureSOLIDCreateSectionsLoft : public Feature
	{
	public:
		FeatureSOLIDCreateSectionsLoft(Part * pPart, TransCAD::IFeaturePtr spFeature);
		virtual ~FeatureSOLIDCreateSectionsLoft(void);

		//	FSketch * GetProfileSketch() {return _profileSketch;}
		//	FSketch * GetGuideSketch() {return _guideSketch;}

		virtual void GetInfo();
		virtual void ToCATIA();

	protected:
		int _sectionCount;
		int hybridShapeExtremumNum;
		string _sectionname[5];

	public:
		//	bool	_flip;
		//	double	_startDepth;
		//	double	_endDepth;

	private:
		//	FSketch * _profileSketch;
		//	FSketch * _guideSketch;
		FSketch * _section[6];
	};
}