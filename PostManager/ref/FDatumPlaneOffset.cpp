#include "stdafx.h"
#include "FDatumPlaneOffset.h"

namespace Post {
	FDatumPlaneOffset::FDatumPlaneOffset(Part * pPart, TransCAD::IFeaturePtr spFeature) : Feature(pPart, spFeature)
	{
	}

	FDatumPlaneOffset::~FDatumPlaneOffset(void)
	{
	}

	void FDatumPlaneOffset::GetInfo()
	{
		TransCAD::IStdDatumPlaneOffsetFeaturePtr spFeature = GetTransCADFeature();
		TransCAD::IReferencePtr spRefernce = spFeature->GetSelectedPlane();

		_referenceName = spRefernce->ReferenceeName;
		_referenceType = spRefernce->GetType();
		_distance = spFeature->Distance;
		_isFlip = spFeature->IsFlip ? true : false;

		cout << "	" << "Name           : " << spFeature->Name << endl;
		cout << "	" << "Distance       : " << spFeature->Distance << endl;
		cout << "	" << "ReferenceeName : " << spRefernce->ReferenceeName << endl;
		cout << "	" << "Type           : " << spRefernce->Type << endl;
	}

	void FDatumPlaneOffset::ToCATIA()
	{
		extern int ref_index;
		extern int DatumPlaneID;

		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), DatumPlaneID))
			return;

		// 기본 명령어 세트. 또 선언 될 필요 없다. 첫 번째 Datum Plane 생성 시만 선언해주고 이후에도 계속해서 사용.
		if (DatumPlaneID == 1)
		{
			fprintf(GetPart()->file, "Dim hybridBodies1 As HybridBodies\n");
			fprintf(GetPart()->file, "Set hybridBodies1 = part1.HybridBodies\n\n");

			fprintf(GetPart()->file, "Dim hybridBody1 As HybridBody\n");
			fprintf(GetPart()->file, "Set hybridBody1 = hybridBodies1.Add()\n\n");

			fprintf(GetPart()->file, "Dim hybridShapeFactory1 As Factory\n");
			fprintf(GetPart()->file, "Set hybridShapeFactory1 = part1.HybridShapeFactory\n\n");

			fprintf(GetPart()->file, "Dim hybridShapes1 As HybridShapes\n");
			//fprintf(GetPart()->file, "Set hybridShapes1 = body1.HybridShapes\n\n");
			fprintf(GetPart()->file, "Set hybridShapes1 = hybridBody1.HybridShapes\n\n");
		}

		//피쳐가 좌표 평면일 경우와 그렇지 않은 경우 레퍼런스 선언을 달리해 줌.
		if (_referenceType == TransCAD::ExplicitModelObject)
		{
			if (_referenceName == "XYPlane" || _referenceName == "YZPlane" || _referenceName == "ZXPlane")
			{
				fprintf(GetPart()->file, "Dim hybridShapePlaneExplicit%d As AnyObject\n", DatumPlaneID);

				if (_referenceName == "XYPlane")
					fprintf(GetPart()->file, "Set hybridShapePlaneExplicit%d = originElements1.PlaneXY\n\n", DatumPlaneID);
				else if (_referenceName == "YZPlane")
					fprintf(GetPart()->file, "Set hybridShapePlaneExplicit%d = originElements1.PlaneYZ\n\n", DatumPlaneID);
				else if (_referenceName == "ZXPlane")
					fprintf(GetPart()->file, "Set hybridShapePlaneExplicit%d = originElements1.PlaneZX\n\n", DatumPlaneID);

				fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
				fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromObject(hybridShapePlaneExplicit%d)\n\n", ref_index, DatumPlaneID);
			}
			else
			{
				int id = 0;
				g_pRefManager->GetFeatureIDByTransCADName(_referenceName, id);

				fprintf(GetPart()->file, "Dim reference%d As HybridShape\n", ref_index);
				fprintf(GetPart()->file, "Set reference%d = hybridShapes1.Item(\"Plane.%d\")\n\n", ref_index, id);
			}

			fprintf(GetPart()->file, "Dim hybridShapePlaneOffset%d As HybridShapePlaneOffset\n", DatumPlaneID);

			if (!_isFlip)
				fprintf(GetPart()->file, "Set hybridShapePlaneOffset%d = hybridShapeFactory1.AddNewPlaneOffset(reference%d, %f, False)\n\n", DatumPlaneID, ref_index, _distance);
			else
				fprintf(GetPart()->file, "Set hybridShapePlaneOffset%d = hybridShapeFactory1.AddNewPlaneOffset(reference%d, %f, True)\n\n", DatumPlaneID, ref_index, _distance);

			_isBrepReference = false;
		}
		else if (_referenceType == TransCAD::Brep)
		{
			std::string faceName = g_pRefManager->ConvertToBRepFaceForDatumPlane(_referenceName);
			fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
			fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromBRepName(\"%s)\n\n", ref_index, faceName.c_str());
			fprintf(GetPart()->file, "Dim hybridShapePlaneOffset%d As HybridShapePlaneOffset\n", DatumPlaneID);

			if (!_isFlip)
				fprintf(GetPart()->file, "Set hybridShapePlaneOffset%d = hybridShapeFactory1.AddNewPlaneOffset(reference%d, %f, True)\n\n", DatumPlaneID, ref_index, _distance);
			else
				fprintf(GetPart()->file, "Set hybridShapePlaneOffset%d = hybridShapeFactory1.AddNewPlaneOffset(reference%d, %f, False)\n\n", DatumPlaneID, ref_index, _distance);

			_isBrepReference = true;
		}

		//fprintf(GetPart()->file, "body1.InsertHybridShape hybridShapePlaneOffset%d\n\n", DatumPlaneID);
		fprintf(GetPart()->file, "hybridBody1.AppendHybridShape hybridShapePlaneOffset%d\n\n", DatumPlaneID);
		fprintf(GetPart()->file, "part1.InWorkObject = hybridShapePlaneOffset%d\n\n", DatumPlaneID);

		fprintf(GetPart()->file, "part1.Update\n\n");

		ref_index += 1;
	}
}