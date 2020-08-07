#include "stdafx.h"
#include "CATIAReferenceManager.h"

namespace Post {
	CATIAReferenceManager::CATIAReferenceManager(void)
	{
		_id = 0;
	}

	CATIAReferenceManager::~CATIAReferenceManager(void)
	{



	}

	// 모든 Sketch Entity(Point, Line 등)를 저장
	int CATIAReferenceManager::AddNewEntity(std::string strSketchName, std::string strTransCADName)
	{
		//만약 새로운 스케치가 추가되는 경우 _id 를 초기화
		int count = 0;
		for (int i = 0; i < (int)_listEntityNameMap.size(); ++i)
		{
			if (_listEntityNameMap[i]._sketchName == strSketchName)
				count++;
		}

		if (count == 0)
			_id = 0; 

		_id++;
		ENTITY_MAP map;
		map._sketchName = strSketchName;
		map.id = _id;
		map._entityName = strTransCADName;
		_listEntityNameMap.push_back(map);

		_listEntityID.push_back(_id);
		_listTransCADName.push_back(strTransCADName);

		return _id;
	}

	// 모든 Sketch, Feature, DatumPlane 정보를 저장
	int CATIAReferenceManager::AddNewFeature(TransCAD::IFeaturePtr spFeature)
	{
		std::string name = (std::string)spFeature->Name;
		TransCAD::FeatureType type = spFeature->Type;

		int id = 0;
		for (int i = 0; i < (int)_listNameMap.size(); ++i)
		{
			if (_listNameMap[i]._type == type)
				id++;
		}

		id++;

		NAME_MAP map;
		map._id = id;
		map._type = type;
		map._name = name;

		_listNameMap.push_back(map);

		return 1;
	}

	// EntityID로 TransCADName 검색
	int CATIAReferenceManager::GetTransCADNameByEntityID(std::string strSketchName, int id, std::string &strTransCADName)
	{
		for (int i = 0; i < (int)_listEntityNameMap.size(); ++i)
		{
			if (_listEntityNameMap[i]._sketchName == strSketchName && _listEntityNameMap[i].id == id)
			{
				strTransCADName = _listEntityNameMap[i]._entityName;
				return 1;
			}
		}

		return 0;
	}

	// TransCADName으로 EntityID 검색
	int CATIAReferenceManager::GetEntityIDByTransCADName(std::string strSketchName, std::string strTransCADName, int &id)
	{
		for (int i = 0; i < (int)_listEntityNameMap.size(); ++i)
		{
			if (_listEntityNameMap[i]._entityName == strTransCADName
				&& _listEntityNameMap[i]._sketchName == strSketchName
				&& i > 1)
			{
				id = _listEntityNameMap[i].id;
				return 1;
			}
		}

		return 0;
	}

	// TransCADName으로 FeatureID 검색
	int CATIAReferenceManager::GetFeatureIDByTransCADName(std::string strTransCADName, int &id)
	{
		for (int i = 0; i < (int)_listNameMap.size(); ++i)
		{
			if (_listNameMap[i]._name == strTransCADName)
			{
				id = _listNameMap[i]._id;
				return 1;
			}
		}

		return 0;
	}

	// TransCADName으로 FeatureType 검색
	int CATIAReferenceManager::GetFeatureTypeByTransCADName(std::string strTransCADName, TransCAD::FeatureType &type)
	{
		for (int i = 0; i < (int)_listNameMap.size(); ++i)
		{
			if (_listNameMap[i]._name == strTransCADName)
			{
				type = _listNameMap[i]._type;
				return 1;
			}
		}

		return 0;
	}

	// 모든 Point의 이름 저장. CATIA에서의 이름, TransCAD에서의 이름을 동시에 저장
	void CATIAReferenceManager::AddNewPointName(std::string strSketchName, std::string CATIApointName, std::string TransCADpointName)
	{
		POINTNAME_MAP map;
		map._sketchName = strSketchName;
		map._CATIAName = CATIApointName;
		map._TransCADName = TransCADpointName;

		_listPointNameMap.push_back(map);
	}

	// TransCAD에서의 point 이름으로 CATIA에서의 point 이름 검색
	std::string CATIAReferenceManager::GetCatiaPointNameByTransCADPointName(std::string strSketchName, std::string TransCADpointName)
	{
		string CATIAPointName;

		for (int i = 0; i < (int)_listPointNameMap.size(); ++i)
		{
			if (_listPointNameMap[i]._sketchName == strSketchName && _listPointNameMap[i]._TransCADName == TransCADpointName)
			{
				CATIAPointName = _listPointNameMap[i]._CATIAName;
			}
		}

		return CATIAPointName;
	}

	// TransCAD의 FeatureType을 입력받아 CATIA의 FeatureType을 string으로 리턴
	std::string CATIAReferenceManager::FeatureTypeToString(TransCAD::FeatureType type)
	{
		switch (type)
		{
		case TransCAD::StdSolidProtrusionExtrudeFeature:
			return std::string("Pad");
		case TransCAD::StdSolidCutExtrudeFeature:
			return std::string("Pocket");
		case TransCAD::StdSketchFeature:
			return std::string("Sketch");
		case TransCAD::StdSolidProtrusionSweepFeature:
			return std::string("Rib");
		case TransCAD::StdSolidProtrusionRevolveFeature:
			return std::string("Shaft");
		case TransCAD::StdSolidCutRevolveFeature:
			return std::string("Groove");
		default:
			return std::string("None");
		}
	}

	// TransCAD Face의 고유 명칭을 받아서 CATIA Face의 고유 명칭을 출력
	std::string CATIAReferenceManager::ConvertToBRepFace(std::string strPersistentName)
	{
		EN_F f;
		Parsing_F(strPersistentName, f);

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(f._bn.feature_id, type);
		GetFeatureIDByTransCADName(f._bn.feature_id, id);

		if (f._sn.total_num != 0)
		{
			GetFeatureTypeByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, type);
			GetFeatureIDByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, id);
		}

		sprintf_s(buf, "%s.%d", FeatureTypeToString(type).c_str(), id);
		std::string feature_name(buf);

		std::string faceName, finalName;
		faceName = GetFaceNameOfCATIA(f);

		finalName = "Selection_RSur:(Face:(Brp:("
			+ faceName
			+ ");None:();Cf11:());"
			+ feature_name
			+ "_ResultOUT;Z0;G3563)";

		return finalName;
	}

	// TransCAD Face의 고유 명칭을 받아서 CATIA Face의 고유 명칭을 출력 (HoleCounterbored와 HoleCountersunk 전용)
	std::string CATIAReferenceManager::ConvertToBRepFaceForHole(std::string strPersistentName)
	{
		EN_F f;
		Parsing_F(strPersistentName, f);

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(f._bn.feature_id, type);
		GetFeatureIDByTransCADName(f._bn.feature_id, id);

		if (f._sn.total_num != 0)
		{
			GetFeatureTypeByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, type);
			GetFeatureIDByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, id);
		}

		if (FeatureTypeToString(type) == "Pad")
			sprintf_s(buf, "%s%d", "pad", id);
		else if (FeatureTypeToString(type) == "Rib")
			sprintf_s(buf, "%s%d", "rib", id);
		else if (FeatureTypeToString(type) == "Pocket")
			sprintf_s(buf, "%s%d", "pocket", id);
		else if (FeatureTypeToString(type) == "Shaft")
			sprintf_s(buf, "%s%d", "shaft", id);
		else if (FeatureTypeToString(type) == "Groove")
			sprintf_s(buf, "%s%d", "groove", id);

		std::string feature_name(buf);

		std::string faceName, finalName;
		faceName = GetFaceNameOfCATIA(f);

		finalName = "FSur:(Face:(Brp:("
			+ faceName
			+ ");None:();Cf11:());WithTemporaryBody;WithoutBuildError;WithInitialFeatureSupport;MonoFond;MFBRepVersion_CXR15)\", "
			+ feature_name;

		return finalName;
	}

	// TransCAD Face의 고유 명칭을 받아서 CATIA Face의 고유 명칭을 출력 (DatumPlaneOffset 전용) 
	std::string CATIAReferenceManager::ConvertToBRepFaceForDatumPlane(std::string strPersistentName)
	{
		EN_F f;
		Parsing_F(strPersistentName, f);

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(f._bn.feature_id, type);
		GetFeatureIDByTransCADName(f._bn.feature_id, id);

		if (f._sn.total_num != 0)
		{
			GetFeatureTypeByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, type);
			GetFeatureIDByTransCADName(f._sn._bn_list[f._sn.total_num - 1].feature_id, id);
		}

		if (FeatureTypeToString(type) == "Pad")
			sprintf_s(buf, "%s%d", "pad", id);
		else if (FeatureTypeToString(type) == "Rib")
			sprintf_s(buf, "%s%d", "rib", id);
		else if (FeatureTypeToString(type) == "Pocket")
			sprintf_s(buf, "%s%d", "pocket", id);
		else if (FeatureTypeToString(type) == "Shaft")
			sprintf_s(buf, "%s%d", "shaft", id);
		else if (FeatureTypeToString(type) == "Groove")
			sprintf_s(buf, "%s%d", "groove", id);

		std::string feature_name(buf);

		std::string faceName, finalName;
		faceName = GetFaceNameOfCATIA(f);

		finalName = "RSur:(Face:(Brp:("
			+ faceName
			+ ");None:();Cf11:());WithPermanentBody;WithoutBuildError;WithSelectingFeatureSupport;MFBRepVersion_CXR15)\", "
			+ feature_name;

		return finalName;
	}

	// TransCAD Edge의 고유 명칭을 받아서 CATIA Edge의 고유 명칭을 출력
	std::string CATIAReferenceManager::ConvertToBRepEdge(std::string strPersistentName)
	{
		EN_E e;
		Parsing_E(strPersistentName, e);


		



		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(e._f2._bn.feature_id, type);
		GetFeatureIDByTransCADName(e._f2._bn.feature_id, id);

		if (e._f2._sn.total_num != 0)
		{
			GetFeatureTypeByTransCADName(e._f2._sn._bn_list[e._f2._sn.total_num - 1].feature_id, type);
			GetFeatureIDByTransCADName(e._f2._sn._bn_list[e._f2._sn.total_num - 1].feature_id, id);
		}

		if (FeatureTypeToString(type) == "Pad")
			sprintf_s(buf, "%s%d", "pad", id);
		else if (FeatureTypeToString(type) == "Rib")
			sprintf_s(buf, "%s%d", "rib", id);
		else if (FeatureTypeToString(type) == "Pocket")
			sprintf_s(buf, "%s%d", "pocket", id);
		else if (FeatureTypeToString(type) == "Shaft")
			sprintf_s(buf, "%s%d", "shaft", id);
		else if (FeatureTypeToString(type) == "Groove")
			sprintf_s(buf, "%s%d", "groove", id);

		std::string feature_name(buf);

		std::string faceName1, faceName2, finalName;
		faceName1 = GetFaceNameOfCATIA(e._f1);
		faceName2 = GetFaceNameOfCATIA(e._f2);

		finalName = "REdge:(Edge:(Face:(Brp:("
			+ faceName1
			+ ");None:();Cf11:());Face:(Brp:("
			+ faceName2
			+ ");None:();Cf11:());None:(Limits1:();Limits2:());Cf11:());WithTemporaryBody;WithoutBuildError;WithSelectingFeatureSupport;MFBRepVersion_CXR14)\", "
			+ feature_name;

		return finalName;
	}

	// TransCAD의 FaceName을 입력받아서 CATIA의 FaceName을 출력하는 함수. SecondaryName 여부를 체크한다.
	std::string CATIAReferenceManager::GetFaceNameOfCATIA(EN_F faceName)
	{
		std::string CATbasicName, CATsecondaryName;

		CATbasicName = GetBasicNameOfCATIA(faceName._bn);

		if (faceName._sn.total_num != 0)
		{
			for (int i = 0; i < faceName._sn.total_num; ++i)
				CATsecondaryName = CATsecondaryName + ";Brp:(" + GetBasicNameOfCATIA(faceName._sn._bn_list[i]) + ")";

			CATbasicName = "(Brp:(" + CATbasicName + ")" + CATsecondaryName + ")";
		}

		return CATbasicName;
	}

	// TransCAD의 BasicName을 입력받아서 CATIA의 BasicName을 출력하는 함수
	std::string CATIAReferenceManager::GetBasicNameOfCATIA(basic_naming basicName)
	{
		// 면의 타입 (-1: 스케치 면, -2: 스케치 반대면, 그 외는 옆면)
		int id2 = atoi(basicName.id2.c_str());

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		GetFeatureTypeByTransCADName(basicName.feature_id, type);
		GetFeatureIDByTransCADName(basicName.feature_id, id);
		sprintf_s(buf, "%s.%d", FeatureTypeToString(type).c_str(), id);
		std::string feature_name(buf); //feature name with number

		sprintf_s(buf, "%s", FeatureTypeToString(type).c_str());
		std::string feature_name1(buf); //feature name without number

		GetFeatureTypeByTransCADName(basicName.id1, type);
		GetFeatureIDByTransCADName(basicName.id1, id);
		sprintf_s(buf, "%s.%d", FeatureTypeToString(type).c_str(), id);
		std::string sketch_name1(buf);

		GetFeatureTypeByTransCADName(basicName.id3, type);
		GetFeatureIDByTransCADName(basicName.id3, id);
		sprintf_s(buf, "%s.%d", FeatureTypeToString(type).c_str(), id);
		std::string sketch_name2(buf);

		int entityID1;
		GetEntityIDByTransCADName(basicName.id1, basicName.id2, entityID1);
		sprintf_s(buf, "%d", entityID1);
		std::string entity_number1(buf);

		int entityID2;
		GetEntityIDByTransCADName(basicName.id3, basicName.id4, entityID2);
		sprintf_s(buf, "%d", entityID2);
		std::string entity_number2(buf);

		std::string CATIAbasicName;

		switch (id2)
		{
		case -1:
			CATIAbasicName = feature_name + ";1";
			break;
		case -2:
			CATIAbasicName = feature_name + ";2";
			break;
		default:
			if (basicName.id3.find("ketch") == -1)
			{
				CATIAbasicName = feature_name
					+ ";0:(Brp:("
					+ sketch_name1		// sketch 
					+ ";"
					+ entity_number1	// sketchEntity 
					+ "))";
			}
			else //Sweep과 같이 Sketch가 2개 있는 경우
			{
				CATIAbasicName = feature_name
					+ ";0:(Brp:("
					+ sketch_name1		// sketch1
					+ ";"
					+ entity_number1	// sketchEntity1
					+ ");Brp:("
					+ sketch_name2		// sketch2
					+ ";"
					+ entity_number2	// sketchEntity2
					+ "))";
			}
			break;
		}

		return CATIAbasicName;
	}

	// referencee 이름을 받아서 Feature 이름만 리턴하는 함수
	std::string CATIAReferenceManager::GetReferenceeFeature(std::string strPersistentName)
	{
		EN_F f;
		Parsing_F(strPersistentName, f);

		TransCAD::FeatureType type;
		char buf[1024];

		GetFeatureTypeByTransCADName(f._bn.feature_id, type);
		sprintf_s(buf, "%s", FeatureTypeToString(type).c_str());
		std::string feature_name(buf);

		return feature_name;
	}

	void CATIAReferenceManager::eew() {
		/*
		::TransCAD::IApplicationPtr g_spApplication;
		::TransCAD::IAssemDocumentPtr _spAssemDocument;
		::TransCAD::IAssemPtr _spAssem;
		::TransCAD::ICompPtr _spComp;

		//COM initialize
		int status = false;
		HRESULT hr = CoInitialize(NULL);
		//TransCAD instance 생성
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		//TransCAD initialize error check
		if (FAILED(hr))status = 0;
		//Assembly document 생성
		_spAssemDocument = g_spApplication->GetDocuments()->AddAssemDocument();
		//Assem 객체 생성
		_spAssem = _spAssemDocument->GetAssem();
		//Get Parts
		_spComp = _spAssem->GetComponent(1);
		int partCnt = _spComp->GetSize();

		for (int i = 0; i < partCnt; ++i) {
			string partName;
			TransCAD::IPartPtr part = _spComp->GetPart(i);
			cout << "Name of part [" << i << "] in component1" << part->Name << endl;

		}

		TransCAD::IPartPtr _spConstrainedPart;
		_spConstrainedPart = _spComp->GetPart(0);

		TransCAD::IPartPtr _spReferencePart;
		_spReferencePart = _spComp->GetPart(1);

		//Get Contraints
		TransCAD::IStdAssemConstraintsPtr _spContraints;
		_spContraints = _spAssem->Constraints;
		int contraintsCnt = _spContraints->Count;

		//Coaxial constraint

		TransCAD::IStdAssemConstraintPtr _spConstraint = _spContraints->Item[1];
		TransCAD::IStdAssemConstraintCoaxialPtr spCoaxial = _spConstraint;

		string constrainName = spCoaxial->Name;
		TransCAD::StdAssemblyConstraintType constraintType = spCoaxial->Type;
		string constrainedPartName = spCoaxial->ConstrainedPart->Name;
		string constrainedGeomName = spCoaxial->ConstrainedGeometry->GetReferenceeName();
		string referencePartName = spCoaxial->ReferencePart->Name;
		string referenceGeomName = spCoaxial->ReferenceGeometry->GetReferenceeName();



		cout << endl << "<< Coaxial constraint information >>" << endl;
		cout << "Constrained name is [" << constrainName << "]" << endl;
		cout << "Constrained part is [" << constrainedPartName << "]" << endl;
		cout << "Constrained geometry is [" << constrainedGeomName << "[" << endl;
		cout << "COnstraied Part is [" << referencePartName << "]" << endl;
		cout << "Constrained geometry is [ " << referenceGeomName << "]" << endl;

		*/
	}


	std::string CATIAReferenceManager::GetItemName(std::string listname, int index) {
	
		if (listname == "listSketchInfo_name") {
			return _listSketchInfo[index].sketch_name;
		
		}
	
	
	}

	/*
	
	//Point-based EDGE name

	std::string CATIAReferenceManager::ConvertToBRepEdge(std::string strPersistentName)
	{
		PBN_E e;
		Parsing_PBN_E(strPersistentName, e);

		TransCAD::FeatureType type;
		int id;
		char buf[1024];

		//std::string feature_name(buf); Currently Hard Coded

		std::string faceName1, faceName2, finalName;

		std::string featureID1, featureID2;

		featureID1 = e._p1.feature_id;
		featureID2 = e._p2.feature_id;

		GetLastFeatureType(featureID1, type); //issue here
		GetLastFeatureID(featureID1, id);

		if (FeatureTypeToString(type) == "Pad")
			sprintf_s(buf, "%s%d", "pad", id);
		else if (FeatureTypeToString(type) == "Rib")
			sprintf_s(buf, "%s%d", "rib", id);
		else if (FeatureTypeToString(type) == "Pocket")
			sprintf_s(buf, "%s%d", "pocket", id);
		else if (FeatureTypeToString(type) == "Shaft")
			sprintf_s(buf, "%s%d", "shaft", id);
		else if (FeatureTypeToString(type) == "Groove")
			sprintf_s(buf, "%s%d", "groove", id);

		std::string feature_name(buf);

		const char * c1 = featureID1.c_str();
		const char * c2 = featureID2.c_str();

		int result = strcmp(c1, c2);

	//	faceName1 = GetFaceNameOfCATIA_1(e._p1);
	//	faceName2 = GetFaceNameOfCATIA_2(e._p2);


	//	finalName = "REdge:(Edge:(Face:(Brp:("
	//		+ faceName1
	//		+ ");None:();Cf11:());Face:(Brp:("
	//		+ faceName2
	//		+ ");None:();Cf11:());None:(Limits1:();Limits2:());Cf11:());WithTemporaryBody;WithoutBuildError;WithSelectingFeatureSupport;MFBRepVersion_CXR14)\", "
	//		+ feature_name;

		// /*
		if (result == 0) //same feature IDs
		{
			if (e._p1.feature_id.find("ad") || e._p1.feature_id.find("xtrude")) //same protrusions (to-be generalized later)
			{

				faceName1 = GetFaceNameOfCATIA_S1(e, e._p1, e._p2);
				faceName2 = GetFaceNameOfCATIA_S2(e, e._p1, e._p2);


				finalName = "REdge:(Edge:(Face:(Brp:("
					+ faceName1
					+ ");None:();Cf11:());Face:(Brp:("
					+ faceName2
					+ ");None:();Cf11:());None:(Limits1:();Limits2:());Cf11:());WithTemporaryBody;WithoutBuildError;WithSelectingFeatureSupport;MFBRepVersion_CXR14)\", "
					+ feature_name;

			}
			else if (e._p1.feature_id.find("ut") || e._p1.feature_id.find("CUT")) //same cuts (to-be generalized later)
			{
				//later
			}

		}
		else
		{
			
		//	if ((e._p1.feature_id.find("ad") || e._p1.feature_id.find("xtrude")) && (e._p2.feature_id.find("ut") || e._p2.feature_id.find("CUT"))) //1 protrusion and 1 cut
		//	{
				//later
		//	}
		//	else //if (((e._p1.feature_id.find("ut") ) || (e._p1.feature_id.find("CUT") )) && ((e._p2.feature_id.find("ad") )|| (e._p2.feature_id.find("xtrude")))) //1 protrusion and 1 cut
		//	{ 
				std::string faceName3ab;

				faceName1 = GetFaceNameOfCATIA_S1(e, e._p1, e._p2);
				faceName2 = GetFaceNameOfCATIA_S2(e, e._p1, e._p2);
				faceName3ab = GetFaceNameOfCATIA_S3(e, e._p1, e._p2);
				//faceName3ab = ;

				finalName = "TgtEdge:(GeneratedEdges;MfIE_R20GA;TgtPropagationFillet;FirstOperands:(Pad.1);SecondOperands:();InitEdges:(REdge:(Edge:(Face:(Brp:("
					+ faceName1
					+ ");None:();Cf11:());Face:(Brp:("
					+ faceName2
					+ ");None:();Cf11:());AtLeastOneNoSharedIncluded:(Limits1:(Brp:("
					+ faceName3ab
					+ "));Limits2:());Cf11:());WithTemporaryBody;WithoutBuildError;WithSelectingFeatureSupport;MFBRepVersion_CXR14)\", "
					+ feature_name;

			//}
			//else //two different cuts/protrusions
			//{
				//next
			//}
		}


		return finalName;
	} */

}
