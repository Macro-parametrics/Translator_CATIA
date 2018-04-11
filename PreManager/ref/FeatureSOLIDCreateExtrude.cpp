#include "stdafx.h"
#include "FeatureSOLIDCreateExtrude.h"
#include "FSketch.h"
#include "ReferenceEntity.h"
namespace Pre {

	FeatureSOLIDCreateExtrude::FeatureSOLIDCreateExtrude(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		_startDepth = 0;	// 초기값 0
		_endDepth = 0;		// 초기값 0
		_startCondition = TransCAD::Blind;	// 초기값 Blind
		_endCondition = TransCAD::Blind;	// 초기값 Blind
		_flip = VARIANT_FALSE;			// 초기값 false

		_featureProfileSketch = 0;
	}

	FeatureSOLIDCreateExtrude::~FeatureSOLIDCreateExtrude(void)
	{
	}

	void FeatureSOLIDCreateExtrude::GetInfo(ifstream & is)
	{
		char buffer[500];

		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set pad1을 찾는 부분
		{
			is.getline(buffer, 500);
		}

		string sketchManagerName;
		char seps_temp[] = " ,\t\n().="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		sketchManagerName = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //세번째 단어
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //네번째 단어

		if (sketchManagerName == "AddNewPad" || sketchManagerName == "AddNewPocket")
		{
			sketchManagerName = strtok_s(NULL, seps_temp, &context);

			// SketchManager의 이름으로 원하는 Sketch 정보를 가져온다.	
			_featureProfileSketch = (FSketch*)GetPart()->GetFeatureByName(sketchManagerName);

			// Feature와 Sketch정보를 저장한다.
			GetPart()->AddFeatureNameSketchName(this->GetFeatureName(), sketchManagerName);
		}
		else if (sketchManagerName == "AddNewPadFromRef" || sketchManagerName == "AddNewPocketFromRef")
		{
			sketchManagerName = strtok_s(NULL, seps_temp, &context);

			// Reference를 저장한다.
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(sketchManagerName));

			// Feature와 Sketch정보를 저장한다. 단, Sketch 정보가 있는 경우
			sketchManagerName = _pPart->GetReferenceEntityByName(sketchManagerName)->GetFeatureName();

			if (!sketchManagerName.empty())
				GetPart()->AddFeatureNameSketchName(GetFeatureName(), sketchManagerName);
		}

		char seps_temp2[] = " ,\t\n()="; //구분자
		sketchManagerName = strtok_s(NULL, seps_temp2, &context);
		_startDepth = atof(sketchManagerName.c_str());

		context = NULL;
	}

	void FeatureSOLIDCreateExtrude::ToTransCAD()
	{
		TransCAD::IReferencePtr spBaseSketch;

		if (_featureProfileSketch)
		{
			bstr_t baseSketchName(GetFeatureProfileSketch()->GetFeatureName().c_str());
			spBaseSketch = GetPart()->_spPart->SelectObjectByName(baseSketchName);
		}
		else
		{
			vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();
			while (iter != _referenceEntityList.end())
			{
				(*iter)->ToTransCAD();
				if ((*iter)->GetReferencePtr() != NULL)
					spBaseSketch = (*iter)->GetReferencePtr();

				iter++;
			}
		}

		if (_type == 1)
			// Create a protrusion extrude feature with the sketch
			GetPart()->_spFeatures->AddNewSolidProtrusionExtrudeFeature(_name.c_str(), spBaseSketch, _startDepth, _startCondition, _endDepth, _endCondition, _flip);
		else
			// Create a cut extrude feature with the sketch
			GetPart()->_spFeatures->AddNewSolidCutExtrudeFeature(_name.c_str(), spBaseSketch, _startDepth, _startCondition, _endDepth, _endCondition, _flip);
	}

	void FeatureSOLIDCreateExtrude::Modify(char * buffer)
	{
		string token;
		char seps[] = " ,\t\n().="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		token = strtok_s(buffer, seps, &context);
		token = strtok_s(NULL, seps, &context);

		if (token == "DirectionOrientation") // 여기서 Flip 여부를 처리
		{
			token = strtok_s(NULL, seps, &context);

			if (token == "catInverseOrientation")
			{
				if (_type == 1)
					_flip = VARIANT_TRUE;
				else if (_type == 2)
					_flip = VARIANT_FALSE;
			}
			else if (token == "catRegularOrientation")
			{
				if (_type == 1)
					_flip = VARIANT_FALSE;
				else if (_type == 2)
					_flip = VARIANT_TRUE;
			}
		}
		else if (token == "IsSymmetric")
		{
			token = strtok_s(NULL, seps, &context);

			if (token == "True")
				_endDepth = _startDepth;
			else if (token == "False")
				_endDepth = 0;
		}
		else if (token == "SetProfileElement")
		{
			token = strtok_s(NULL, seps, &context);

			// ReferenceElement 정보를 vector에 저장.	
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(token));

			// Feature와 Sketch정보를 저장한다. 단, Sketch 정보가 있는 경우
			token = _pPart->GetReferenceEntityByName(token)->GetFeatureName();
			if (!token.empty())
				GetPart()->AddFeatureNameSketchName(GetFeatureName(), token);
		}

		context = NULL;
	}

	void FeatureSOLIDCreateExtrude::CheckAttribute(string name, double value, int type)
	{
		if (name == "FirstLimit")
		{
			if (type == 0)
			{
				_startDepth = value;
				_startCondition = TransCAD::Blind;	// 0: catOffsetLimit
			}
			else if (type == 1 || type == 2)			// 1: catUpThruNextLimit, 2: catUpToLastLimit
			{
				_startDepth = 0;
				_startCondition = TransCAD::ThroughAll;
			}
			else if (type == 3 || type == 4)			// 3: catUpToPlaneLimit, 4: catUpToSurfaceLimit. 수정 필요
			{
				_startDepth = 0;
				_startCondition = TransCAD::ThroughAll;
			}
		}
		else if (name == "SecondLimit")
		{
			if (type == 0)
			{
				_endDepth = value;
				_endCondition = TransCAD::Blind;	// 0: catOffsetLimit
			}
			else if (type == 1 || type == 2)			// 1: catUpThruNextLimit, 2: catUpToLastLimit
			{
				_endDepth = 0;
				_endCondition = TransCAD::ThroughAll;
			}
			else if (type == 3 || type == 4)			// 3: catUpToPlaneLimit, 4: catUpToSurfaceLimit. 수정 필요
			{
				_endDepth = 0;
				_endCondition = TransCAD::ThroughAll;
			}
		}
	}
}