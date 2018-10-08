#pragma once
#include "CATIApost_DLL.h"
#include "CATIAReferenceManager.h"

namespace Post {
	class Feature;

	class Part
	{
	public:
		Part();											//used to make Part class array
		Part::Part(string fileName);				    //Basic Part Constructor : For a Part Translation, TransCAD Document must be PartDocument(filename : fullpath)
		Part(TransCAD::IPartPtr _p, string fileName);   //used Assembly Part : if you already have TransCAD Part ptr(IPartPtr) and you need to make a Script, use this constructor(filename : fullpath)
		Part(TransCAD::IPartPtr _p, int f);				//used Assembly Part : if you already have TransCAD Part ptr(IPartPtr) and you don't need to make a Script, use this constructor(default f=0)
		~Part(void);

		void Clear();

		string GetPartName() const { return _strPartName; }

		Feature * GetFeature(int i) const { return _featureList.at(i); }
		Feature * GetFeatureByName(string name);

		size_t GetSize() const { return _featureList.size(); }

		void GetInfo();
		void ToCATIA();

		void ExternModeToggle(int mode);
		void InitReferManager();
	protected:
		Feature * CreateFeature(TransCAD::IFeaturePtr spFeature);

	public:
		FILE * file;
		TransCAD::IDocsPtr _spDocuments;
		TransCAD::IPartDocumentPtr	_spDocument;
		TransCAD::IPartPtr			_spPart;
		TransCAD::IFeaturesPtr		_spFeatures;

	private:
		string	_strPartName;	// TransCAD Part full path name
		vector<Feature*> _featureList;
	};
}