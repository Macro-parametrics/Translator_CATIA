#include "stdafx.h"
#include "ReferenceEntity.h"

namespace Pre {
	ReferenceEntity::ReferenceEntity(Part * pPart, int _type, string _name)
	{
		_pPart = pPart;
		name = _name;
		type = _type;

		pSketchManager = 0; // 스케치 매니저 초기화. 스케치 엘리먼트 검색 시 활용
	}

	ReferenceEntity::~ReferenceEntity()
	{
	}

	void ReferenceEntity::GetInfo(ifstream & is)
	{
		// ReferenceEntity의 정보를 추출하는 함수
		char buffer[1000];

		f_Getline_Custom(is, buffer, 500)
		while (strncmp(buffer, "Set", 3))
		{
			f_Getline_Custom(is, buffer, 500)
		}

		string referenceValue;
		char seps_temp[] = " :;,\t\n()=."; //구분자
		char * context = NULL;

		referenceValue = strtok_s(buffer, seps_temp, &context);	//첫번째 단어
		referenceValue = strtok_s(NULL, seps_temp, &context);	//두번째 
		referenceValue = strtok_s(NULL, seps_temp, &context);	//세번째 

		// Default DatumPlane인 경우
		if (referenceValue == "originElements1")
		{
			referenceValue = strtok_s(NULL, seps_temp, &context);	//네번째

			if (referenceValue == "PlaneXY" || referenceValue == "PlaneYZ" || referenceValue == "PlaneZX")
			{
				type = 1;
				feature = referenceValue;
			}
		}
		// DatumPlaneOffset 인 경우
		else if (referenceValue.find("hybridShapes") == 0)
		{
			referenceValue = strtok_s(NULL, seps_temp, &context);	//네번째 

			if (referenceValue == "Item")
			{
				char seps_temp2[] = " \",\t\n()=."; //구분자
				referenceValue = strtok_s(NULL, seps_temp2, &context); //Plane

				if (!strncmp(referenceValue.c_str(), "Plane", 5))
				{
					referenceValue = strtok_s(NULL, seps_temp2, &context);  //1
					referenceValue = "Plane." + referenceValue;

					type = 4;
					feature = referenceValue;
				}
			}
		}
		// 스케치의 Local axis인 경우
		else if (referenceValue.find("axis2D") == 0)
		{
			pWire._sketchName = referenceValue;

			referenceValue = strtok_s(NULL, seps_temp, &context);	//네번째

			// VDirection 또는 HDirection인 경우
			if (referenceValue == "GetItem")
			{
				char seps_temp2[] = " \",\t\n()=."; //구분자
				referenceValue = strtok_s(NULL, seps_temp2, &context); //Plane
				pWire._sketchItemName = referenceValue;
			}
		}
		// 특징형상의 B-rep 요소를 선택하거나, Datum Plane 객체를 선택하는 경우
		else if (referenceValue == "part1")
		{
			referenceValue = strtok_s(NULL, seps_temp, &context);	//네번째

			if (referenceValue == "CreateReferenceFromName")
			{
				type = 3;  //Surface
				referenceValue = strtok_s(NULL, seps_temp, &context);

				if (referenceValue == "\"Selection_RSur")
				{
					TransCADFaceName = GetFullFaceName(context);
				}
				else  // 아무것도 참조하지 않을 경우
				{
					type = 0;
				}
			}
			else if (referenceValue == "CreateReferenceFromBRepName")
			{
				referenceValue = strtok_s(NULL, seps_temp, &context);

				if (referenceValue == "\"FSur" || referenceValue == "\"RSur")
				{
					type = 3; //Surface
					TransCADFaceName = GetFullFaceName(context);
				}
				else if (referenceValue == "\"REdge" || referenceValue == "\"TgtEdge" || referenceValue == "\"TgtIntersEdge")
				{
					type = 2; //Edge
					while (referenceValue != "Edge")
					{
						referenceValue = strtok_s(NULL, seps_temp, &context); //Edge
					}

					TransCADEdgeName = GetFullFaceName(context); // 첫 번째 인접 Face 이름 추가

					referenceValue = strtok_s(NULL, seps_temp, &context); //Face

					if (TransCADEdgeName.find(":0;0") != -1)
						referenceValue = strtok_s(NULL, seps_temp, &context); //Brep

					TransCADEdgeName = TransCADEdgeName + "#" + GetFullFaceName(context); // 두 번째 인접 Face 이름 추가
				}
				else if (referenceValue == "\"WireREdge" || referenceValue == "\"Axis")
				{
					type = 2; //WireEdge

					referenceValue = strtok_s(NULL, seps_temp, &context);
					referenceValue = strtok_s(NULL, seps_temp, &context);
					referenceValue = strtok_s(NULL, seps_temp, &context);

					if (referenceValue == "Sketch")
					{
						referenceValue = strtok_s(NULL, seps_temp, &context);

						pWire._sketchName = "Sketch" + referenceValue; // 스케치 이름

						referenceValue = strtok_s(NULL, seps_temp, &context);
						int edgeNumber = atoi(referenceValue.c_str());

						pSketchManager = (FSketch*)_pPart->GetFeatureByName(pWire._sketchName);

						pWire._sketchItemName = pSketchManager->GetEntityNameFromReportName(edgeNumber); // 스케치 요소 이름
					}
					else
					{
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);
						referenceValue = strtok_s(NULL, seps_temp, &context);

						pWire._sketchName = "Sketch" + referenceValue; // 스케치 이름

						referenceValue = strtok_s(NULL, seps_temp, &context);
						int edgeNumber = atoi(referenceValue.c_str());

						pSketchManager = (FSketch*)_pPart->GetFeatureByName(pWire._sketchName);

						pWire._sketchItemName = pSketchManager->GetEntityNameFromReportName(edgeNumber); // 스케치 요소 이름
					}

					pSketchManager = 0;
				}
				else if (referenceValue == "\"WireFVertex")
				{
					type = 5; // Point
					// Loft의 start point 정보를 관리하는데 사용
					// 향 후 추가
				}
				else
				{
				}
			}
			else if (referenceValue == "CreateReferenceFromObject")
			{
				//기본은 type이 4라고 가정하고 DatumPoint 처리 루틴을 추가
				//Loft에서 참조하는 Sketch 면을 나타내는 경우
				//참조하는 객체가 Factory2D 즉 sketch element의 경우

				referenceValue = strtok_s(NULL, seps_temp, &context); //factory2D2 or sketch5

				type = 4;  //object
				feature = referenceValue;
			}
		}

		context = NULL;
	}

	void ReferenceEntity::ToTransCAD()
	{
		spReferencePtr = NULL;

		// 1=Default datum plane, 2=Edge, 3=Surface, 4=Object, 5=Point  
		if (type == 1)
		{
			if (feature == "PlaneXY")
				spReferencePtr = _pPart->_spPart->SelectObjectByName("XYPlane");
			else if (feature == "PlaneYZ")
				spReferencePtr = _pPart->_spPart->SelectObjectByName("YZPlane");
			else if (feature == "PlaneZX")
				spReferencePtr = _pPart->_spPart->SelectObjectByName("ZXPlane");
		}
		else if (type == 2)
		{
			spReferencePtr = _pPart->_spPart->SelectBrepByName(TransCADEdgeName.c_str());
		}
		else if (type == 3)
		{
			spReferencePtr = _pPart->_spPart->SelectBrepByName(TransCADFaceName.c_str());
		}
		else if (type == 4)
		{
			// DatumPlaneOffset을 Reference로 잡는 경우,
			// ex) hybridShapePlaneOffset1 → Plane.1
			string temp = feature;

			if (!strncmp(feature.c_str(), "hybridShapePlaneOffset", 22))
			{
				int index = 0;
				char refNum[1024], refName[1024];

				for (int i = 0; i < (int)feature.size(); ++i)
				{
					if (feature[i] >= '0' && feature[i] <= '9' && feature[i] != '.')
						refNum[index++] = feature[i];
				}

				int refNumber = atoi(refNum);
				sprintf_s(refName, "Plane.%d", refNumber);
				temp = refName;

				spReferencePtr = _pPart->_spPart->SelectObjectByName(temp.c_str());
			}
			else // Plane.1
				spReferencePtr = _pPart->_spPart->SelectObjectByName(temp.c_str());
		}
		else if (type == 5)
		{
		}
	}

	// CATIA의 FullFaceName을 통해 TransCAD의 FullFaceName을 리턴하는 함수
	string ReferenceEntity::GetFullFaceName(char *& buffer)
	{
		string referenceValue;
		string _fullFaceName;
		char seps_temp[] = " :;,\t\n()=."; //구분자

		referenceValue = strtok_s(NULL, seps_temp, &buffer);
		referenceValue = strtok_s(NULL, seps_temp, &buffer);
		string temp_buff = buffer;

		referenceValue = strtok_s(NULL, seps_temp, &buffer);

		// 면이 Merge 되어 있는 경우 (Secondary Name이 있음)
		if (referenceValue == "Brp")
		{
			vector<string> _faceNameList;

			_faceNameList.push_back(GetBasicFaceName(buffer));

			referenceValue = strtok_s(NULL, seps_temp, &buffer);

			while (referenceValue == "Brp")
			{
				_faceNameList.push_back(GetBasicFaceName(buffer));

				referenceValue = strtok_s(NULL, seps_temp, &buffer);
			}

			// The number of merged faces
			stringstream mergedFaceNum;
			mergedFaceNum << _faceNameList.size() - 1;

			for (int i = _faceNameList.size() - 1; i >= 0; --i)
			{
				if (i == _faceNameList.size() - 1)
					_fullFaceName = _faceNameList[i] + ":0,0:" + mergedFaceNum.str();
				else
					_fullFaceName = _fullFaceName + ";" + _faceNameList[i];
			}
		}
		else // Merge 된 면이 없는 경우
		{
			buffer = new char[temp_buff.length() + 1];

			strcpy_s(buffer, temp_buff.length() + 1, temp_buff.c_str());
			string _basicFaceName = GetBasicFaceName(buffer);

			_fullFaceName = _basicFaceName
				+ ":0,0"	// Split Face Information
				+ ":0;0";	// Merge Face Information
		}

		if (type == 2) //Edge 정보 저장
		{
			pEdge.push_back(pFace);
			pFace.clear();
		}

		return _fullFaceName;
	}

	// CATIA의 BasicFaceName을 통해 TransCAD의 BasicFaceName을 리턴하는 함수
	string ReferenceEntity::GetBasicFaceName(char *& buffer)
	{
		string referenceValue;
		char seps_temp[] = " :;,\t\n()=."; //구분자

		FaceInfo _pFace; //면 정보를 임시로 저장

		referenceValue = strtok_s(NULL, seps_temp, &buffer);

		// Shell의 경우 TransCAD아직 구현 안됨.
		// 네이밍 스키마도 없는 상태. (16-01-18)
		if (referenceValue == "Shell")
		{
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
		}

		if (referenceValue == "Pad")
		{
			_pFace._featureName = "pad";
			_pFace._featureType = "ExtrudeFeature";
		}
		else if (referenceValue == "Pocket")
		{
			_pFace._featureName = "pocket";
			_pFace._featureType = "ExtrudeFeature";
		}
		else if (referenceValue == "Shaft")
		{
			_pFace._featureName = "shaft";
			_pFace._featureType = "RevolveFeature";
		}
		else if (referenceValue == "Groove")
		{
			_pFace._featureName = "groove";
			_pFace._featureType = "RevolveFeature";
		}
		else if (referenceValue == "Rib")
		{
			_pFace._featureName = "rib";
			_pFace._featureType = "SweepFeature";
		}
		else if (referenceValue == "Slot")
		{
			_pFace._featureName = "slot";
			_pFace._featureType = "SweepFeature";
		}

		referenceValue = strtok_s(NULL, seps_temp, &buffer); // Feature 번호
		_pFace._featureName = _pFace._featureName + referenceValue;

		referenceValue = strtok_s(NULL, seps_temp, &buffer);
		int _faceType = atoi(referenceValue.c_str());

		_pFace._sketchName2 = "0";
		_pFace._sketchItemName2 = "0";

		if (_faceType == 0) // 0:옆면, 1:아랫면, 2:윗면
		{
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			_pFace._sketchName1 = "Sketch" + referenceValue;

			referenceValue = strtok_s(NULL, seps_temp, &buffer);
			int _sketchItemNum1 = atoi(referenceValue.c_str());

			pSketchManager = (FSketch*)_pPart->GetFeatureByName(_pFace._sketchName1);
			_pFace._sketchItemName1 = pSketchManager->GetEntityNameFromReportName(_sketchItemNum1);

			// rib이 아니고 sketch 정보가 2개 필요한 경우에도 처리 필요
			// 현재는 rib의 경우만 처리
			if (_pFace._featureType == "SweepFeature")
			{
				referenceValue = strtok_s(NULL, seps_temp, &buffer);

				if (referenceValue != "None")
				{
					referenceValue = strtok_s(NULL, seps_temp, &buffer);
					referenceValue = strtok_s(NULL, seps_temp, &buffer);
					_pFace._sketchName2 = "Sketch" + referenceValue;

					referenceValue = strtok_s(NULL, seps_temp, &buffer);
					int _sketchItemNum2 = atoi(referenceValue.c_str());

					pSketchManager = (FSketch*)_pPart->GetFeatureByName(_pFace._sketchName2);
					_pFace._sketchItemName2 = pSketchManager->GetEntityNameFromReportName(_sketchItemNum2);
				}
			}
		}
		else if (_faceType == 1)
		{
			_faceType = -1;
			_pFace._sketchName1 = "0";
			_pFace._sketchItemName1 = "-1";
		}
		else if (_faceType == 2)
		{
			_faceType = -2;
			_pFace._sketchName1 = "0";
			_pFace._sketchItemName1 = "-2";
		}

		// 최종 basic face name
		string _basicFaceName = _pFace._featureName + ","
			+ _pFace._sketchName1 + ","
			+ _pFace._sketchItemName1 + ","
			+ _pFace._sketchName2 + ","
			+ _pFace._sketchItemName2 + ","
			+ "0,"
			+ _pFace._featureType;


		pFace.push_back(_pFace);

		return _basicFaceName;
	}

	//Assembly용
	/*
		전부 다 적용한 것은 아니고 ReferenceFromName, ReferenceFromBrepName 에 대한 것만 적용함
	*/
	string ReferenceEntity::GetReferName(char *& context) {
	
		
		string referenceValue;
		char seps_temp[] = " :;,\t\n()=."; //구분자
		//char * context = NULL;

		string s;

#pragma region GetInfo  부분 추출

		referenceValue = strtok_s(NULL, seps_temp, &context);

		//CreateReferenceFromName*******************************/
		if (referenceValue == "\"Selection_RSur")
		{
			type = 3;  //Surface
			TransCADFaceName = GetFullFaceName(context);
		}
		
		//CreateReferenceFromBRepName*******************************/
		else if (referenceValue == "\"FSur" || referenceValue == "\"RSur")
		{
			type = 3; //Surface
			TransCADFaceName = GetFullFaceName(context);
		}
		else if (referenceValue == "\"REdge" || referenceValue == "\"TgtEdge" || referenceValue == "\"TgtIntersEdge")
		{
			type = 2; //Edge
			while (referenceValue != "Edge")
			{
				referenceValue = strtok_s(NULL, seps_temp, &context); //Edge
			}

			TransCADEdgeName = GetFullFaceName(context); // 첫 번째 인접 Face 이름 추가

			referenceValue = strtok_s(NULL, seps_temp, &context); //Face

			if (TransCADEdgeName.find(":0;0") != -1)
				referenceValue = strtok_s(NULL, seps_temp, &context); //Brep

			TransCADEdgeName = TransCADEdgeName + "#" + GetFullFaceName(context); // 두 번째 인접 Face 이름 추가
		}
		else if (referenceValue == "\"WireREdge" || referenceValue == "\"Axis")
		{
			type = 2; //WireEdge

			referenceValue = strtok_s(NULL, seps_temp, &context);
			referenceValue = strtok_s(NULL, seps_temp, &context);
			referenceValue = strtok_s(NULL, seps_temp, &context);

			if (referenceValue == "Sketch")
			{
				referenceValue = strtok_s(NULL, seps_temp, &context);

				pWire._sketchName = "Sketch" + referenceValue; // 스케치 이름

				referenceValue = strtok_s(NULL, seps_temp, &context);
				int edgeNumber = atoi(referenceValue.c_str());

				pSketchManager = (FSketch*)_pPart->GetFeatureByName(pWire._sketchName);

				pWire._sketchItemName = pSketchManager->GetEntityNameFromReportName(edgeNumber); // 스케치 요소 이름
			}
			else
			{
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);
				referenceValue = strtok_s(NULL, seps_temp, &context);

				pWire._sketchName = "Sketch" + referenceValue; // 스케치 이름

				referenceValue = strtok_s(NULL, seps_temp, &context);
				int edgeNumber = atoi(referenceValue.c_str());

				pSketchManager = (FSketch*)_pPart->GetFeatureByName(pWire._sketchName);

				pWire._sketchItemName = pSketchManager->GetEntityNameFromReportName(edgeNumber); // 스케치 요소 이름
			}

			pSketchManager = 0;
		}
		else if (referenceValue == "\"WireFVertex")
		{
			type = 5; // Point
					  // Loft의 start point 정보를 관리하는데 사용
					  // 향 후 추가
		}
		else
		{	// 아무것도 참조하지 않을 경우
			type = 0;

		}

#pragma endregion

#pragma region ToTransCAD  부분 추출
		// 1=Default datum plane, 2=Edge, 3=Surface, 4=Object, 5=Point  
		if (type == 1)
		{
			if (feature == "PlaneXY")
				s = "XYPlane";
			else if (feature == "PlaneYZ")
				s = "YZPlane";
			else if (feature == "PlaneZX")
				s = "ZXPlane";
		}
		else if (type == 2)
		{
			s = TransCADEdgeName.c_str();
		}
		else if (type == 3)
		{
			s = TransCADFaceName.c_str();
		}
		else if (type == 4)
		{
			// DatumPlaneOffset을 Reference로 잡는 경우,
			// ex) hybridShapePlaneOffset1 → Plane.1
			string temp = feature;

			if (!strncmp(feature.c_str(), "hybridShapePlaneOffset", 22))
			{
				int index = 0;
				char refNum[1024], refName[1024];

				for (int i = 0; i < (int)feature.size(); ++i)
				{
					if (feature[i] >= '0' && feature[i] <= '9' && feature[i] != '.')
						refNum[index++] = feature[i];
				}

				int refNumber = atoi(refNum);
				sprintf_s(refName, "Plane.%d", refNumber);
				temp = refName;

				s = temp.c_str();
			}
			else // Plane.1
				s = temp.c_str();
		}
		else if (type == 5)
		{
		}

#pragma endregion

		return s;
	}
}