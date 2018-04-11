#include "stdafx.h"
#include "FDatumPlaneOffset.h"
#include "ReferenceEntity.h"

namespace Pre {
	FDatumPlaneOffset::FDatumPlaneOffset(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		_distance = 0;
		_flip = 0;
	}

	FDatumPlaneOffset::~FDatumPlaneOffset()
	{
	}

	void FDatumPlaneOffset::GetInfo(ifstream &is)
	{
		// Chamfer의 정보 입력
		char buffer[500];
		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set chamfer1을 찾는 부분
		{
			is.getline(buffer, 500);
		}

		string planeOffsetValue;
		char seps_temp[] = " ,\t\n()="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		planeOffsetValue = strtok_s(buffer, seps_temp, &context); //Set
		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //hybridShapePlaneOffset1
		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //hybridShapeFactory1.AddNewPlaneOffset

		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //reference1

		// ReferenceElement 정보를 feature에 저장.	
		// 처음 reference entity의 이름을 읽고 다시 그 이름을 가지고 다른 reference entity의 이름을 가져온다.

		string target_surface;
		target_surface = GetPart()->GetReferenceEntityByName(planeOffsetValue)->GetFeatureName();

		if (GetPart()->GetReferenceEntityByName(target_surface))
			_referenceEntityList.push_back(GetPart()->GetReferenceEntityByName(target_surface));
		else
			_referenceEntityList.push_back(GetPart()->GetReferenceEntityByName(planeOffsetValue));


		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //20.000000
		_distance = atof(planeOffsetValue.c_str());

		planeOffsetValue = strtok_s(NULL, seps_temp, &context);  //False

		if (planeOffsetValue == "False")
		{
			_flip = 0;
		}
		else
		{
			_flip = 1;
		}

		context = NULL;
	}

	void FDatumPlaneOffset::ToTransCAD()
	{
		cerr << "#-- Call DatumPlaneOffset::Translate2TransCAD() --#" << endl;

		TransCAD::IReferencePtr spPlane;

		vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();
		while (iter != _referenceEntityList.end())
		{
			(*iter)->ToTransCAD();

			if ((*iter)->GetReferencePtr() != NULL)
				spPlane = (*iter)->GetReferencePtr();

			iter++;
		}

		// Create a datum plane offset feature
		GetPart()->_spFeatures->AddNewDatumPlaneOffsetFeature(_name.c_str(), spPlane, _distance, _flip);

		cerr << "#-- End DatumPlaneOffset::Translate2TransCAD() --#" << endl << endl;
	}

	void FDatumPlaneOffset::Modify(char * buffer) //Script 파일에 바로 Class 인스턴스 이름이 나올때 
	{
	}

	void FDatumPlaneOffset::CheckAttribute(string name, double value, int type)
	{
	}




}