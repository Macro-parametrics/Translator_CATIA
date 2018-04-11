#pragma once
#include "Feature.h"
namespace Pre {
	class FSketch;
	class ReferenceEntity;

	class FeatureSOLIDCreatePatternCircular : public Feature
	{
	public:
		FeatureSOLIDCreatePatternCircular(Part * pPart, int type, string name);
		virtual ~FeatureSOLIDCreatePatternCircular();

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void CheckAttribute(string name, double value, int type);
		virtual void Modify(char * buffer);

		virtual void SetDirection(string refplane);
		void CreateAxis(FSketch * pRefSketch, FSketch * pRevSketch, Point3D origin, Point3D direction);
		void TransformAxis(double refSketCSYS[9], double revSketCSYS[9], Point3D &coords);
		void TransformCoordinates(double sourceCSYS[3][3], double targetCSYS[3][3], Point3D &coords);

	private:
		bstr_t _spTargetFeature;
		double CircleSpacing;
		double OrgX;
		double OrgY;
		double OrgZ;
		int CircleNumber;

		double AngularSpacing;
		double DirX;
		double DirY;
		double DirZ;
		int AngularNumber;

		string _RefPlane;
		string _rotationAxis;

		vector<ReferenceEntity *> _referenceEntityList;
	};
}