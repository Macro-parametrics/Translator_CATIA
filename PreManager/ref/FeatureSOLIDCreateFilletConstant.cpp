#include "stdafx.h"
#include "FeatureSOLIDCreateFilletConstant.h"
#include "ReferenceEntity.h"

namespace Pre {
	FeatureSOLIDCreateFilletConstant::FeatureSOLIDCreateFilletConstant(Part * pPart, int type, string name) :
		Feature(pPart, type, name)
	{
		_propagationType = TransCAD::Minimal;
	}

	FeatureSOLIDCreateFilletConstant::~FeatureSOLIDCreateFilletConstant(void)
	{
	}

	void FeatureSOLIDCreateFilletConstant::GetInfo(ifstream & is)
	{
		// Chamfer의 정보 입력
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)

		while (strncmp(buffer, "Set", 3))  // Set chamfer1을 찾는 부분
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string filletValue;
		char seps_temp[] = " ,\t\n()="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		filletValue = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		filletValue = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		filletValue = strtok_s(NULL, seps_temp, &context);  //세번째 단어
		filletValue = strtok_s(NULL, seps_temp, &context);  //네번째 단어 : reference name

		// ReferenceElement 정보를 feature에 저장.	
		_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(filletValue));

		filletValue = strtok_s(NULL, seps_temp, &context);  //propagation
		_propagation = atoi(filletValue.c_str());

		filletValue = strtok_s(NULL, seps_temp, &context);  //length1
		_radius = atof(filletValue.c_str());

		context = NULL;
	}

	void FeatureSOLIDCreateFilletConstant::ToTransCAD()
	{
		TransCAD::IReferencesPtr spReferences = GetPart()->_spPart->CreateReferences();
		vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();

		while (iter != _referenceEntityList.end())
		{
			(*iter)->ToTransCAD();
			if ((*iter)->GetReferencePtr() != NULL)
				spReferences->Add((*iter)->GetReferencePtr()); //다수의 레퍼런스(엣지) 선택 시 추가
			iter++;
		}

		if (_propagation == 0) //minimal
			_propagationType = TransCAD::Minimal;
		else if (_propagation == 1) //tangent
			_propagationType = TransCAD::Tangency;
		else
		{
		}

		// Create a fillet constant feature
		GetPart()->_spFeatures->AddNewSolidFilletConstantFeature(_name.c_str(), spReferences, _radius, _propagationType);
	}

	void FeatureSOLIDCreateFilletConstant::Modify(char * buffer)
	{
		string token;
		char seps[] = " ,\t\n().="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		token = strtok_s(buffer, seps, &context);  //chamfer1
		token = strtok_s(NULL, seps, &context); //두번째 구분자

		if (token == "AddObjectToFillet")
		{
			token = strtok_s(NULL, seps, &context); //세번째 구분자

			// ReferenceElement 정보를 vector에 저장.	
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(token));
		}

		if (token == "EdgePropagation")
		{
			token = strtok_s(NULL, seps, &context); //세번째 구분자

			if (token == "catMinimalFilletEdgePropagation")
				_propagation = 0;
			else if (token == "catTangencyFilletEdgePropagation")
				_propagation = 1;
		}
	}

	void FeatureSOLIDCreateFilletConstant::CheckAttribute(string name, double value, int type)
	{
		if (name == "Radius")
		{
			_radius = value;
		}
	}
}