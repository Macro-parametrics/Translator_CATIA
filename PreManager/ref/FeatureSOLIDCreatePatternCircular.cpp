#include "stdafx.h"
#include "FeatureSOLIDCreatePatternCircular.h"
#include "FSketch.h"
#include "ReferenceEntity.h"
#include "FSKETCHCreate2DCircleCenterPoint.h"

namespace Pre {
	FeatureSOLIDCreatePatternCircular::FeatureSOLIDCreatePatternCircular(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		CircleSpacing = 0;
		OrgX = 0;
		OrgY = 0;
		OrgZ = 0;
		CircleNumber = 0;

		AngularSpacing = 0;
		DirX = 0;
		DirY = 0;
		DirZ = 0;
		AngularNumber = 0;
	}

	FeatureSOLIDCreatePatternCircular::~FeatureSOLIDCreatePatternCircular()
	{
	}

	void FeatureSOLIDCreatePatternCircular::GetInfo(ifstream &is)
	{
		char buffer[400];
		char * context = NULL;
		is.getline(buffer, 400);
		while (strncmp(buffer, "Set", 3))  // Set chamfer1을 찾는 부분
		{
			is.getline(buffer, 400);
		}


		string circpatternValue;
		char seps_temp[] = " ,\t\n()="; //구분자
		circpatternValue = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		circpatternValue = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		circpatternValue = strtok_s(NULL, seps_temp, &context);  //세번째 단어

		circpatternValue = strtok_s(NULL, seps_temp, &context);  //네번째 단어 

		_spTargetFeature = circpatternValue.c_str();	// Pattern이 적용 될 Target Feature

		circpatternValue = strtok_s(NULL, seps_temp, &context);  //Number of circles
		CircleNumber = atoi(circpatternValue.c_str());

		circpatternValue = strtok_s(NULL, seps_temp, &context);  //Number of instances
		AngularNumber = atoi(circpatternValue.c_str());

		circpatternValue = strtok_s(NULL, seps_temp, &context);  //Circle spacing
		CircleSpacing = atoi(circpatternValue.c_str());

		circpatternValue = strtok_s(NULL, seps_temp, &context);  //Angular spacing
		AngularSpacing = atof(circpatternValue.c_str());

		context = NULL;
	}

	void FeatureSOLIDCreatePatternCircular::ToTransCAD()
	{
		cerr << "#-- Call RectPattern::Translate2TransCAD() --#" << endl;

		cerr << "RectPattern Name:" << _name.c_str() << endl;

		// 패턴 방향 설정
		SetDirection(_RefPlane);

		TransCAD::IReferencePtr spTargetFeature = GetPart()->_spPart->SelectFeatureByName(_spTargetFeature);
		TransCAD::IReferencesPtr spTargetFeatures = GetPart()->_spPart->CreateReferences();
		spTargetFeatures->Add(spTargetFeature);

		GetPart()->_spFeatures->AddNewSolidOperatePatternCircularFeature(_name.c_str(), spTargetFeatures, OrgX, OrgY, OrgZ, DirX, DirY, DirZ,
			AngularNumber, AngularSpacing, CircleNumber, CircleSpacing, true);
		cerr << "#-- End RectPattern::Translate2TransCAD() --#" << endl << endl;
	}

	void FeatureSOLIDCreatePatternCircular::CheckAttribute(string name, double value, int type)
	{
		if (name == "AngularRepartition")
		{
			if (type == 0)
				AngularNumber = (int)value;
			else if (type == 1)
				AngularSpacing = (double)value;
		}
		else if (name == "RadialRepartition")
		{
			if (type == 0)
				CircleNumber = (int)value;
			else if (type == 1)
				CircleSpacing = (double)value;
		}
	}

	void FeatureSOLIDCreatePatternCircular::Modify(char * buffer)  //Script 파일에 바로 Class 인스턴스 이름이 나올때 
	{
		string token;
		char * context = NULL;
		char seps[] = " ,\t\n().="; //구분자
		token = strtok_s(buffer, seps, &context);	//rectPattern1
		token = strtok_s(NULL, seps, &context);		//SetFirst Direction

		int _type;

		if (token == "SetRotationAxis")
		{
			token = strtok_s(NULL, seps, &context);		//reference11
			_rotationAxis = token;

			_type = _pPart->GetReferenceEntityByName(token)->GetReferenceType();
		}

		if (token == "RotationOrientation")
		{
			token = strtok_s(NULL, seps, &context);		//reference11

			if (token == "False")
			{
				DirX = (-1)*DirX;
				DirY = (-1)*DirY;
				DirZ = (-1)*DirZ;
			}
		}

		// 레퍼런스가 Plane 혹은 Face인 경우 
		if (_type == 1 || _type == 3 || _type == 4)
			_RefPlane = token;

		context = NULL;
	}

	void FeatureSOLIDCreatePatternCircular::SetDirection(string refplane)
	{
		string RefPlane = _pPart->GetReferenceEntityByName(refplane)->GetFeatureName();

		if (RefPlane == "PlaneXY")
		{
			DirZ = 1.0;
		}
		else if (RefPlane == "PlaneYZ")
		{
			DirX = 1.0;
		}
		else if (RefPlane == "PlaneZX")
		{
			DirY = 1.0;
		}
		else
		{
			if (RefPlane != "")
			{
				string RefPlane1 = _pPart->GetReferenceEntityByName(RefPlane)->GetFeatureName();
				if (RefPlane1 == "PlaneXY")
				{
					DirZ = 1.0;
				}
				else if (RefPlane1 == "PlaneYZ")
				{
					DirX = 1.0;
				}
				else if (RefPlane1 == "PlaneZX")
				{
					DirY = 1.0;
				}
			}
			else
			{
				string _revRefSketName = GetPart()->GetReferenceEntityByName(refplane)->GetFaceInfo()[0]._sketchName1;		// 스케치 이름
				string _revRefEntName = GetPart()->GetReferenceEntityByName(refplane)->GetFaceInfo()[0]._sketchItemName1;	// 스케치에 포함 된 라인. 즉, 회전 축

				// Circle을 Extrude한 원기둥인 경우
				FSketch * pRefSketch = (FSketch*)GetPart()->GetFeatureByName(_revRefSketName);
				Point3D origin, direction;
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

				CreateAxis(pRefSketch, 0, origin, direction);
				OrgX = origin.X(); OrgY = origin.Y(); OrgZ = origin.Z();
				DirX = direction.X(); DirY = direction.Y(); DirZ = direction.Z();
			}
		}
	}

	void FeatureSOLIDCreatePatternCircular::CreateAxis(FSketch * pRefSketch, FSketch * pRevSketch, Point3D origin, Point3D direction)
	{
		if (pRefSketch && (pRefSketch != pRevSketch))
		{
			TransformAxis(pRefSketch->sketchCoord, pRevSketch->sketchCoord, origin);
			TransformAxis(pRefSketch->sketchCoord, pRevSketch->sketchCoord, direction);
		}
	}

	void FeatureSOLIDCreatePatternCircular::TransformAxis(double refSketCSYS[9], double revSketCSYS[9], Point3D &coords)
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
		coords.SetX(coords.X() + refSketCSYS[0]);
		coords.SetY(coords.Y() + refSketCSYS[1]);
		coords.SetZ(coords.Z() + refSketCSYS[2]);
	}

	void FeatureSOLIDCreatePatternCircular::TransformCoordinates(double sourceCSYS[3][3], double targetCSYS[3][3], Point3D &coords)
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