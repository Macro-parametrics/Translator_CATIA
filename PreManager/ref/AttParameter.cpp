#include "stdafx.h"
#include "AttParameter.h"

namespace Pre {
	AttParameter::AttParameter(Part * pPart, int type, string name)
		: AttributeItem(pPart, type, name)
	{
		_value = 0;
	}

	AttParameter::~AttParameter()
	{
	}

	void AttParameter::GetInfo(ifstream &is)
	{
		// Limit의 정보 입력
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)
		while (strncmp(buffer, "Set", 3))  // Set limit2을 찾는 부분
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string parameterValue;
		char seps_temp[] = " ,\"\\\t\n().="; //구분자
		char * context = NULL;

		parameterValue = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		parameterValue = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		parameterValue = strtok_s(NULL, seps_temp, &context);  //세번째 단어
		parameterValue = strtok_s(NULL, seps_temp, &context);
		parameterValue = strtok_s(NULL, seps_temp, &context);
		parameterValue = strtok_s(NULL, seps_temp, &context);
		parameterValue = strtok_s(NULL, seps_temp, &context);

		_featureName = GetFeatureTypeInLowerString(parameterValue);	// ex) Shaft

		if (_featureName == "constRadEdgeFillet" || _featureName == "chamfer")
		{
			parameterValue = strtok_s(NULL, seps_temp, &context); // 특징형상 번호

			_featureName = _featureName + parameterValue;	// ex) shaft1

			parameterValue = strtok_s(NULL, seps_temp, &context);
			parameterValue = strtok_s(NULL, seps_temp, &context);
			parameterValue = strtok_s(NULL, seps_temp, &context);
			_attributeName = parameterValue;
		}
		else
		{
			parameterValue = strtok_s(NULL, seps_temp, &context); // 특징형상 번호

			_featureName = _featureName + parameterValue;	// ex) shaft1

			parameterValue = strtok_s(NULL, seps_temp, &context);
			_attributeName = parameterValue;
		}

		context = NULL;
	}

	void AttParameter::Modify(char * buffer)
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
			GetPart()->GetFeatureByName(_featureName)->CheckAttribute(_attributeName, _value, (int)0);
		}
		else if (GetPart()->GetAttributeItemByName(_featureName)) //Set length9 = limit1.Dimension 와 같은 경우. 즉 attribute(e.g. limit1)의 sub-attribute(e.g. Dimension)를 변경.
		{
			GetPart()->GetAttributeItemByName(_featureName)->CheckAttribute(_attributeName, _value, (int)0);
		}

		context = NULL;
	}

	void AttParameter::CheckAttribute(string name, double value, int type)
	{
	}

	std::string AttParameter::GetFeatureTypeInLowerString(std::string _featureType)
	{
		string featureType;

		if (_featureType == "Pad" || _featureType == "패드")
			featureType = "pad";
		else if (_featureType == "Pocket" || _featureType == "포켓")
			featureType = "pocket";
		else if (_featureType == "EdgeFillet")
			featureType = "constRadEdgeFillet";
		else if (_featureType == "Rib")
			featureType = "rib";
		else if (_featureType == "Shaft" || _featureType == "샤프트")
			featureType = "shaft";
		else if (_featureType == "Chamfer" || _featureType == "챔퍼")
			featureType = "chamfer";

		return featureType;
	}
}