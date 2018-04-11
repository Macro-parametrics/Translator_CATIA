#pragma once
#include "CATIApre_DLL.h"
namespace Pre {
	class Feature;
	class AttributeItem;
	class ReferenceEntity;

	struct SketchFeature
	{
		string _featureName;
		string _sketchName;
	};

	class Part
	{
	public:

		Part(string fileName);
		Part::Part(string fileName, int partNum);
		Part::Part(string fileName, TransCAD::IPartDocumentPtr partdoc);
		virtual ~Part(void);
		virtual void Clear();

		Feature * GetFeature(int i) const { return _featureList.at(i); }
		Feature * GetFeatureByName(string name);

		ReferenceEntity * GetReferenceEntity(int i) const { return _referenceEntityList.at(i); }
		ReferenceEntity * GetReferenceEntityByName(string name);

		AttributeItem * GetAttributeItem(int i) const { return _attributeItemList.at(i); }
		AttributeItem * GetAttributeItemByName(string name);

		virtual void AddFeatureNameSketchName(string featureName, string sketchName);
		virtual string GetSketchNameFromFeatureName(string featureName);
		virtual string GetFeatureNameFromSketchName(string sketchName);
		size_t GetSize() const { return _featureList.size(); }

		void GetInfo();
		void ToTransCAD();

		int GetPartDocNumber() { return _partDocNum; }

	protected:
		void CreateFeature(string type, string name, ifstream &is);
		void CreateAttributeItem(string type, string name, ifstream &is);
		void CreateReferenceEntity(string type, string name, ifstream &is);

	public:
		ifstream fin;
		TransCAD::IDocsPtr _spDocuments;
		TransCAD::IPartDocumentPtr	_spDocument;
		TransCAD::IPartPtr			_spPart;
		TransCAD::IFeaturesPtr		_spFeatures;
		int _partDocNum;

	private:
		vector<Feature*> _featureList;
		vector<AttributeItem*> _attributeItemList;
		vector<ReferenceEntity*> _referenceEntityList;
		vector <SketchFeature> sketchNameTable;
		string _filePath;
		string _fileName;
	};
}