#include "stdafx.h"
#include "FSketch.h"

#include "SKETCHItem.h"
#include "SKETCHCreate2DCircleCenterPoint.h"
#include "SKETCHCreate2DLine2Points.h"
#include "SKETCHCreate2DPoint.h"
#include "SKETCHCreate2DCircularArc.h"
#include "SKETCHCreate2DCenterline.h"

#include "FDatumPlaneOffset.h"
namespace Post {
	

	FSketch::FSketch(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
		// set the Coordinate System
		((TransCAD::IStdSketchFeaturePtr)_spFeature)->GetCoordinateSystem(_CSYS + 0, _CSYS + 1, _CSYS + 2, _CSYS + 3, _CSYS + 4, _CSYS + 5, _CSYS + 6, _CSYS + 7, _CSYS + 8);

		// set sketch plane reference
		_spReference = ((TransCAD::IStdSketchFeaturePtr)_spFeature)->SketchPlane;

		// set sketch objects group, Geometries
		_spGeometries = ((TransCAD::IStdSketchFeaturePtr)_spFeature)->Geometries;

		// Sketch의 레퍼런스가 Brep Face를 참조하는 DatumPlaneOffset인 경우, 
		// 스케치 평면과 스케치 좌표계의 방향이 같을 때 이 값이 true가 되어, CATIA의 스케치의 방향을 뒤집는다.
		// 왜냐하면 Brep Face를 참조하는 DatumPlane은 CATIA와 TransCAD에서의 방향이 반대이기 때문
		_isInversed = false;
	}

	FSketch::~FSketch(void)
	{
		Clear();
	}

	void FSketch::Clear()
	{
		std::vector<SKETCHItem *>::iterator i = _sketchItemList.begin();

		while (i != _sketchItemList.end())
		{
			delete *i;
			++i;
		}

		_sketchItemList.clear();
	}

	SKETCHItem * FSketch::CreateSketchItem(TransCAD::IStdSketchGeometryPtr spItem)
	{
		SKETCHItem * pSketchItem = 0;

		cout << "  SketchGeometry Type : " << spItem->Type << endl;

		switch (spItem->Type)
		{
		case TransCAD::Line:
			pSketchItem = new SKETCHCreate2DLine2Points(this, spItem);
			cout << " -> " << "SKETCHCreate2DLIne2Points created" << endl;
			break;
		case TransCAD::Centerline:
			pSketchItem = new SKETCHCreate2DCenterline(this, spItem);
			cout << " -> " << "SKETCHCreate2DCenterline created" << endl;
			break;
		case TransCAD::Circle:
			pSketchItem = new SKETCHCreate2DCircleCenterPoint(this, spItem);
			cout << " -> " << "SKETCHCreate2DCircleCenterPoint created" << endl;
			break;
		case TransCAD::CircularArc:
			pSketchItem = new SKETCHCreate2DCircularArc(this, spItem);
			cout << " -> " << "SKETCHCreate2DCircularArc created" << endl;
			break;
		case TransCAD::ControlPoint:
			pSketchItem = new SKETCHCreate2DPoint(this, spItem);
			cout << " -> " << "SKETCHCreate2DPoint created" << endl;
			break;
		}

		if (pSketchItem)
			_sketchItemList.push_back(pSketchItem);

		return pSketchItem;
	}

	void FSketch::GetInfo()
	{
		extern int ExternMode;

		// 레퍼런스 이름, 타입
		_refName = _spReference->ReferenceeName;
		_type = _spReference->Type;

		cout << "  Ref name:" << _spReference->Name << " Type= " << _spReference->Type << endl;
		cout << "  RefFullName: " << _refName << endl;

		sInfo.sketch_name = GetFeatureName().c_str();
		sInfo.sketch_ref = _refName;

		g_pRefManager->AddSketchInfo(sInfo);

		if (ExternMode == 1) {
			g_pRefManager->AddNewEntity(sInfo.sketch_name, std::string("HDirection"));

		}
		if (ExternMode == 1) { g_pRefManager->AddNewEntity(sInfo.sketch_name, std::string("VDirection")); }

		///////////////////////////////스케치 지오메트리
		if (ExternMode == 1) {
			SKETCHItem *  pSketchItem;

			for (int i = 1; i <= _spGeometries->Count; ++i)
			{
				TransCAD::IStdSketchGeometryPtr spItem = _spGeometries->Item[i];

				pSketchItem = CreateSketchItem(spItem);

				if (!pSketchItem)
					cerr << "  " << "****** Cannot support the sketch object! ******" << endl;
			}

			cout << "  " << "***** Translating Sketch Items... *****" << endl;	//DEBUG

																				//-------  Translate SketchItems in list  --------//


			for (size_t i = 0; i < GetSketchItemsSize(); ++i)
			{
				SKETCHItem * pItem = GetSketchItem((unsigned int)i);

				pItem->GetInfo(); // 스케치 요소 정보(점, 선, 원, 호 등) 불러오기
				// pItem->ToCATIA();  // 스케체 요소 정보 CATIA 매크로로 내보내기
								  //DEBUG
				cout << "  " << (unsigned int)i << " : " << GetSketchItem((unsigned int)i)->GetSketchItemName() << " translated" << endl;
			}
		}
	}

	void FSketch::ToCATIA()
	{
		extern int ref_index;
		extern int arr_index;
		extern int ln_index;
		extern int sktch;

		// 좌표평면의 경우
		if (_refName == "XYPlane" || _refName == "YZPlane" || _refName == "XZPlane" || _refName == "ZXPlane")
		{
			fprintf(GetPart()->file, "Dim reference%d As AnyObject\n", ref_index);
			fprintf(GetPart()->file, "Set reference%d", ref_index);

			if (_refName == "XYPlane")
				fprintf(GetPart()->file, " = originElements1.PlaneXY\n\n");
			else if (_refName == "YZPlane")
				fprintf(GetPart()->file, " = originElements1.PlaneYZ\n\n");
			else if (_refName == "XZPlane")
				fprintf(GetPart()->file, " = originElements1.PlaneZX\n\n"); //CATIA 는 ZXPlane으로 인식
			else if (_refName == "ZXPlane")
				fprintf(GetPart()->file, " = originElements1.PlaneZX\n\n");
		}
		else if (_type == 2) // Face인 경우
		{
			std::string face_name = g_pRefManager->ConvertToBRepFace(_refName);

			fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
			fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromName(\"%s\")\n\n", ref_index, face_name.c_str());
		}
		else // Datum Plane의 경우
		{
			int count = 0;

			for (int i = 1; i <= _pPart->_spFeatures->GetCount(); ++i)
			{
				TransCAD::IFeaturePtr pFeature = _pPart->_spFeatures->GetItem(i);
				string pFeatureName = pFeature->GetName();

				if (pFeature->GetType() == TransCAD::StdDatumPlaneOffsetFeature)
				{
					count++;

					if (_refName == pFeatureName)
					{
						FDatumPlaneOffset * pDatum = (FDatumPlaneOffset *)_pPart->GetFeatureByName(pFeatureName);

						if (pDatum->IsBrepReference())
						{
							if (!((TransCAD::IStdSketchFeaturePtr)GetTransCADFeature())->GetIsInversed())
								_isInversed = true;
						}

						break;
					}
				}
			}

			fprintf(GetPart()->file, "Dim reference%d As HybridShape\n", ref_index);
			fprintf(GetPart()->file, "Set reference%d = hybridShapes1.Item(\"Plane.%d\")\n\n", ref_index, count);
		}

		fprintf(GetPart()->file, "Dim sketch%d As Sketch\n", sktch);
		fprintf(GetPart()->file, "Set sketch%d = sketches1.Add(reference%d)\n\n", sktch, ref_index);

		fprintf(GetPart()->file, "Dim arrayOfVariantOfDouble%d(8)\n", arr_index);

		for (int k = 0; k < 9; ++k)
			fprintf(GetPart()->file, "arrayOfVariantOfDouble%d(%d) = %lf\n", arr_index, k, _CSYS[k]);

		fprintf(GetPart()->file, "sketch%d.SetAbsoluteAxisData arrayOfVariantOfDouble%d\n\n", sktch, arr_index);
		fprintf(GetPart()->file, "part1.InWorkObject = sketch%d\n\n", sktch);

		fprintf(GetPart()->file, "Dim factory2D%d As Factory2D\n", sktch);
		fprintf(GetPart()->file, "Set factory2D%d = sketch%d.OpenEdition()\n\n", sktch, sktch);
		fprintf(GetPart()->file, "Dim geometricElements%d As GeometricElements\n", sktch);
		fprintf(GetPart()->file, "Set geometricElements%d = sketch%d.GeometricElements\n\n", sktch, sktch);
		fprintf(GetPart()->file, "Dim axis2D%d As GeometricElement\n", sktch);
		fprintf(GetPart()->file, "Set axis2D%d = geometricElements%d.Item(\"AbsoluteAxis\")\n\n", sktch, sktch);

		fprintf(GetPart()->file, "Dim line2D%d As CATBaseDispatch\n", ln_index);
		fprintf(GetPart()->file, "Set line2D%d = axis2D%d.GetItem(\"HDirection\")\n\n", ln_index, sktch);

		char sketchName[1024], lineName1[1024];
		sprintf_s(sketchName, "Sketch%d", sktch);
		sprintf_s(lineName1, "HDirection");
		fprintf(GetPart()->file, "line2D%d.ReportName = %d\n\n", ln_index, g_pRefManager->AddNewEntity(std::string(sketchName), std::string(lineName1)));

		ln_index += 1;

		fprintf(GetPart()->file, "Dim line2D%d As CATBaseDispatch\n", ln_index);
		fprintf(GetPart()->file, "Set line2D%d = axis2D%d.GetItem(\"VDirection\")\n\n", ln_index, sktch);

		char lineName2[1024];
		sprintf_s(lineName2, "VDirection");
		fprintf(GetPart()->file, "line2D%d.ReportName = %d\n\n", ln_index, g_pRefManager->AddNewEntity(std::string(sketchName), std::string(lineName2)));

		ln_index += 1;
		ref_index += 1;
		arr_index += 1;

		////-------  Create sketch item list in Geometries --------//
		SKETCHItem *  pSketchItem;

		for (int i = 1; i <= _spGeometries->Count; ++i)
		{
			TransCAD::IStdSketchGeometryPtr spItem = _spGeometries->Item[i];

			pSketchItem = CreateSketchItem(spItem);

			if (!pSketchItem)
				cerr << "  " << "****** Cannot support the sketch object! ******" << endl;
		}

		cout << "  " << "***** Translating Sketch Items... *****" << endl;	//DEBUG

		//-------  Translate SketchItems in list  --------//
		for (size_t i = 0; i < GetSketchItemsSize(); ++i)
		{
			SKETCHItem * pItem = GetSketchItem((unsigned int)i);

			pItem->GetInfo(); // 스케치 요소 정보(점, 선, 원, 호 등) 불러오기
			pItem->ToCATIA(); // 스케체 요소 정보 CATIA 매크로로 내보내기

			//DEBUG
			cout << "  " << (unsigned int)i << " : " << GetSketchItem((unsigned int)i)->GetSketchItemName() << " translated" << endl;
		}

		fprintf(GetPart()->file, "sketch%d.CloseEdition\n\n", sktch);

		if (_isInversed)
			fprintf(GetPart()->file, "sketch%d.InverseOrientation()\n\n", sktch);

		fprintf(GetPart()->file, "part1.InWorkObject = sketch%d\n\n", sktch);
		fprintf(GetPart()->file, "part1.UpdateObject sketch%d\n\n", sktch);

		sktch += 1;
	}
}