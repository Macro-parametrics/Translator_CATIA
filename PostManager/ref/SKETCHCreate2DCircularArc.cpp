#include "stdafx.h"
#include "SKETCHCreate2DCircularArc.h"

namespace Post {
	SKETCHCreate2DCircularArc::SKETCHCreate2DCircularArc(FSketch* pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
		: SKETCHItem(pFSketch, spItem)
	{
	}

	SKETCHCreate2DCircularArc::~SKETCHCreate2DCircularArc(void)
	{
	}
	
	void SKETCHCreate2DCircularArc::GetInfo()
	{
		extern int ExternMode;
		TransCAD::IStdSketchCircularArcPtr spItem = GetTransCADGeometryItem();

		SetCenterPoint(Point3D(spItem->CenterPoint->X, spItem->CenterPoint->Y, 0));
		SetStartPoint(Point3D(spItem->StartPoint->X, spItem->StartPoint->Y, 0));
		SetEndPoint(Point3D(spItem->EndPoint->X, spItem->EndPoint->Y, 0));
		TransCADName = spItem->Name;

		_startAngle = spItem->StartAngle;
		_endAngle = spItem->EndAngle;
		_radius = spItem->Radius;

		cout << "CenterPoint : " << _centerPoint[0] << ", " << _centerPoint[1] << endl;
		cout << "StartPoint : " << _startPoint[0] << ", " << _startPoint[1] << endl;
		cout << "EndPoint : " << _endPoint[0] << ", " << _endPoint[1] << endl;

		if (ExternMode == 1) { g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName); }
		//g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName);
	}

	void SKETCHCreate2DCircularArc::ToCATIA()
	{
		cout << "    SKETCHCreate2DCircularArc " << GetSketchItemName() << endl;

		extern int circle_index;
		extern int sktch;

		char circleName[1024];
		sprintf_s(circleName, "circle2D%d", circle_index);
		fprintf(GetFeatureSketch()->GetPart()->file, "Dim %s As Circle2D\n", circleName);
		fprintf(GetFeatureSketch()->GetPart()->file, "Set %s = factory2D%d.CreateCircle(%lf, %lf, %lf, %lf, %lf)\n\n", circleName, sktch, _centerPoint[0], _centerPoint[1], _radius, _startAngle, _endAngle);
		fprintf(GetFeatureSketch()->GetPart()->file, "%s.ReportName = %d\n\n", circleName, g_pRefManager->AddNewEntity(std::string(_pFSketch->GetTransCADFeature()->GetName()), TransCADName));

		circle_index++;
	}
}