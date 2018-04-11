#pragma once
#include "CATIApost_DLL.h"
#include "CATIAReferenceManager.h"


class Feature;

class Part
{
public:
	Part(); 
	Part::Part(string fileName);
	Part(TransCAD::IPartPtr _p, string fileName);
	Part(TransCAD::IPartPtr _p,int f);
	~Part(void);

	void Clear();

    string GetPartName() const {return _strPartName;}

	Feature * GetFeature(int i) const {return _featureList.at(i);}
	Feature * GetFeatureByName(string name);

	size_t GetSize() const {return _featureList.size();}
	
	void GetInfo();
	void ToCATIA();

	void ExternModeToggle(int mode);
	void InitReferManager();
protected:
	Feature * CreateFeature(TransCAD::IFeaturePtr spFeature);
	
public:
	FILE *file;
	TransCAD::IDocsPtr _spDocuments;
	TransCAD::IPartDocumentPtr	_spDocument;
	TransCAD::IPartPtr			_spPart;
	TransCAD::IFeaturesPtr		_spFeatures;

private:
	string	_strPartName;	// TransCAD Part full path name
	vector<Feature*> _featureList;
};