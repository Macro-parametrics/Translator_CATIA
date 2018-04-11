#include "stdafx.h"
#include "FSKETCHCreate2DCircleCenterPoint.h"
#include "FSketch.h"

namespace Pre {
	FSKETCHCreate2DCircleCenterPoint::FSKETCHCreate2DCircleCenterPoint(FSketch * pFSketch, int type, string name)
		: FSKETCHItem(pFSketch, type, name)
	{
	}

	FSKETCHCreate2DCircleCenterPoint::~FSKETCHCreate2DCircleCenterPoint(void)
	{
	}

	void FSKETCHCreate2DCircleCenterPoint::GetInfo(ifstream & is)
	{
		char buffer[500];
		is.getline(buffer, 500);
		while (strncmp(buffer, "Set", 3))  // Set point2D1을 찾는 부분
		{
			is.getline(buffer, 500);
		}

		// 두개의 시작점과 끝점의 데이터를 읽는 부분
		string coordinate;
		char seps_temp[] = " ,\t\n()="; //구분자에 .을 추가하면 안된다. 소숫점 아래의 값에서 잘리지 않게 하기 위함.
		char * context = NULL;			// strtok_s 함수의 입력 변수

		coordinate = strtok_s(buffer, seps_temp, &context); //첫번째 단어 Set
		coordinate = strtok_s(NULL, seps_temp, &context);  //두번째 단어 circle2D1
		coordinate = strtok_s(NULL, seps_temp, &context);  //세번째 단어 factory2D2.CreateClosedCircle

		string temp1 = GetFeatureSketch()->GetFactoryName() + ".CreateClosedCircle";
		string temp2 = GetFeatureSketch()->GetFactoryName() + ".CreateCircle";

		if (coordinate == temp1) // CreateClosedCircle 인 경우
		{
			circleType = 1;

			coordinate = strtok_s(NULL, seps_temp, &context);  //center의 x
			center[0] = atof(coordinate.c_str());
			coordinate = strtok_s(NULL, seps_temp, &context);  //center의 y
			center[1] = atof(coordinate.c_str());
			center[2] = 0;										  //center의 z

			coordinate = strtok_s(NULL, seps_temp, &context);  //point2의 x
			_radius = atof(coordinate.c_str());
		}
		else if (coordinate == temp2) // CreateCircle인 경우
		{
			circleType = 2;

			coordinate = strtok_s(NULL, seps_temp, &context);  //center의 x
			center[0] = atof(coordinate.c_str());
			coordinate = strtok_s(NULL, seps_temp, &context);  //center의 y
			center[1] = atof(coordinate.c_str());
			center[2] = 0;										  //center의 z

			coordinate = strtok_s(NULL, seps_temp, &context);  //radius 20.000000
			_radius = atof(coordinate.c_str());

			coordinate = strtok_s(NULL, seps_temp, &context);  //start angle:0.000000
			startAngle = atof(coordinate.c_str());
			coordinate = strtok_s(NULL, seps_temp, &context);  //end angle : 1.570796
			endAngle = atof(coordinate.c_str());
		}
	}

	void FSKETCHCreate2DCircleCenterPoint::ToTransCAD()
	{
		// Get SketchEditorPtr
		TransCAD::ISketchEditorPtr spSketchEditor = GetFeatureSketch()->GetSketchEditorPtr();

		if (circleType == 1)
		{
			// Create Create2DCircleCenterPoint
			spSketchEditor->Create2DCircleCenterPoint(_name.c_str(), center[0], center[1], _radius);
		}
		else if (circleType == 2)
		{
			ConvertRadToDeg(startAngle, endAngle);
			// Create Create2DArc
			spSketchEditor->Create2DArcCenterStartEnd(_name.c_str(), center[0], center[1], startPoint[0], startPoint[1], endPoint[0], endPoint[1]);
		}
	}

	void FSKETCHCreate2DCircleCenterPoint::Modify(char * buffer)
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
		else if (token == "CenterPoint")
		{
			token = strtok_s(NULL, seps, &context); //point2D3
			_center = token.c_str();
		}
		else if (token == "ReportName")
		{
			token = strtok_s(NULL, seps, &context); //point2D3
			_id = atoi(token.c_str());
			GetFeatureSketch()->AddEntityNameReportName(_id, _name);
		}
		else
		{
		}
	}

	void  FSKETCHCreate2DCircleCenterPoint::ConvertRadToDeg(double x, double y)
	{
		// 소숫점 처리
		double digit = pow(10.0, 3);
		startPoint[2], endPoint[2];

		startPoint[0] = center[0] + _radius*cos(x);
		startPoint[1] = center[1] + _radius*sin(x);
		startPoint[2] = 0.0;
		endPoint[0] = center[0] + _radius*cos(y);
		endPoint[1] = center[1] + _radius*sin(y);
		endPoint[2] = 0.0;

		double spRest, epRest;

		for (int i = 0; i < 2; ++i)
		{
			startPoint[i] = startPoint[i] * digit;
			endPoint[i] = endPoint[i] * digit;

			spRest = startPoint[i] - floor(startPoint[i]);
			epRest = endPoint[i] - floor(endPoint[i]);

			if (spRest >= 0.5)
				startPoint[i] = ceil(startPoint[i]);
			else
				startPoint[i] = floor(startPoint[i]);

			if (epRest >= 0.5)
				endPoint[i] = ceil(endPoint[i]);
			else
				endPoint[i] = floor(endPoint[i]);

			startPoint[i] /= digit;
			endPoint[i] /= digit;
		}


		//_startAngle = x*180.0/PI;
		//_endAngle = y*180.0/PI;

		//double sArest, eArest;
		//sArest = _startAngle - floor(_startAngle);
		//eArest = _endAngle - floor(_endAngle);


		//if ( sArest > 0.5 )
		//	_startAngle = ceil(_startAngle);
		//else
		//	_startAngle = floor(_startAngle);


		//if ( eArest > 0.5 )
		//	_endAngle = ceil(_endAngle);
		//else
		//	_endAngle = floor(_endAngle);


	}
}