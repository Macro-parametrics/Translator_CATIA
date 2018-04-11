#pragma once
#include "Feature.h"
namespace Pre {
	class FSketch;
	class ReferenceEntity;

	class FeatureSOLIDCreateRevolve : public Feature
	{
	public:
		FeatureSOLIDCreateRevolve(Part * pPart, int type, string name);
		virtual ~FeatureSOLIDCreateRevolve();

		FSketch * GetFeatureProfileSketch() { return _featureProfileSketch; }

		virtual void GetInfo(ifstream &is);
		virtual void ToTransCAD();

		virtual void CheckAttribute(string name, double value, int type);
		virtual void Modify(char * buffer);

		Point3D GetAxisOrigin() { return origin; }
		Point3D GetAxisDirection() { return direction; }

	protected:
		void AddNewRevolveAxis();
		void CreateRevolveAxis(FSketch * pRefSketch, FSketch * pRevSketch, Point3D origin, Point3D direction);
		void TransformRevolveAxis(double refSketCSYS[9], double revSketCSYS[9], Point3D &coords);
		void TransformCoordinates(double sourceCSYS[3][3], double targetCSYS[3][3], Point3D &coords);

	private:
		double startAngle;
		double endAngle;
		string startCondition;
		string endCondition;
		string _revolveAxis;
		Point3D origin, direction;
		int flip;

		FSketch * _featureProfileSketch;
		vector<ReferenceEntity *> _referenceEntityList;
	};

}