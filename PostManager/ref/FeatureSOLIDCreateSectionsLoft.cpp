#include "stdafx.h"
#include "FeatureSOLIDCreateSectionsLoft.h"
#include "FSketch.h"

namespace Post {
	FeatureSOLIDCreateSectionsLoft::FeatureSOLIDCreateSectionsLoft(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
		hybridShapeExtremumNum = 0;
	}

	FeatureSOLIDCreateSectionsLoft::~FeatureSOLIDCreateSectionsLoft(void)
	{
	}

	void FeatureSOLIDCreateSectionsLoft::GetInfo()
	{
		TransCAD::IStdSolidLoftSectionsFeaturePtr spFeature = GetTransCADFeature();
		TransCAD::IReferencesPtr spReferences = spFeature->References;

		_sectionCount = spReferences->Count;

		for (int i = 1; i <= _sectionCount; ++i)
		{
			TransCAD::IReferencePtr spReference = spReferences->Item[i];

			cout << "	" << "Name           : " << spReference->Name << endl;
			cout << "	" << "ReferenceeName : " << spReference->ReferenceeName << endl;
			cout << "	" << "Type           : " << spReference->Type << endl;	// 1 : Brep

			_sectionname[i] = spReference->ReferenceeName;
		}

		//cout << " LoftSections-> " << SectionNum<< endl;
		 //	spReferences->Item[]
		//_section = (FSketch*)(GetPart()->GetFeature(spReferences->Item));
		//TransCAD::IReferences sectionzxcvs = spReferences->Item;
		//string SectionSketchName = ;
		//string sketchName = spReferences->Count;
		//cout << " sketch-> " << sketchName.c_str() << endl;
		//_startSection = spFeature.
		//	_startSection = spFeature->
	}

	void FeatureSOLIDCreateSectionsLoft::ToCATIA()
	{
		extern int ref_index;

		//int SectionSketchNum;
		//if(!g_pRefManager->GetFeatureIDByTransCADName(std::string(_profileSketch->GetTransCADFeature()->GetName()), SectionSketchNum))

		int LoftID = 1;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), LoftID))
			return;
		int shapeFactoryID = LoftID + 1;

		//Loft는 여러개더라도 아래 두 줄은 한번만 (shapeFactory)
		fprintf(GetPart()->file, "Dim shapeFactory%d As Factory\n", shapeFactoryID);
		fprintf(GetPart()->file, "Set shapeFactory%d = part1.ShapeFactory\n\n", shapeFactoryID);

		fprintf(GetPart()->file, "Dim loft%d As AnyObject\n", LoftID);
		fprintf(GetPart()->file, "Set loft%d = shapeFactory%d.AddNewLoft()\n\n", LoftID, shapeFactoryID);		//Loft 정의

		fprintf(GetPart()->file, "Dim hybridShapeLoft%d As HybridShape\n", LoftID);
		fprintf(GetPart()->file, "Set hybridShapeLoft%d = loft1.HybridShape\n\n", LoftID);
		fprintf(GetPart()->file, "hybridShapeLoft%d.SectionCoupling = 1\n", LoftID);
		fprintf(GetPart()->file, "hybridShapeLoft%d.Relimitation = 1\n", LoftID);
		fprintf(GetPart()->file, "hybridShapeLoft%d.CanonicalDetection = 2\n", LoftID);

		//Extremum 정의

		for (int i = 1; i <= _sectionCount; ++i) {
			ref_index++;
			hybridShapeExtremumNum++;
			fprintf(GetPart()->file, "Dim hybridShapeDirection%d As HybridShapeDirection\n", hybridShapeExtremumNum);
			fprintf(GetPart()->file, "Set hybridShapeDirection%d = hybridShapeFactory1.AddNewDirectionByCoord(1.000000, 2.000000, 3.000000)\n\n", hybridShapeExtremumNum);
			std::string sectionNameToCAT = _sectionname[i];
			fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
			fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromObject(%s)\n\n", ref_index, sectionNameToCAT.c_str());//여기서 에러발생함.

			fprintf(GetPart()->file, "Dim hybridShapeExtremum%d As HybridShapeExtremum\n", hybridShapeExtremumNum);
			fprintf(GetPart()->file, "Set hybridShapeExtremum%d = hybridShapeFactory%d.AddNewExtremum(reference%d, hybridShapeDirection%d, 1)\n", hybridShapeExtremumNum, shapeFactoryID, ref_index, hybridShapeExtremumNum);
			fprintf(GetPart()->file, "body1.InsertHybridShape hybridShapeExtremum%d\n", hybridShapeExtremumNum);

			fprintf(GetPart()->file, "part1.InWorkObject = hybridShapeExtremum%d\n", hybridShapeExtremumNum);
			fprintf(GetPart()->file, "part1.Update\n\n");
		}


		//SectionNum 개수만큼 반복되야함.
		hybridShapeExtremumNum = 1;
		for (int i = 1; i <= _sectionCount; ++i) {
			ref_index++;

			std::string sectionNameToCAT = _sectionname[i];
			fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
			fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromObject(%s)\n\n", ref_index, sectionNameToCAT.c_str());//여기서 에러발생함.

			ref_index++;
			fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
			fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromObject(hybridShapeExtremum%d)\n\n", ref_index, hybridShapeExtremumNum);
			hybridShapeExtremumNum++;
			fprintf(GetPart()->file, "hybridShapeLoft%d.AddSectionToLoft reference%d, 1, reference%d\n", LoftID, ref_index - 1, ref_index);
		}
		//SectionNum 개수만큼 반복되야함.

		//fprintf(GetPart()->file,"Dim reference%d As Reference\n", ref_index);
		//fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromObject(sketch%d)\n\n", ref_index, guidesktchNum);

		//fprintf(GetPart()->file,"Dim rib%d As Rib\n", ribID);
		//fprintf(GetPart()->file, "Set rib%d = shapeFactory1.AddNewRibFromRef(reference%d, reference%d)\n\n", ribID, ref_index-1, ref_index);
		//fprintf(GetPart()->file, "%s", "part1.Update\n\n");


			//END
		fprintf(GetPart()->file, "part1.InWorkObject = hybridShapeLoft%d", LoftID);
		fprintf(GetPart()->file, "%s", "part1.Update\n\n");

		ref_index++;
	}
}