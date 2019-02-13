#include "stdafx.h"
#include "AttAngularRepartation.h"

namespace Pre {
	AttAngularRepartation::AttAngularRepartation(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttAngularRepartation::~AttAngularRepartation()
	{
	}

	void AttAngularRepartation::GetInfo(ifstream &is)
	{
		char buffer[500];

		f_Getline_Custom(is, buffer, 500)
		while (strncmp(buffer, "Set", 3))
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string LengthValue;
		char seps_temp[] = " ,\t\n()=."; //구분자
		char * context = NULL;

		LengthValue = strtok_s(buffer, seps_temp, &context);	//첫번째 단어
		LengthValue = strtok_s(NULL, seps_temp, &context);	//두번째 단어

		LengthValue = strtok_s(NULL, seps_temp, &context);	//세번째 단어 : circPattern1
		_featureName = LengthValue;

		LengthValue = strtok_s(NULL, seps_temp, &context);	//네번째 단어 : AngularRepartition
		_attributeName = LengthValue;

		context = NULL;
	}

	void AttAngularRepartation::Modify(char * buffer)  //Script 파일에 바로 Class 인스턴스 이름이 나올때 
	{
	}

	void AttAngularRepartation::CheckAttribute(string name, double value, int type)
	{
		if (name == "InstancesCount")
			type = 0;
		else if (name == "AngularSpacing")
			type = 1;

		GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, value, type);
	}
}