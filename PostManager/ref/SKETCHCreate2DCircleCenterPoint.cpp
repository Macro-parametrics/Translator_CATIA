#include "stdafx.h"
#include "SKETCHCreate2DCircleCenterPoint.h"

namespace Post {
	SKETCHCreate2DCircleCenterPoint::SKETCHCreate2DCircleCenterPoint(FSketch* pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
		: SKETCHItem(pFSketch, spItem)
	{
	}

	SKETCHCreate2DCircleCenterPoint::~SKETCHCreate2DCircleCenterPoint(void)
	{
	}
	
	void SKETCHCreate2DCircleCenterPoint::GetInfo()
	{
		extern int ExternMode;
		TransCAD::IStdSketchCirclePtr spItem = GetTransCADGeometryItem();

		SetCenterPoint(Point3D(spItem->CenterPoint->X, spItem->CenterPoint->Y, 0));
		SetRadius(spItem->Radius);
		TransCADName = spItem->Name;

		cout << "CenterPoint : " << _centerPoint[0] << ", " << _centerPoint[1] << endl;
		cout << "Radius : " << _rad << endl;
		if (ExternMode == 1) { g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName); }
	}

	void SKETCHCreate2DCircleCenterPoint::ToCATIA()
	{
		cout << "    SKETCHCreate2DCircleCenterPoint " << GetSketchItemName() << endl;

		extern int circle_index;
		extern int sktch;

		fprintf(GetFeatureSketch()->GetPart()->file, "Dim circle2D%d As Circle2D\n", circle_index);
		fprintf(GetFeatureSketch()->GetPart()->file, "Set circle2D%d = factory2D%d.CreateClosedCircle(%f, %f, %f)\n\n", circle_index, sktch, _centerPoint[0], _centerPoint[1], _rad);
		fprintf(GetFeatureSketch()->GetPart()->file, "circle2D%d.ReportName = %d\n\n", circle_index, g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName));

		circle_index++;
	}
}