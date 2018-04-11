#include "stdafx.h"
#include "FeatureSOLIDCreateHole.h"
#include "ReferenceEntity.h"
namespace Pre {
	FeatureSOLIDCreateHole::FeatureSOLIDCreateHole(Part * pPart, int type, string name) //hole의 construct, ??
		: Feature(pPart, type, name)
	{
		hole_type = 0;			// default=linear hole
		end_condition = 0;		// default=blind
		flip = false;
		depth_default = true;
		bottom_angle = 180.0;
	}

	FeatureSOLIDCreateHole::~FeatureSOLIDCreateHole()
	{
	}

	void FeatureSOLIDCreateHole::GetInfo(ifstream &is) // hole class에 정의된 Read 함수를 읽어라. Read함수에서 ifstream이라는 변수를 &is타입으로 가진다.
	{
		char buffer[400];			//char타입인 buffer는 400개의 배열을 가진다.
		char * context = NULL;
		is.getline(buffer, 400);

		while (strncmp(buffer, "Set", 3))  // Set hole1을 찾는 부분
		{
			is.getline(buffer, 400);
		}

		string token;
		char seps_temp[] = " ,\t\n()="; //구분자
		token = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		token = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		token = strtok_s(NULL, seps_temp, &context);  //세번째 단어

		token = strtok_s(NULL, seps_temp, &context);
		center[0] = atof(token.c_str());
		token = strtok_s(NULL, seps_temp, &context);
		center[1] = atof(token.c_str());
		token = strtok_s(NULL, seps_temp, &context);
		center[2] = atof(token.c_str());

		token = strtok_s(NULL, seps_temp, &context);
		_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(token));

		token = strtok_s(NULL, seps_temp, &context);
		depth = atof(token.c_str());

		radius = atof(token.c_str()) / 2.;

		context = NULL;
	}

	void FeatureSOLIDCreateHole::ToTransCAD()
	{
		TransCAD::IReferencePtr spHoleReference;

		vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();

		while (iter != _referenceEntityList.end())
		{
			(*iter)->ToTransCAD();
			if ((*iter)->GetReferencePtr() != NULL) {
				spHoleReference = (*iter)->GetReferencePtr();
			}
			iter++;
		}

		if (hole_type == 0)		// Hole Simple
			GetPart()->_spFeatures->AddNewSolidHoleSimpleFeature(_name.c_str(), spHoleReference, center[0], center[1], center[2], radius, depth);
		else if (hole_type == 1)		// Hole Counterbored
			GetPart()->_spFeatures->AddNewSolidHoleCounterboredFeature(_name.c_str(), spHoleReference, center[0], center[1], center[2], head_diameter / 2, head_depth, lower_hole_diameter / 2, depth, bottom_angle);
		else if (hole_type == 2) // Hole Countersunk
			GetPart()->_spFeatures->AddNewSolidHoleCountersunkFeature(_name.c_str(), spHoleReference, center[0], center[1], center[2], radius, depth, countersink_radius, head_angle, bottom_angle);
	}

	void FeatureSOLIDCreateHole::CheckAttribute(string name, double value, int type)
	{
		if (name == "BottomLimit")
		{
			// Do nothing
		}
		else if (name == "Diameter")
		{
			radius = value / 2;
		}
		else if (name == "HeadAngle")
		{
			head_angle = value;
		}
		else if (name == "HeadDepth")
		{
			head_depth = value;
		}
		else if (name == "HeadDiameter")
		{
			head_diameter = value;
		}
		else if (name == "BottomAngle")
		{
			bottom_angle = value;
		}

		if (hole_type == 1)
		{
			ModifyCounterbore();
		}
		else if (hole_type == 2)
		{
			ModifyCountersunk();
		}
	}

	void FeatureSOLIDCreateHole::Modify(char* buffer)
	{
		string token;
		char * context = NULL;
		char seps[] = " ,\t\n().="; //구분자

		token = strtok_s(buffer, seps, &context);  //hole1
		token = strtok_s(NULL, seps, &context); //두번째 구분자

		if (token == "ThreadingMode")
		{
			// do nothing
		}

		if (token == "ThreadSide")
		{
			// do nothing
		}

		if (token == "Type")
		{
			token = strtok_s(NULL, seps, &context); //세번째 구분자

			if (token == "catSimpleHole" || token == "0")
			{
				hole_type = 0; // Linear hole. This cannot be reached
			}
			else if (token == "catCounterboredHole" || token == "2")
			{
				ModifyCounterbore();
				hole_type = 1;
			}
			else if (token == "catCountersunkHole" || token == "3")
			{
				ModifyCountersunk();
				hole_type = 2;
			}
		}
	}

	void FeatureSOLIDCreateHole::ModifyCounterbore()
	{
		lower_hole_depth = depth;
		lower_hole_diameter = radius * 2;
		upper_hole_depth = bottom_limit_dim - depth;
		upper_hole_diameter = head_diameter;
	}

	void FeatureSOLIDCreateHole::ModifyCountersunk()
	{
		countersink_angle = head_angle*PI / 180;

		double theta = 0.5 * countersink_angle;
		double y = radius / tan(theta);
		double x = (head_depth + y) * tan(theta);
		countersink_radius = x;
	}

}

