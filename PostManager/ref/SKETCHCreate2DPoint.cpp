#include "stdafx.h"
#include "SKETCHCreate2DPoint.h"


SKETCHCreate2DPoint::SKETCHCreate2DPoint(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
	: SKETCHItem(pFSketch, spItem)
{
}

SKETCHCreate2DPoint::~SKETCHCreate2DPoint(void)
{
}
extern int ExternMode;
void SKETCHCreate2DPoint::GetInfo()
{
	TransCAD::IStdSketchControlPointPtr spItem = GetTransCADGeometryItem();
	
	SetPoint(Point3D(spItem->GetX(), spItem->GetY(), 0));
	TransCADName = spItem->Name;
	if (ExternMode == 1) { g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName); }
}

void SKETCHCreate2DPoint::ToCATIA()
{
	extern int pt_index;
	extern int sktch;

	char pointName[1024];
	sprintf_s(pointName, "point2D%d", pt_index);
	
	g_pRefManager->AddNewPointName(std::string(_pFSketch->GetTransCADFeature()->GetName()), std::string(pointName), TransCADName);

	fprintf(GetFeatureSketch()->GetPart()->file, "Dim %s As Point2D\n", pointName);
	fprintf(GetFeatureSketch()->GetPart()->file, "Set %s = factory2D%d.CreatePoint(%lf, %lf)\n\n", pointName, sktch, _point.X(), _point.Y());
	fprintf(GetFeatureSketch()->GetPart()->file, "%s.ReportName = %d\n\n", pointName, g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName));

	pt_index++;
}