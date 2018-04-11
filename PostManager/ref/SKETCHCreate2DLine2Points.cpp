#include "stdafx.h"
#include "SKETCHCreate2DLine2Points.h"


SKETCHCreate2DLine2Points::SKETCHCreate2DLine2Points(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
	: SKETCHItem(pFSketch, spItem)
{
}

SKETCHCreate2DLine2Points::~SKETCHCreate2DLine2Points(void)
{
}
extern int ExternMode;
void SKETCHCreate2DLine2Points::GetInfo()
{
	TransCAD::IStdSketchLinePtr spItem = GetTransCADGeometryItem();
	
	SetStartPoint(Point3D(spItem->StartPoint->X, spItem->StartPoint->Y, 0));
	SetEndPoint(Point3D(spItem->EndPoint->X, spItem->EndPoint->Y, 0));
	TransCADName = spItem->Name;

	cout << "   Point1 : " << _startPoint[0] << " " << _startPoint[1] << " " << _startPoint[2] << endl;
	cout << "   Point2 : " << _endPoint[0] << " " << _endPoint[1] << " " << _endPoint[2] << endl;

	// TransCAD의 point이름으로 CATIA의 point이름 찾아서 저장
	sprintf_s(startPointName, "%s", g_pRefManager->GetCatiaPointNameByTransCADPointName(std::string(_pFSketch->GetTransCADFeature()->GetName()), std::string(spItem->GetStartPoint()->GetName())).c_str());
	sprintf_s(endPointName, "%s", g_pRefManager->GetCatiaPointNameByTransCADPointName(std::string(_pFSketch->GetTransCADFeature()->GetName()), std::string(spItem->GetEndPoint()->GetName())).c_str());
	if (ExternMode == 1) { g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName); }
}

void SKETCHCreate2DLine2Points::ToCATIA()
{
	cout << "    SKETCHCreate2DLine2Points " << GetSketchItemName() << endl;

	extern int ln_index;
	extern int sktch;

	char lineName[1024];
	sprintf_s(lineName, "line2D%d", ln_index);

	fprintf(GetFeatureSketch()->GetPart()->file, "Dim %s As Line2D\n", lineName);
	fprintf(GetFeatureSketch()->GetPart()->file, "Set %s = factory2D%d.CreateLine(%lf, %lf, %lf, %lf)\n\n", lineName, sktch, _startPoint[0], _startPoint[1], _endPoint[0], _endPoint[1]);
	fprintf(GetFeatureSketch()->GetPart()->file, "%s.ReportName = %d\n\n", lineName, g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName));
	fprintf(GetFeatureSketch()->GetPart()->file, "%s.StartPoint = %s\n\n", lineName, startPointName);
	fprintf(GetFeatureSketch()->GetPart()->file, "%s.EndPoint = %s\n\n", lineName, endPointName);

	ln_index++;
}