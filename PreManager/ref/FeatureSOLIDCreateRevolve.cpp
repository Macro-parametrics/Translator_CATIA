#include "stdafx.h"
#include "FeatureSOLIDCreateRevolve.h"
#include "ReferenceEntity.h"
#include "FSketchItem.h"
#include "FSKETCHCreate2DLine2Points.h"
#include "FSKETCHCreate2DCircleCenterPoint.h"

//using namespace Pre;

namespace Pre {

	FeatureSOLIDCreateRevolve::FeatureSOLIDCreateRevolve(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		startAngle = 360;	//시작각은 360.
		endAngle = 0;		//끝각은 0이다.
		startCondition = "Dimension";
		endCondition = "Dimension";
		flip = 0;				//flip은 없다.

		_featureProfileSketch = 0;
	}

	FeatureSOLIDCreateRevolve::~FeatureSOLIDCreateRevolve()
	{
	}

	void FeatureSOLIDCreateRevolve::GetInfo(ifstream &is)
	{
		char buffer[500];
		f_Getline_Custom(is, buffer, 500)

		while (strncmp(buffer, "Set", 3))  // Set pad1을 찾는 부분
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string sketchManagerName;
		char seps_temp[] = " ,\t\n().="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		sketchManagerName = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //세번째 단어
		sketchManagerName = strtok_s(NULL, seps_temp, &context);  //네번째 단어

		if (sketchManagerName == "AddNewShaft" || sketchManagerName == "AddNewGroove")
		{
			sketchManagerName = strtok_s(NULL, seps_temp, &context);

			// SketchManager의 이름으로 원하는 Sketch 정보를 가져온다.	
			_featureProfileSketch = (FSketch*)GetPart()->GetFeatureByName(sketchManagerName);

			// Feature와 Sketch정보를 저장한다.
			GetPart()->AddFeatureNameSketchName(GetFeatureName(), sketchManagerName);
		}
		else if (sketchManagerName == "AddNewShaftFromRef" || sketchManagerName == "AddNewGrooveFromRef")
		{
			sketchManagerName = strtok_s(NULL, seps_temp, &context);

			// Reference를 저장한다.
			_referenceEntityList.push_back(_pPart->GetReferenceEntityByName(sketchManagerName));

			// Feature와 Sketch정보를 저장한다. 단, Sketch 정보가 있는 경우
			sketchManagerName = _pPart->GetReferenceEntityByName(sketchManagerName)->GetFeatureName();

			if (!sketchManagerName.empty())
				GetPart()->AddFeatureNameSketchName(GetFeatureName(), sketchManagerName);
		}

		context = NULL;
	}

	void FeatureSOLIDCreateRevolve::ToTransCAD()
	{
		// 스케치 외부에서 회전 축을 잡을 경우, 그 축을 스케치 내부에 생성
		AddNewRevolveAxis();

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

		if (_type == 5)
			// Create a protrusion Revolve feature with the sketch
			GetPart()->_spFeatures->AddNewSolidProtrusionRevolveFeature(_name.c_str(), spBaseSketch, startAngle, TransCAD::StdRevolveEndType_Blind, endAngle, TransCAD::StdRevolveEndType_Blind, false);
		else
			// Create a cut Revolve feature with the sketch
			GetPart()->_spFeatures->AddNewSolidCutRevolveFeature(_name.c_str(), spBaseSketch, startAngle, TransCAD::StdRevolveEndType_Blind, endAngle, TransCAD::StdRevolveEndType_Blind, false);
	}

	void FeatureSOLIDCreateRevolve::CheckAttribute(string name, double value, int type)
	{
		if (name == "FirstAngle")
		{
			startAngle = value;
		}
		else if (name == "SecondAngle")
		{
			endAngle = value;
		}
	}

	void FeatureSOLIDCreateRevolve::Modify(char * buffer)
	{
		string token;
		char * context = NULL;
		char seps[] = " ,\t\n().=";		//구분자

		token = strtok_s(buffer, seps, &context);		//hole1
		token = strtok_s(NULL, seps, &context);		//두번째 구분자

		if (token == "RevoluteAxis")
		{
			token = strtok_s(NULL, seps, &context);	//세번째 구분자

			_revolveAxis = token; // reference
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
	}

	void FeatureSOLIDCreateRevolve::AddNewRevolveAxis()
	{
		if (!_revolveAxis.empty())
		{
			// 레퍼런스 축의 스케치
			string _revRefSketName = GetPart()->GetReferenceEntityByName(_revolveAxis)->GetWireInfo()._sketchName;		// 스케치 이름
			string _revRefEntName = GetPart()->GetReferenceEntityByName(_revolveAxis)->GetWireInfo()._sketchItemName;	// 스케치에 포함 된 라인. 즉, 회전 축
			FSketch * pRefSketch = (FSketch*)GetPart()->GetFeatureByName(_revRefSketName);								// 레퍼런스 스케치

			string _shaftSketName = GetPart()->GetSketchNameFromFeatureName(_name);			// Revolve 스케치 이름
			FSketch * pRevSketch = (FSketch*)GetPart()->GetFeatureByName(_shaftSketName);	// Revolve 스케치

			if (_revRefSketName.find("sketch") == 0)
			{
				// 일반적인 경우.
				// 스케치 내부의 하나의 Line을 Centerline으로 지정함
				if (_revRefSketName == _shaftSketName)
				{
					FSKETCHCreate2DLine2Points * pRefLine = (FSKETCHCreate2DLine2Points*)pRefSketch->GetSketchItemFromName(_revRefEntName);
					origin = pRefLine->GetStartPoint();
					direction = pRefLine->GetEndPoint();

				}
				// Revolve의 Sketch와 Revolve Axis의 sketch가 다른 경우
				// 예를 들어, 다른 원기둥의 Axis를 참조 할 경우
				else
				{
					// Circle을 Extrude한 원기둥인 경우
					if (_revRefEntName.find("circle2D") == 0)
					{
						FSKETCHCreate2DCircleCenterPoint * pRefCircle = (FSKETCHCreate2DCircleCenterPoint*)pRefSketch->GetSketchItemFromName(_revRefEntName);

						// circle의 중점. 단, 이 때는 Local 좌표
						pRefCircle->GetCenterPoint(origin);

						// circle 중점의 normal 벡터
						direction.SetX(origin.X());
						direction.SetY(origin.Y());
						direction.SetZ(origin.Z() + 1.0);
					}
					// 일반 스케치를 Revolve한 회전체의 경우
					else
					{
						string refFeatName = GetPart()->GetFeatureNameFromSketchName(_revRefSketName);

						FeatureSOLIDCreateRevolve * pRevFeature = (FeatureSOLIDCreateRevolve *)GetPart()->GetFeatureByName(refFeatName);
						origin = pRevFeature->GetAxisOrigin();
						direction = pRevFeature->GetAxisDirection();
					}
				}
			}
			// 스케치 내부의 Local 좌표계를 Axis로 잡는 경우
			else if (_revRefSketName.find("axis2D") == 0)
			{
				_revRefSketName = "sketch" + _revRefSketName.substr(6, 1);
				FSketch * pRevSketch = (FSketch*)GetPart()->GetFeatureByName(_revRefSketName);

				if (_revRefEntName == "HDirection")
					direction.SetX(1.0);
				else if (_revRefEntName == "VDirection")
					direction.SetY(1.0);
			}

			//새로운 Centerline 생성
			CreateRevolveAxis(pRefSketch, pRevSketch, origin, direction);
		}
	}

	void FeatureSOLIDCreateRevolve::CreateRevolveAxis(FSketch * pRefSketch, FSketch * pRevSketch, Point3D origin, Point3D direction)
	{
		if (pRefSketch && (pRefSketch != pRevSketch))
		{
			TransformRevolveAxis(pRefSketch->sketchCoord, pRevSketch->sketchCoord, origin);
			TransformRevolveAxis(pRefSketch->sketchCoord, pRevSketch->sketchCoord, direction);
		}

		TransCAD::ISketchEditorPtr spSketEditor = pRevSketch->GetSketchFeaturePtr()->OpenEditorEx(VARIANT_FALSE);
		spSketEditor->Create2DCenterline2Points("ShaftAxis", origin.X(), origin.Y(), direction.X(), direction.Y());
	}

	void FeatureSOLIDCreateRevolve::TransformRevolveAxis(double refSketCSYS[9], double revSketCSYS[9], Point3D &coords)
	{
		double globalCSYS[3][3], localCSYS[3][3];

		// Global 좌표계
		globalCSYS[0][0] = 1; globalCSYS[0][1] = 0; globalCSYS[0][2] = 0;
		globalCSYS[1][0] = 0; globalCSYS[1][1] = 1; globalCSYS[1][2] = 0;
		globalCSYS[2][0] = 0; globalCSYS[2][1] = 0; globalCSYS[2][2] = 1;

		// Local 좌표계 (Reference 스케치)
		localCSYS[0][0] = refSketCSYS[3]; localCSYS[0][1] = refSketCSYS[4]; localCSYS[0][2] = refSketCSYS[5];
		localCSYS[1][0] = refSketCSYS[6]; localCSYS[1][1] = refSketCSYS[7]; localCSYS[1][2] = refSketCSYS[8];
		localCSYS[2][0] = localCSYS[0][1] * localCSYS[1][2] - localCSYS[0][2] * localCSYS[1][1];
		localCSYS[2][1] = localCSYS[0][2] * localCSYS[1][0] - localCSYS[0][0] * localCSYS[1][2];
		localCSYS[2][2] = localCSYS[0][0] * localCSYS[1][1] - localCSYS[0][1] * localCSYS[1][0];

		// Local 좌표계 → Global 좌표계 변환
		TransformCoordinates(localCSYS, globalCSYS, coords);

		// 좌표계 중점 이동
		// Reference 스케치의 중점만큼 양(+)의 이동(Local→Global), Revolve 스케치의 중점만큼 음(-)의 이동(Global→Local)
		coords.SetX(coords.X() + refSketCSYS[0] - revSketCSYS[0]);
		coords.SetY(coords.Y() + refSketCSYS[1] - revSketCSYS[1]);
		coords.SetZ(coords.Z() + refSketCSYS[2] - revSketCSYS[2]);

		// Local 좌표계 (Revolve 스케치)	
		localCSYS[0][0] = revSketCSYS[3]; localCSYS[0][1] = revSketCSYS[4]; localCSYS[0][2] = revSketCSYS[5];
		localCSYS[1][0] = revSketCSYS[6]; localCSYS[1][1] = revSketCSYS[7]; localCSYS[1][2] = revSketCSYS[8];
		localCSYS[2][0] = localCSYS[0][1] * localCSYS[1][2] - localCSYS[0][2] * localCSYS[1][1];
		localCSYS[2][1] = localCSYS[0][2] * localCSYS[1][0] - localCSYS[0][0] * localCSYS[1][2];
		localCSYS[2][2] = localCSYS[0][0] * localCSYS[1][1] - localCSYS[0][1] * localCSYS[1][0];

		// Global 좌표계 → Local 좌표계 변환
		TransformCoordinates(globalCSYS, localCSYS, coords);
	}

	void FeatureSOLIDCreateRevolve::TransformCoordinates(double sourceCSYS[3][3], double targetCSYS[3][3], Point3D &coords)
	{
		double sourceCoords[3], targetCoords[3], transMatrix[3][3];

		// sourceCSYS의 좌표점
		sourceCoords[0] = coords.X();
		sourceCoords[1] = coords.Y();
		sourceCoords[2] = coords.Z();

		// Transformation matrix 구하기
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				transMatrix[i][j] = 0;

				for (int k = 0; k < 3; ++k)
					transMatrix[i][j] = transMatrix[i][j] + targetCSYS[i][k] * sourceCSYS[j][k];
			}
		}

		// Transformation matrix를 기반으로 targetCSYS의 좌표점 구하기
		for (int i = 0; i < 3; ++i)
		{
			targetCoords[i] = 0;

			for (int j = 0; j < 3; ++j)
				targetCoords[i] = targetCoords[i] + transMatrix[i][j] * sourceCoords[j];

		}

		coords.SetX(targetCoords[0]);
		coords.SetY(targetCoords[1]);
		coords.SetZ(targetCoords[2]);
	}
}