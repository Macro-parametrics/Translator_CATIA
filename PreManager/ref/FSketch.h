#pragma once
#include "Feature.h"
namespace Pre {
	class FSKETCHItem;
	class ReferenceEntity;

	struct ReportItem
	{
		int _reportName;
		string _entityName;
	};

	class FSketch : public Feature
	{
	public:
		FSketch(Part * pPart, int type, string name);
		virtual ~FSketch(void);
		void Clear();

		FSKETCHItem * GetSketchItem(int index) const { return _sketchItemList.at(index); }
		size_t GetSketchItemsSize() const { return _sketchItemList.size(); }
		string GetFactoryName() { return _factoryName; }
		string GetAxisName() { return _axisName; }

		virtual ReferenceEntity * GetReferenceEntity() { return _pReferenceEntity; }
		virtual FSKETCHItem * GetSketchItemFromName(string name);

		virtual void AddEntityNameReportName(int reportName, string entityName);
		virtual string GetEntityNameFromReportName(int reportName);
		virtual TransCAD::ISketchEditorPtr GetSketchEditorPtr() { return _spSketEditor; }
		virtual TransCAD::IStdSketchFeaturePtr GetSketchFeaturePtr() { return _spSketch; }

		virtual void GetInfo(ifstream & is);
		void GetSketchItemInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void Modify(char * buffer);

	protected:
		void CreateSketchItem(string type, string name, ifstream &is);

		ReferenceEntity * _pReferenceEntity;

	public:
		std::vector<FSKETCHItem*> _sketchItemList;
		vector<ReportItem> reportNameTable;

		double sketchCoord[9];
		int id;
		int type;  // 0: none, 1: base Sketch, 2: other Sketch
		string _factoryName;
		string _axisName;

	public:
		TransCAD::ISketchEditorPtr _spSketEditor;	// TransCAD SketchEditorPtr
		TransCAD::IStdSketchFeaturePtr _spSketch;
	};
}