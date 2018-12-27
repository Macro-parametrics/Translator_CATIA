#include "stdafx.h"
#include "FeatureSOLIDCreatePatternRectangular.h"
#include "ReferenceEntity.h"

namespace Pre {
	FeatureSOLIDCreatePatternRectangular::FeatureSOLIDCreatePatternRectangular(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
		ColumnSpacing = 0;
		ColumnNumber = 0;
		ColDirX = 0;
		ColDirY = 0;
		ColDirZ = 0;

		RowSpacing = 0;
		RowNumber = 0;
		RowDirX = 0;
		RowDirY = 0;
		RowDirZ = 0;
	}

	FeatureSOLIDCreatePatternRectangular::~FeatureSOLIDCreatePatternRectangular()
	{
	}

	void FeatureSOLIDCreatePatternRectangular::GetInfo(ifstream &is)
	{
		// Chamfer의 정보 입력
		char buffer[400];
		char * context = NULL;
		f_Getline_Custom(is, buffer, 500)

		while (strncmp(buffer, "Set", 3))  // Set chamfer1을 찾는 부분
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string rectpatternValue;
		char seps_temp[] = " ,\t\n()="; //구분자
		rectpatternValue = strtok_s(buffer, seps_temp, &context);//첫번째 단어
		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //세번째 단어

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //네번째 단어 

		_spTargetFeature = rectpatternValue.c_str();				// Pattern이 적용 될 Target Feature

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //ColumnNumber
		ColumnNumber = atoi(rectpatternValue.c_str());

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //RowNumber
		RowNumber = atoi(rectpatternValue.c_str());

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //ColumnSpacing
		ColumnSpacing = atoi(rectpatternValue.c_str());

		rectpatternValue = strtok_s(NULL, seps_temp, &context);  //RowSpacing
		RowSpacing = atof(rectpatternValue.c_str());

		context = NULL;
	}

	void FeatureSOLIDCreatePatternRectangular::ToTransCAD()
	{
		cerr << "#-- Call RectPattern::Translate2TransCAD() --#" << endl;

		cerr << "RectPattern Name:" << _name.c_str() << endl;

		// 패턴 방향 설정
		SetDirection(ColumnSpacing, RowSpacing, _RefPlane);

		TransCAD::IReferencePtr spTargetFeature = GetPart()->_spPart->SelectFeatureByName(_spTargetFeature);
		TransCAD::IReferencesPtr spTargetFeatures = GetPart()->_spPart->CreateReferences();
		spTargetFeatures->Add(spTargetFeature);

		GetPart()->_spFeatures->AddNewSolidOperatePatternRectangularFeature(_name.c_str(), spTargetFeatures, ColumnSpacing, ColDirX, ColDirY, ColDirZ, ColumnNumber,
			RowSpacing, RowDirX, RowDirY, RowDirZ, RowNumber);

		cerr << "#-- End RectPattern::Translate2TransCAD() --#" << endl << endl;
	}

	void FeatureSOLIDCreatePatternRectangular::Modify(char * buffer)  //Script 파일에 바로 Class 인스턴스 이름이 나올때 
	{
		string token;
		char * context = NULL;
		char seps[] = " ,\t\n().="; //구분자
		token = strtok_s(buffer, seps, &context);	//rectPattern1
		token = strtok_s(NULL, seps, &context);		//SetFirstDirection

		int _type;

		if (token == "SetFirstDirection")
		{
			token = strtok_s(NULL, seps, &context);		//reference11
			_firstDirection = token;

			_type = _pPart->GetReferenceEntityByName(token)->GetReferenceType();
		}
		else if (token == "SetSecondDirection")
		{
			token = strtok_s(NULL, seps, &context);		//reference11
			_secondDirection = token;

			_type = _pPart->GetReferenceEntityByName(token)->GetReferenceType();
		}
		else if (token == "FirstOrientation")
		{
			token = strtok_s(NULL, seps, &context);		//reference11

			if (token == "False")
				ColumnSpacing = (-1)*ColumnSpacing;

		}
		else if (token == "SecondOrientation")
		{
			token = strtok_s(NULL, seps, &context);		//reference11

			if (token == "False")
				RowSpacing = (-1)*RowSpacing;
		}

		// 레퍼런스가 평면 (Plane 혹은 Face)인 경우 진입
		// 평면이면 두 방향이 반드시 같은 평면을 참조한다.
		if (_type == 1 || _type == 3 || _type == 4)
			_RefPlane = token;

		context = NULL;
	}

	void FeatureSOLIDCreatePatternRectangular::CheckAttribute(string name, double value, int type)
	{
		if (name == "FirstDirectionRepartition")
		{
			if (type == 0)
				ColumnNumber = (int)value;
			else if (type == 1)
				ColumnSpacing = (double)value;
		}
		else if (name == "SecondDirectionRepartition")
		{
			if (type == 0)
				RowNumber = (int)value;
			else if (type == 1)
				RowSpacing = (double)value;
		}
	}

	void FeatureSOLIDCreatePatternRectangular::SetDirection(double colspacing, double rowspacing, string refplane)
	{
		// RefPlane값을 바탕으로 ColDir, RowDir의 방향을 확정. 
		// 예를들어 PlaneXY일 경우 ColDirX = 1, RowDirY = 1이 됨.
		// 또한, 입력받은 Spacing 값이 음수 일 경우 Direction에 적용
		string RefPlane = _pPart->GetReferenceEntityByName(refplane)->GetFeatureName();
		if (RefPlane == "PlaneXY")
		{
			ColDirX = 1;
			RowDirY = 1;

			if (colspacing < 0)
				ColDirX = ColDirX * (-1);
			if (rowspacing < 0)
				RowDirY = RowDirY * (-1);
		}
		else if (RefPlane == "PlaneYZ")
		{
			ColDirY = 1;
			RowDirZ = 1;

			if (colspacing < 0)
				ColDirY = ColDirY * (-1);
			if (rowspacing < 0)
				RowDirZ = RowDirZ * (-1);
		}
		else if (RefPlane == "PlaneZX")
		{
			ColDirZ = 1;
			RowDirX = 1;

			if (colspacing < 0)
				ColDirZ = ColDirZ * (-1);
			if (rowspacing < 0)
				RowDirX = RowDirX * (-1);
		}
		else
		{	// 현재는 DatumPlaneOffset의 경우만 처리
			// 향 후, Face와 Edge의 경우에도 처리해야 함
			string RefPlane1 = _pPart->GetReferenceEntityByName(RefPlane)->GetFeatureName();
			if (RefPlane1 == "PlaneXY")
			{
				ColDirX = 1;
				RowDirY = 1;

				if (colspacing < 0)
					ColDirX = ColDirX * (-1);
				if (rowspacing < 0)
					RowDirY = RowDirY * (-1);
			}
			else if (RefPlane1 == "PlaneYZ")
			{
				ColDirY = 1;
				RowDirZ = 1;

				if (colspacing < 0)
					ColDirY = ColDirY * (-1);
				if (rowspacing < 0)
					RowDirZ = RowDirZ * (-1);
			}
			else if (RefPlane1 == "PlaneZX")
			{
				ColDirZ = 1;
				RowDirX = 1;

				if (colspacing < 0)
					ColDirZ = ColDirZ * (-1);
				if (rowspacing < 0)
					RowDirX = RowDirX * (-1);
			}
		}

		// Spacing(간격)은 절댓값이기 때문에 절댓값 적용
		ColumnSpacing = abs(ColumnSpacing);
		RowSpacing = abs(RowSpacing);
	}





}