#include "stdafx.h"
#include "AttAngle.h"

namespace Pre {
	AttAngle::AttAngle(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttAngle::~AttAngle()
	{
	}

	void AttAngle::GetInfo(ifstream &is)
	{
		// Angle의 정보 입력
		char buffer[500];

		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set angle1을 찾는 부분
		{
			is.getline(buffer, 500);
		}

		string AngleValue;
		char seps_temp[] = " ,\t\n()=."; //구분자
		char * context = NULL;

		AngleValue = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		AngleValue = strtok_s(NULL, seps_temp, &context);  //두번째 단어

		AngleValue = strtok_s(NULL, seps_temp, &context);  //세번째 단어 : _featureName, shaft1
		_featureName = AngleValue;

		AngleValue = strtok_s(NULL, seps_temp, &context);  //네번째 단어 : _attributeName, SecondAngle
		_attributeName = AngleValue;

		context = NULL;
	}

	void AttAngle::Modify(char * buffer)  //Script 파일에 바로 Class 인스턴스 이름이 나올때 
	{
		string token;
		char seps[] = " ,\t\n().="; //구분자
		char * context = NULL;

		token = strtok_s(buffer, seps, &context);  //length2
		token = strtok_s(NULL, seps, &context); //두번째 구분자

		if (token == "Value")
		{
			char seps2[] = " ,\t\n()="; //구분자
			token = strtok_s(NULL, seps2, &context); //5.700000

			_value = atof(token.c_str());
		}

		if (GetPart()->GetFeatureByName(_featureName)) //Set length2 = hole1.Diameter 와 같은 경우. 즉 feature(e.g. hole1)의 attribute(e.g. Diameter)를 변경.
		{
			GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, _value, _type);
		}
		else if (GetPart()->GetAttributeItemByName(_featureName)) //Set length9 = limit1.Dimension 와 같은 경우. 즉 attribute(e.g. limit1)의 sub-attribute(e.g. Dimension)를 변경.
		{
			GetPart()->GetAttributeItemByName(_featureName)->CheckAttribute(_attributeName, _value, _type);
		}

		context = NULL;
	}
}