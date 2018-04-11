#include "stdafx.h"
#include "FSKETCHCreate2DLine2Points.h"
#include "FSketch.h"

namespace Pre {
	FSKETCHCreate2DLine2Points::FSKETCHCreate2DLine2Points(FSketch * pFSketch, int type, string name)
		: FSKETCHItem(pFSketch, type, name)
	{
		centerLineNum = 0;
		_isLocalAxis = false;
	}

	FSKETCHCreate2DLine2Points::~FSKETCHCreate2DLine2Points(void)
	{
	}

	void FSKETCHCreate2DLine2Points::GetInfo(ifstream & is)
	{
		char buffer[500];
		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set point2D1을 찾는 부분
		{
			is.getline(buffer, 500);
		}

		// 두개의 시작점과 끝점의 데이터를 읽는 부분
		string coordinate;
		char seps_temp[] = " ,\"\t\n()="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		coordinate = strtok_s(buffer, seps_temp, &context); //첫번째 단어
		coordinate = strtok_s(NULL, seps_temp, &context);  //두번째 단어
		coordinate = strtok_s(NULL, seps_temp, &context);  //세번째 단어

		coordinate = strtok_s(NULL, seps_temp, &context);  //Point1의 x

		if (coordinate == "HDirection" || coordinate == "VDirection")
		{
			startPoint[0] = 0.0;
			startPoint[1] = 0.0;
			startPoint[2] = 0.0;

			if (coordinate == "HDirection")
			{
				//endPoint[0] = GetFeatureSketch()->sketchCoord[3];
				//endPoint[1] = GetFeatureSketch()->sketchCoord[4];
				//endPoint[2] = GetFeatureSketch()->sketchCoord[5];
			}
			else
			{
				//endPoint[0] = GetFeatureSketch()->sketchCoord[6];
				//endPoint[1] = GetFeatureSketch()->sketchCoord[7];
				//endPoint[2] = GetFeatureSketch()->sketchCoord[8];

			}

			_isLocalAxis = true;
		}
		else //일반 Line 의 경우
		{
			startPoint[0] = atof(coordinate.c_str());
			coordinate = strtok_s(NULL, seps_temp, &context);  //Point1의 y
			startPoint[1] = atof(coordinate.c_str());
			startPoint[2] = 0;							//Point1의 z

			coordinate = strtok_s(NULL, seps_temp, &context);  //point2의 x
			endPoint[0] = atof(coordinate.c_str());
			coordinate = strtok_s(NULL, seps_temp, &context);  //Point2의 y
			endPoint[1] = atof(coordinate.c_str());
			endPoint[2] = 0;							//Point2의 z
		}
	}

	void FSKETCHCreate2DLine2Points::ToTransCAD()
	{
		// TransCADAPI - Get SketchEditorPtr

		if (_centerLineInfo == 0) // 일반 Line 인 경우
		{
			// TransCADAPI - Create sketch lines
			if (!_isLocalAxis)
			{
				TransCAD::ISketchEditorPtr spSketchEditor = GetFeatureSketch()->GetSketchEditorPtr();
				spSketchEditor->Create2DLine2Points(_name.c_str(), startPoint[0], startPoint[1], endPoint[0], endPoint[1]);
			}
		}
		else if (_centerLineInfo == 1) // CenterLine 인 경우
		{
			char centerLineName[1024];
			sprintf_s(centerLineName, "%s.%d", "CenterLine", centerLineNum);

			// TransCADAPI - Create sketch center lines
			TransCAD::ISketchEditorPtr spSketchEditor = GetFeatureSketch()->GetSketchEditorPtr();
			spSketchEditor->Create2DCenterline2Points(centerLineName, startPoint[0], startPoint[1], endPoint[0], endPoint[1]);
			centerLineNum++;
		}
	}

	void FSKETCHCreate2DLine2Points::Modify(char * buffer)
	{
		string token;
		char seps[] = " ,\t\n().="; //구분자
		char * context = NULL;			// strtok_s 함수의 입력 변수

		token = strtok_s(buffer, seps, &context);  //line2D7
		token = strtok_s(NULL, seps, &context); //StartPoint

		if (token == "StartPoint")
		{
			token = strtok_s(NULL, seps, &context); //point2D2
			_startPoint = token.c_str();
		}
		else if (token == "EndPoint")
		{
			token = strtok_s(NULL, seps, &context); //point2D3
			_endPoint = token.c_str();
		}
		else if (token == "ReportName")
		{
			token = strtok_s(NULL, seps, &context); //point2D3

			_id = atoi(token.c_str());
			GetFeatureSketch()->AddEntityNameReportName(_id, _name);
		}

	}
}