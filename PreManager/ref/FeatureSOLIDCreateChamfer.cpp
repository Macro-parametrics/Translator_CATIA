#include "stdafx.h"
#include "FeatureSOLIDCreateChamfer.h"
#include "ReferenceEntity.h"

namespace Pre {
	FeatureSOLIDCreateChamfer::FeatureSOLIDCreateChamfer(Part * pPart, int type, string name) :
		Feature(pPart, type, name)
	{
	}

	FeatureSOLIDCreateChamfer::~FeatureSOLIDCreateChamfer(void)
	{
	}

	void FeatureSOLIDCreateChamfer::GetInfo(ifstream & is)
	{
		// Chamfer의 정보 입력
		char buffer[500];
		is.getline(buffer, 500);

		while (strncmp(buffer, "Set", 3))  // Set chamfer1을 찾는 부분
		{
			is.getline(buffer, 500);
		}

		string chamferValue;
		char seps_temp[] = " ,\t\n()="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		chamferValue = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		chamferValue = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		chamferValue = strtok_s(NULL, seps_temp, &context);  //세번째 단어

		chamferValue = strtok_s(NULL, seps_temp, &context);  //네번째 단어 : reference name

		// ReferenceElement 정보를 feature에 저장.	
		_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(chamferValue));

		chamferValue = strtok_s(NULL, seps_temp, &context);  //propagation
		_propagation = atoi(chamferValue.c_str());

		chamferValue = strtok_s(NULL, seps_temp, &context);  //mode
		_mode = atoi(chamferValue.c_str());

		chamferValue = strtok_s(NULL, seps_temp, &context);  //orientation
		_orientation = atoi(chamferValue.c_str());

		chamferValue = strtok_s(NULL, seps_temp, &context);  //length1
		_length1 = atof(chamferValue.c_str());

		chamferValue = strtok_s(NULL, seps_temp, &context);  //length2
		_length2 = atof(chamferValue.c_str());

		context = NULL;
	}

	void FeatureSOLIDCreateChamfer::ToTransCAD()
	{
		cerr << "#-- Call Chamfer::Translate2TransCAD() --#" << endl;

		TransCAD::IReferencesPtr spReferences = GetPart()->_spPart->CreateReferences();
		vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();

		while (iter != _referenceEntityList.end())
		{
			(*iter)->ToTransCAD();
			if ((*iter)->GetReferencePtr() != NULL)
				spReferences->Add((*iter)->GetReferencePtr()); //다수의 레퍼런스(엣지) 선택 시 추가
			iter++;
		}

		GetPart()->_spFeatures->AddNewSolidChamferFeature(_name.c_str(), spReferences, _length1);

		cerr << "#-- End Chamfer::Translate2TransCAD() --#" << endl << endl;
	}

	void FeatureSOLIDCreateChamfer::Modify(char * buffer)
	{
		string token;
		char seps[] = " ,\t\n().="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		token = strtok_s(buffer, seps, &context);  //chamfer1
		token = strtok_s(NULL, seps, &context); //두번째 구분자

		if (token == "AddElementToChamfer")
		{
			token = strtok_s(NULL, seps, &context); //세번째 구분자

			// ReferenceElement 정보를 vector에 저장.	
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(token));
		}

		if (token == "Mode")
		{
			token = strtok_s(NULL, seps, &context); //세번째 구분자
			_mode = atoi(token.c_str());
		}

		if (token == "Propagation")
		{
			token = strtok_s(NULL, seps, &context); //세번째 구분자
			_propagation = atoi(token.c_str());
		}

		if (token == "Orientation")
		{
			token = strtok_s(NULL, seps, &context); //세번째 구분자
			_orientation = atoi(token.c_str());
		}
	}

	void FeatureSOLIDCreateChamfer::CheckAttribute(string name, double value, int type)
	{
		if (name == "Length1")
		{
			_length1 = value;
		}

		if (name == "Length2")
		{
			_length2 = value;
		}

		if (name == "Angle")
		{
			_length2 = value;
		}
	}
}