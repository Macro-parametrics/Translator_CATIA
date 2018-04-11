#include "stdafx.h"

#include "FSketch.h"
#include "FSKETCHItem.h"
#include "FSKETCHCreate2DCircleCenterPoint.h"
#include "FSKETCHCreate2DLine2Points.h"
#include "FSKETCHCreate2DPoint.h"

#include "ReferenceEntity.h"

namespace Pre {
	FSketch::FSketch(Part * pPart, int type, string name)
		: Feature(pPart, type, name)
	{
	}

	FSketch::~FSketch(void)
	{
		_spSketEditor = NULL;
		_spSketch = NULL;

		Clear();
	}

	void FSketch::Clear()
	{
		std::vector<FSKETCHItem*>::iterator i = _sketchItemList.begin();
		while (i != _sketchItemList.end()) {
			delete *i;
			i++;
		}
		_sketchItemList.clear();
	}

	void FSketch::CreateSketchItem(string type, string name, ifstream &is)
	{
		FSKETCHItem * pSketchItem = 0;

		if (type == "Point2D") //Point2D : 0
			pSketchItem = new FSKETCHCreate2DPoint(this, 0, name);
		else if (type == "Line2D") //Line2D : 1
			pSketchItem = new FSKETCHCreate2DLine2Points(this, 1, name);
		else if (type == "Circle2D") //Circle2D : 2
			pSketchItem = new FSKETCHCreate2DCircleCenterPoint(this, 2, name);
		else if (type == "Factory2D")
			_factoryName = name;
		else if (type == "GeometricElement")
			_axisName = name;
		else if (type == "CATBaseDispatch")
		{
			if (name.find("point") == 0)
				pSketchItem = new FSKETCHCreate2DPoint(this, 0, name);
			else if (name.find("line") == 0)
				pSketchItem = new FSKETCHCreate2DLine2Points(this, 1, name);
		}

		if (pSketchItem)
		{
			pSketchItem->GetInfo(is);
			_sketchItemList.push_back(pSketchItem);
		}
	}

	void FSketch::GetInfo(ifstream & is)
	{
		char buffer[500];
		char seps_temp[] = " ()="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		string token, name, type;

		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set을 찾는 부분
		{
			is.getline(buffer, 500);
		}

		token = strtok_s(buffer, seps_temp, &context);
		token = strtok_s(NULL, seps_temp, &context);
		token = strtok_s(NULL, seps_temp, &context);

		if (token == "sketches1.Add")
		{
			token = strtok_s(NULL, seps_temp, &context);

			// 스케치가 놓일 reference 이름을 저장
			_pReferenceEntity = _pPart->GetReferenceEntityByName(token);

			// SketchManager의 코디네이트를 저장하는 부분
			// Dim arrayOfVariantOfDouble1(8)을 찾는 부분
			while (strncmp(buffer, "Dim", 3))
			{
				is.getline(buffer, 500);
			}

			for (int i = 0; i < 9; ++i)
			{
				is.getline(buffer, 500);

				token = strtok_s(buffer, seps_temp, &context);	//첫번째 단어
				token = strtok_s(NULL, seps_temp, &context);		//두번째 단어
				token = strtok_s(NULL, seps_temp, &context);		//세번째 단어

				sketchCoord[i] = atof(token.c_str());
			}

			// Sketch 정보를 불러오는 함수 호출
			GetSketchItemInfo(is);
		}

		context = NULL;
	}

	void FSketch::GetSketchItemInfo(ifstream & is)
	{
		char buffer[500] = "";
		char seps_temp[] = " ()="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		string token, name, type;

		string nameTemp = _name + ".CloseEdition"; //sketch1.CloseEdition의 글자수를 가져온다.
		int nameSize = nameTemp.size();

		//sketch1.CloseEdition이란 문장이 오기 전까지 엔터티를 읽어서 생성
		while (strncmp(buffer, nameTemp.c_str(), nameSize))
		{
			if (strlen(buffer) == 0)  // 빈 라인의 경우는 통과
			{
			}
			else if (!strncmp(buffer, "Dim", 3))  //첫번째 단어가 dim인지 검사하여 적당한 객체 생성
			{
				token = strtok_s(buffer, seps_temp, &context);//Dim
				token = strtok_s(NULL, seps_temp, &context);	//line2D4
				name = token; //객체의 이름 저장

				token = strtok_s(NULL, seps_temp, &context);	//As
				token = strtok_s(NULL, seps_temp, &context);	//Line2D
				type = token; //객체의 타입 저장

				CreateSketchItem(type, name, is); // 스케치 아이템 생성
			}
			else
			{
				//아래의 예제와 같이 Script 파일에 바로 Class 인스턴스 이름이 나올때 사용
				// line2D7.StartPoint = point2D5
				// line2D7.EndPoint = point2D6
				// line2D7.ReportName = 8
				// sketch1.CenterLine = line2D7

				char temp[500];
				char seps_temp2[] = " .()="; //구분자
				strcpy_s(temp, buffer);

				token = strtok_s(buffer, seps_temp2, &context);
				name = token;

				if (_name == name)  //sketch의 이름과 같은 경우
				{
					token = strtok_s(NULL, seps_temp2, &context);

					if (token == "CenterLine") //sketch1.CenterLine와 같은 경우 처리
					{
						token = strtok_s(NULL, seps_temp2, &context);
						GetSketchItemFromName(token)->SetCenterLineInfo(1); //해당 line을 센터라인으로 지정
					}
					else
					{
					}
				}
				else if (GetSketchItemFromName(name)) //SketchItem에 있는지 확인
				{
					GetSketchItemFromName(name)->Modify(temp);  //값을 수정
				}
			}

			context = NULL;

			is.getline(buffer, 500);  //다음 라인을 읽기

		} // end of while

		context = NULL;
	}

	void FSketch::ToTransCAD()
	{
		// Sketch가 놓일 레퍼런스 생성
		GetReferenceEntity()->ToTransCAD();

		// Add new StdSketchFeature to TransCAD
		_spSketch = GetPart()->_spFeatures->AddNewSketchFeature(GetFeatureName().c_str(), GetReferenceEntity()->GetReferencePtr());

		// Set Coordinate system of Sketch
		_spSketch->SetCoordinateSystem(sketchCoord[0], sketchCoord[1], sketchCoord[2], sketchCoord[3], sketchCoord[4], sketchCoord[5], sketchCoord[6], sketchCoord[7], sketchCoord[8]);

		// TransCAD Sketch Open
		_spSketEditor = _spSketch->OpenEditorEx(VARIANT_FALSE);

		// 스케치 요소 정보 TransCAD로 변환하기
		for (int i = 0; (unsigned int)i < GetSketchItemsSize(); ++i)
		{
			GetSketchItem(i)->ToTransCAD();
		}

		// TransCAD Sketch Close
		_spSketEditor->Close();
	}

	FSKETCHItem * FSketch::GetSketchItemFromName(string name)
	{
		vector<FSKETCHItem *>::iterator iter = _sketchItemList.begin();

		while (iter != _sketchItemList.end())
		{
			if (((*iter)->GetSketchItemName()) == name)
				return *iter;

			iter++;
		}

		return 0;
	}

	void FSketch::Modify(char * buffer)
	{

	}

	//SketchItem의 이름과, 해당 SketchItem의 ReportName을 저장하기 위한 용도. 
	void FSketch::AddEntityNameReportName(int reportName, string entityName)
	{
		ReportItem _repItem;
		_repItem._reportName = reportName;
		_repItem._entityName = entityName;

		reportNameTable.push_back(_repItem);
	}

	//ReportName을 입력하여 해당 SketchItem의 이름을 출력하기 위한 용도.
	string FSketch::GetEntityNameFromReportName(int reportName)
	{
		string entityName;
		for (int i = 0; (unsigned int)i < reportNameTable.size(); ++i)
		{
			if (reportNameTable[i]._reportName == reportName)
				entityName = reportNameTable[i]._entityName;
		}

		return entityName;
	}

}