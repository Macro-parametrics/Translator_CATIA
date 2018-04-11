#pragma once
#include "Feature.h"
namespace Pre {
	class ReferenceEntity;

	class FeatureSOLIDCreateHole : public Feature
	{
	public:
		FeatureSOLIDCreateHole(Part * pPart, int type, string name);
		virtual ~FeatureSOLIDCreateHole();

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void Modify(char* buffer);
		virtual void CheckAttribute(string name, double value, int type);

		void ModifyCounterbore();
		void ModifyCountersunk();

	private:
		string _selected_face;
		string _center;

		// linear hole
		double center[3];
		double radius;
		double depth;
		int end_condition; // 0: blind, 1: 
		bool flip;

		// counterbored
		double lower_hole_diameter;
		double lower_hole_depth;
		double bottom_angle;
		double head_diameter;

		// countersunk
		double head_angle;
		double head_depth;
		double upper_hole_diameter;
		double upper_hole_depth;
		double countersink_angle;
		double countersink_radius;

		// 0: linear, 1: counterbore, 2: countersunk
		int hole_type;

		// temporary buffer
		double bottom_limit_dim;
		bool depth_default;

		vector<ReferenceEntity *> _referenceEntityList;
	};
}