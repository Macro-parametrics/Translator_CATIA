#pragma once
#include "Feature.h"
namespace Pre {
	class ReferenceEntity;

	class FeatureSOLIDCreatePatternRectangular : public Feature
	{
	public:
		FeatureSOLIDCreatePatternRectangular(Part * pPart, int type, string name);
		virtual ~FeatureSOLIDCreatePatternRectangular();

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void CheckAttribute(string name, double value, int type);
		virtual void Modify(char * buffer);

		virtual void SetDirection(double colspacing, double rowspacing, string refplane);

	private:
		bstr_t _spTargetFeature;
		double ColumnSpacing;
		double ColDirX;
		double ColDirY;
		double ColDirZ;
		int ColumnNumber;

		double RowSpacing;
		double RowDirX;
		double RowDirY;
		double RowDirZ;
		int RowNumber;

		string _RefPlane;
		string _firstDirection, _secondDirection;

		vector<ReferenceEntity *> _referenceEntityList;
	};
}