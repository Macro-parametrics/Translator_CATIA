#include "stdafx.h"
#include ".\SKETCHCreate2DCenterline.h"

namespace Post {
	SKETCHCreate2DCenterline::SKETCHCreate2DCenterline(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
		: SKETCHItem(pFSketch, spItem)
	{
	}

	SKETCHCreate2DCenterline::~SKETCHCreate2DCenterline(void)
	{
	}

	

	void SKETCHCreate2DCenterline::GetInfo()
	{
		extern int ExternMode;
		TransCAD::IStdSketchCenterlinePtr spItem = GetTransCADGeometryItem();

		SetStartPoint(Point3D(spItem->StartPoint->X, spItem->StartPoint->Y, 0));
		SetEndPoint(Point3D(spItem->EndPoint->X, spItem->EndPoint->Y, 0));
		TransCADName = spItem->Name;

		cout << "Centerline 시작점 : " << spItem->StartPoint->X << ", " << spItem->StartPoint->Y << " 끝점 : " << spItem->EndPoint->X << ", " << spItem->EndPoint->Y << endl;
		if (ExternMode == 1) { g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName); }
	}

	void SKETCHCreate2DCenterline::ToCATIA()
	{
		cout << "    SKETCHCreate2DCenterline " << GetSketchItemName() << endl;

		extern int ln_index;
		extern int sktch;

		fprintf(GetFeatureSketch()->GetPart()->file, "Dim line2D%d As Line2D\n", ln_index);
		fprintf(GetFeatureSketch()->GetPart()->file, "Set line2D%d = factory2D%d.CreateLine(%lf, %lf, %lf, %lf)\n\n", ln_index, sktch, _startPoint[0], _startPoint[1], _endPoint[0], _endPoint[1]);
		fprintf(GetFeatureSketch()->GetPart()->file, "sketch%d.CenterLine = line2D%d \n\n", sktch, ln_index);

		g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName);

		ln_index++;
	}
}