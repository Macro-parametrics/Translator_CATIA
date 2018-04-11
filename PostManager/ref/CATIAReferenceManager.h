#pragma once
#include "CATIApost_DLL.h"
#include "PN_TYPE.h"
#include "MP_Utility.h"

typedef struct _tagNameMap
{
	int _id;
	TransCAD::FeatureType _type;
	std::string _name;
} NAME_MAP;

typedef struct _tagEntityMap
{
	int id;
	std::string _sketchName;
	std::string _entityName;
} ENTITY_MAP;

typedef struct _tagpointNameMap
{
	std::string _sketchName;
	std::string _CATIAName;
	std::string _TransCADName;
} POINTNAME_MAP;

typedef struct _sketchInfo 
{
	std::string sketch_name;
	std::string sketch_ref;
} SKETCH_INFO;

class CATIAReferenceManager
{
public:
	CATIAReferenceManager(void);
	~CATIAReferenceManager(void);

	int AddNewEntity(std::string strSketchName, std::string strTransCADName);
	int AddNewFeature(TransCAD::IFeaturePtr spFeature);
	
	int GetFeatureIDByTransCADName(std::string strTransCADName, int &id);
	int GetFeatureTypeByTransCADName(std::string strTransCADName, TransCAD::FeatureType &type);

	std::string FeatureTypeToString(TransCAD::FeatureType type);

 	std::string ConvertToBRepFace(std::string strPersistentName);
	std::string ConvertToBRepFaceForHole(std::string strPersistentName); // Counterbored 및 Countersunk 특징형상 전용
	std::string ConvertToBRepFaceForDatumPlane(std::string strPersistentName); // DatumPlaneOffset 전용
	std::string ConvertToBRepEdge(std::string strPersistentName);

	void AddNewPointName(std::string strSketchName, std::string CATIApointName, std::string TransCADpointName);
	std::string GetCatiaPointNameByTransCADPointName(std::string strSketchName, std::string TransCADpointName);

	void AddSketchInfo(SKETCH_INFO _sInfo) {_listSketchInfo.push_back(_sInfo);}
	//std::string GetSketchRefBySketchName(std::string _skechName);
	void eew();

protected:
	int GetTransCADNameByEntityID(std::string strSketchName, int id, std::string &strTransCADName);
	int GetEntityIDByTransCADName(std::string strSketchName, std::string strTransCADName, int &id);

	std::string GetFaceNameOfCATIA(EN_F faceName);
	std::string GetBasicNameOfCATIA(basic_naming basicName);

	std::string GetReferenceeFeature(std::string strPersistentName); // B-rep 이름을 입력받아 특징형상 타입을 리턴

private:
	int _id;
	std::vector<POINTNAME_MAP> _listPointNameMap;
	std::vector<ENTITY_MAP> _listEntityNameMap;
	std::vector<int> _listEntityID;
	std::vector<std::string> _listTransCADName;
	std::vector<NAME_MAP> _listNameMap;
	std::vector<SKETCH_INFO> _listSketchInfo;
};
