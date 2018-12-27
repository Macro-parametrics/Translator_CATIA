#include "stdafx.h"
#include "FeatureSOLIDCreateSweep.h"
#include "ReferenceEntity.h"
namespace Pre {
	FeatureSOLIDCreateSweep::FeatureSOLIDCreateSweep(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		sketch1 = "none";
		sketch2 = "none";
	}

	FeatureSOLIDCreateSweep::~FeatureSOLIDCreateSweep()
	{
	}

	void FeatureSOLIDCreateSweep::GetInfo(ifstream &is)
	{
		//sketch manager의 이름을 찾은 후 ProtrusionExtrude의 데이터로 입력
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)

		while (strncmp(buffer, "Set", 3))  // Set rib1을 찾는 부분
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string sketchManagerName;
		char seps_temp[] = " ,\t\n()="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		sketchManagerName = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //세번째 단어

		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //네번째 단어, reference13

		// Nothing인 경우에는 Modify 함수에서 처리
		if (sketchManagerName != "Nothing")
		{
			sketch1 = GetPart()->GetReferenceEntityByName(sketchManagerName)->GetFeatureName();

			sketchManagerName = strtok_s(NULL, seps_temp, &context);  //여섯번째 단어, reference14
			sketch2 = GetPart()->GetReferenceEntityByName(sketchManagerName)->GetFeatureName();

			// Feature와 Sketch정보를 저장한다.
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketch1);
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketch2);
		}

		context = NULL;
	}

	void FeatureSOLIDCreateSweep::ToTransCAD()
	{
		cerr << "#-- Call ProtrusionSweep::Translate2TransCAD() --#" << endl;

		bstr_t baseSketchName((GetPart()->GetFeatureByName(sketch1)->GetFeatureName().c_str()));
		TransCAD::IReferencePtr spBaseSketch = GetPart()->_spPart->SelectObjectByName(baseSketchName);

		bstr_t guideSketchName((GetPart()->GetFeatureByName(sketch2)->GetFeatureName().c_str()));
		TransCAD::IReferencePtr spGuideSketch = GetPart()->_spPart->SelectObjectByName(guideSketchName);

		if (_type == 4)
			// Create a protrusion sweep feature with the sketches
			GetPart()->_spFeatures->AddNewSolidProtrusionSweepFeature(_name.c_str(), spBaseSketch, spGuideSketch);
		else
			// Create a protrusion sweep feature with the sketches
			GetPart()->_spFeatures->AddNewSolidCutSweepFeature(_name.c_str(), spBaseSketch, spGuideSketch);

		cerr << "#-- End ProtrusionSweep::Translate2TransCAD() --#" << endl << endl;
	}

	void FeatureSOLIDCreateSweep::CheckAttribute(string name, double value, int type)
	{
	}

	void FeatureSOLIDCreateSweep::Modify(char * buffer)  //Script 파일에 바로 Class 인스턴스 이름이 나올때 
	{
		string token;
		char seps[] = " ,\t\n().="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		token = strtok_s(buffer, seps, &context);//rib1
		token = strtok_s(NULL, seps, &context);	//SetProfileElement

		if (token == "SetProfileElement")
		{
			token = strtok_s(NULL, seps, &context); //두번째 구분자

			sketch1 = GetPart()->GetReferenceEntityByName(token)->GetFeatureName();
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketch1);
		}
		else if (token == "CenterCurveElement")
		{
			token = strtok_s(NULL, seps, &context); //두번째 구분자

			sketch2 = GetPart()->GetReferenceEntityByName(token)->GetFeatureName();
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketch2);
		}

		context = NULL;
	}


}