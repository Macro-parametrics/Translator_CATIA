#pragma once
#include "Feature.h"

class Part;
class SKETCHItem;

class FSketch : public Feature
{
public:
	FSketch(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FSketch(void);

	void Clear();
	
	SKETCHItem * GetSketchItem(int index) const {return _sketchItemList.at(index);}
	size_t GetSketchItemsSize() const {return _sketchItemList.size();}
	
	virtual void GetInfo();
	virtual void ToCATIA();
	
protected:
	SKETCHItem * CreateSketchItem(TransCAD::IStdSketchGeometryPtr spItem);	

private:
	TransCAD::IReferencePtr _spReference;
	TransCAD::IStdSketchGeometriesPtr _spGeometries;

	std::vector<SKETCHItem*> _sketchItemList;
	double	_CSYS[9];	
	int _type;

	string _refName;
	SKETCH_INFO sInfo;
	
	bool _isInversed;
};