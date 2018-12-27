#include "stdafx.h"
#include "AttLimit.h"

namespace Pre {
	AttLimit::AttLimit(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttLimit::~AttLimit()
	{
	}

	void AttLimit::GetInfo(ifstream &is)
	{
		// Limit의 정보 입력
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)
		while (strncmp(buffer, "Set", 3))  // Set limit2을 찾는 부분
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string limitValue;
		char seps_temp[] = " ,\t\n()=."; //구분자
		char * context = NULL;

		limitValue = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		limitValue = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		limitValue = strtok_s(NULL, seps_temp, &context);  //세번째 단어 : pocket1
		_featureName = limitValue;

		limitValue = strtok_s(NULL, seps_temp, &context);  //네번째 단어 : FirstLimit
		_attributeName = limitValue;

		context = NULL;
	}

	void AttLimit::Modify(char * buffer)
	{
		string token;
		char seps[] = " ,\t\n().="; //구분자
		char * context = NULL;

		token = strtok_s(buffer, seps, &context);  //limit2
		token = strtok_s(NULL, seps, &context); //두번째 구분자  LimitMode

		if (token == "LimitMode")
		{
			char seps2[] = " ,\t\n()="; //구분자
			token = strtok_s(NULL, seps2, &context); //1
			limitMode = token;
			//_value = atof(token.c_str());

			if (limitMode == "catOffsetLimit")
			{
				_type = 0;
			}
			else if (limitMode == "catUpThruNextLimit")
			{
				_type = 1;
			}
			else if (limitMode == "catUpToLastLimit")
			{
				_type = 2;
			}
			else if (limitMode == "catUpToPlaneLimit")
			{
				_type = 3;
			}
			else if (limitMode == "catUpToSurfaceLimit")
			{
				_type = 4;
			}
		}

		if (GetPart()->GetFeatureByName(_featureName))
		{
			GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, _value, _type);
		}

		context = NULL;
	}

	void AttLimit::CheckAttribute(string name, double value, int type)
	{
		//Length Class의 인스턴스에서 오는 값은 "Dimension", (double)0이다
		if (name == "Dimension")
		{
			GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, value, type);
		}
	}

}