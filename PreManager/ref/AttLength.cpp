#include "stdafx.h"
#include "AttLength.h"

namespace Pre {
	AttLength::AttLength(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttLength::~AttLength()
	{
	}

	void AttLength::GetInfo(ifstream &is)
	{
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)
		while (strncmp(buffer, "Set", 3))  // Set Length1을 찾는 부분
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string LengthValue;
		char seps_temp[] = " ,\t\n()=."; //구분자
		char * context = NULL;

		LengthValue = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		LengthValue = strtok_s(NULL, seps_temp, &context);  //두번째 단어

		LengthValue = strtok_s(NULL, seps_temp, &context);  //세번째 단어 : _featureName
		_featureName = LengthValue;

		LengthValue = strtok_s(NULL, seps_temp, &context);  //네번째 단어 : _attributeName
		_attributeName = LengthValue;

		context = NULL;
	}

	void AttLength::Modify(char * buffer)  //Script 파일에 바로 Class 인스턴스 이름이 나올때 
	{
		string token;
		char seps[] = " ,\t\n().=";
		char * context = NULL;

		token = strtok_s(buffer, seps, &context);
		token = strtok_s(NULL, seps, &context);

		if (token == "Value")
		{
			char seps2[] = " ,\t\n()=";
			token = strtok_s(NULL, seps2, &context);

			_value = atof(token.c_str());
		}

		if (GetPart()->GetFeatureByName(_featureName)) //Set length2 = hole1.Diameter 와 같은 경우. 즉 feature(e.g. hole1)의 attribute(e.g. Diameter)를 변경.
		{
			GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, _value, (int)0);
		}
		else if (GetPart()->GetAttributeItemByName(_featureName)) //Set length9 = limit1.Dimension 와 같은 경우. 즉 attribute(e.g. limit1)의 sub-attribute(e.g. Dimension)를 변경.
		{
			GetPart()->GetAttributeItemByName(_featureName)->CheckAttribute(_attributeName, _value, (int)0);
		}
	}
}