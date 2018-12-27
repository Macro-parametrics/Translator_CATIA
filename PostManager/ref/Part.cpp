#include "stdafx.h"
#include "Part.h"

#include "Feature.h"
#include "FSketch.h"
#include "FeatureSOLIDCreateProtrusionExtrude.h"
#include "FeatureSOLIDCreateCutExtrude.h"
#include "FeatureSOLIDCreateFilletConstant.h"
#include "FeatureSOLIDCreateProtrusionSweep.h"
#include "FeatureSOLIDCreateProtrusionRevolve.h"
#include "FeatureSOLIDCreateCutRevolve.h"
#include "FeatureSOLIDCreateChamfer.h"
#include "FDatumPlane.h"
#include "FDatumPlaneOffset.h"
#include "FeatureSOLIDCreatePatternRectangular.h"
#include "FeatureSOLIDCreatePatternCircular.h"
#include "FeatureSOLIDCreateHoleCounterbored.h"
#include "FeatureSOLIDCreateHoleCountersunk.h"
#include "FeatureSOLIDCreateFacesLoft.h"
#include "FeatureSOLIDCreateSectionsLoft.h"

	extern int ExternMode;
	
	extern int ref_index;
	extern int repname_index;
	extern int pt_index;
	extern int ln_index;
	extern int arr_index;
	extern int fac_index;
	extern int lim_index;
	extern int lng_index;
	extern int circle_index;
	extern int sktch;
	extern int holecounterID;
	extern int ang_index;
	extern int DatumPlaneID;

namespace Post {

	extern CATIAReferenceManager *g_pRefManager;

	Part::Part() //used to make Part class array
	{
	}


	Part::Part(string fileName) //Basic Part Constructor : For a Part Translation TransCAD Document must be PartDocument
								//filename : fullpath
	{
		// Set TransCAD Active Document Ptr
		/*_spDocument = g_spApplication->ActiveDocument;

		if (_spDocument == NULL) 
		{
			cout << " spDocument is Null " << endl;
			return;
		}*/

		_spPart = _spDocument->GetPart();		// Set TransCAD Part ptr
		_spFeatures = _spPart->Features;	// Set TransCAD Features ptr

											// 최종 CATIA 매크로 파일이 위치할 경로
		fopen_s(&file, fileName.c_str(), "wt");

		_strPartName = fileName;
		/////////////////////////////////////////////////////////////////////////////////
		fprintf(file, "Language = \"VBSCRIPT\"\n\n");
		fprintf(file, "Sub CATMain()\n\n");

		fprintf(file, "Dim documents1 As Documents\n");
		fprintf(file, "Set documents1 = CATIA.Documents\n\n");

		fprintf(file, "Dim partDocument1 As Document\n");
		fprintf(file, "Set partDocument1 = documents1.Add(\"Part\")\n\n");

		fprintf(file, "Dim part1 As Part\n");
		fprintf(file, "Set part1 = partDocument1.Part\n\n");

		fprintf(file, "Dim bodies1 As Bodies\n");
		fprintf(file, "Set bodies1 = part1.Bodies\n\n");

		fprintf(file, "Dim body1 As Body\n");
		fprintf(file, "Set body1 = bodies1.Item(\"PartBody\")\n\n");

		fprintf(file, "Dim sketches1 As Sketches\n");
		fprintf(file, "Set sketches1 = body1.Sketches\n\n");

		fprintf(file, "Dim originElements1 As OriginElements\n");
		fprintf(file, "Set originElements1 = part1.OriginElements\n\n");

		fprintf(file, "Dim shapeFactory1 As Factory\n");
		fprintf(file, "Set shapeFactory1 = part1.ShapeFactory\n\n");
		/////////////////////////////////////////////////////////////////////////////////
	}

	Part::Part(TransCAD::IPartPtr _p, string fileName)//used Assembly Part : if you already have TransCAD Part ptr(IPartPtr) and you need to make a Script, use this constructor
												      //TransCAD Document is not Part Document = AssemblyDoccument
													  //filename : fullpath
	{

		_spPart = _p;						// Set TransCAD Part ptr
		_spFeatures = _spPart->Features;	// Set TransCAD Features ptr

		// 최종 CATIA 매크로 파일이 위치할 경로
		fopen_s(&file, fileName.c_str(), "wt");

		_strPartName = fileName;
		/////////////////////////////////////////////////////////////////////////////////
		fprintf(file, "Language = \"VBSCRIPT\"\n\n");
		fprintf(file, "Sub CATMain()\n\n");

		fprintf(file, "Dim documents1 As Documents\n");
		fprintf(file, "Set documents1 = CATIA.Documents\n\n");

		fprintf(file, "Dim partDocument1 As Document\n");
		fprintf(file, "Set partDocument1 = documents1.Add(\"Part\")\n\n");

		fprintf(file, "Dim part1 As Part\n");
		fprintf(file, "Set part1 = partDocument1.Part\n\n");

		fprintf(file, "Dim bodies1 As Bodies\n");
		fprintf(file, "Set bodies1 = part1.Bodies\n\n");

		fprintf(file, "Dim body1 As Body\n");
		fprintf(file, "Set body1 = bodies1.Item(\"PartBody\")\n\n");

		fprintf(file, "Dim sketches1 As Sketches\n");
		fprintf(file, "Set sketches1 = body1.Sketches\n\n");

		fprintf(file, "Dim originElements1 As OriginElements\n");
		fprintf(file, "Set originElements1 = part1.OriginElements\n\n");

		fprintf(file, "Dim shapeFactory1 As Factory\n");
		fprintf(file, "Set shapeFactory1 = part1.ShapeFactory\n\n");
		/////////////////////////////////////////////////////////////////////////////////
	}

	

	Part::Part(TransCAD::IPartPtr _p, int f = 0) //used Assembly Part : if you already have TransCAD Part ptr(IPartPtr) and you don't need to make a Script, use this constructor
												 //TransCAD Document is not Part Document = AssemblyDoccument
	{
		_spPart = _p;						// Set TransCAD Part ptr
		_spFeatures = _spPart->Features;	// Set TransCAD Features ptr
	}

	

	Part::~Part(void)
	{
		fprintf(file, "part1.Update\n\n");
		fprintf(file, "End Sub\n");
		fclose(file);

		Clear();

		// Clear TransCAD
		_spDocument = NULL;
		_spPart = NULL;
		_spFeatures = NULL;
		g_spApplication = NULL;
	}

	void Part::Clear()
	{
		std::vector<Feature *>::iterator i = _featureList.begin();
		while (i != _featureList.end())
		{
			delete *i;
			++i;
		}

		_featureList.clear();
	}

	Feature * Part::GetFeatureByName(string name)
	{
		for (size_t i = 0; (unsigned int)i < GetSize(); ++i)
		{
			Feature * pFeature = GetFeature((int)i);

			if (name == pFeature->GetFeatureName())
				return pFeature;
		}

		return NULL;
	}

	Feature * Part::CreateFeature(TransCAD::IFeaturePtr spFeature)
	{
		Feature * pFeature = 0;

		g_pRefManager->AddNewFeature(spFeature);

		switch (spFeature->Type)
		{
		case TransCAD::StdSketchFeature:
			pFeature = new FSketch(this, spFeature);
			cout << " -> " << "FSketch created" << endl;
			break;

		case TransCAD::StdSolidProtrusionExtrudeFeature:
			pFeature = new FeatureSOLIDCreateProtrusionExtrude(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateProtrusionExtrude created" << endl;
			break;

		case TransCAD::StdSolidCutExtrudeFeature:
			pFeature = new FeatureSOLIDCreateCutExtrude(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateCutExtrude created" << endl;
			break;

		case TransCAD::StdSolidFilletConstantFeature:
			pFeature = new FeatureSOLIDCreateFilletConstant(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateFilletConstant created" << endl;
			break;

		case TransCAD::StdDefaultDatumPlaneFeature:
			pFeature = new FDatumPlane(this, spFeature);
			cout << " -> " << "FDatumPlane created" << endl;
			break;

		case TransCAD::StdSolidProtrusionSweepFeature:
			pFeature = new FeatureSOLIDCreateProtrusionSweep(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateProtrusionSweep created" << endl;
			break;

		case TransCAD::StdSolidProtrusionRevolveFeature:
			pFeature = new FeatureSOLIDCreateProtrusionRevolve(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateProtrusionRevolve created" << endl;
			break;

		case TransCAD::StdSolidChamferFeature:
			pFeature = new FeatureSOLIDCreateChamfer(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateChamfer created" << endl;
			break;

		case TransCAD::StdSolidCutRevolveFeature:
			pFeature = new FeatureSOLIDCreateCutRevolve(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateCutRevolve created" << endl;
			break;

		case TransCAD::StdDatumPlaneOffsetFeature:
			pFeature = new FDatumPlaneOffset(this, spFeature);
			cout << " -> " << "FDatumPlaneOffset created" << endl;
			break;

		case TransCAD::StdSolidOperatePatternRectangularFeature:
			pFeature = new FeatureSOLIDCreatePatternRectangular(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreatePatternRectangular created" << endl;
			break;

		case TransCAD::StdSolidOperatePatternCircularFeature:
			pFeature = new FeatureSOLIDCreatePatternCircular(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreatePatternCircular created" << endl;
			break;

		case TransCAD::StdSolidHoleCounterboredFeature:
			pFeature = new FeatureSOLIDCreateHoleCounterbored(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateHoleCounterbored created" << endl;
			break;

		case TransCAD::StdSolidHoleCountersunkFeature:
			pFeature = new FeatureSOLIDCreateHoleCountersunk(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateHoleCountersunk created" << endl;
			break;

		case TransCAD::StdSolidLoftFacesFeature:
			pFeature = new FeatureSOLIDCreateFacesLoft(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateFacesLoft created" << endl;
			break;

		case TransCAD::StdSolidLoftSectionsFeature:
			pFeature = new FeatureSOLIDCreateSectionsLoft(this, spFeature);
			cout << " -> " << "FeatureSOLIDCreateSectionsLoft created" << endl;
			break;
		}

		if (pFeature)
			_featureList.push_back(pFeature);

		return pFeature;
	}

	void Part::GetInfo()
	{
		//-------  Create features existing in TransCAD and save it into _featureList  --------//
		for (int i = 1; i <= _spFeatures->Count; ++i)
		{
			Feature * pFeature = CreateFeature(_spFeatures->Item[i]);

			if (!pFeature)
				cerr << "** Cannot support this feature type yet! **" << endl;
		}

		//-------  Get TransCAD Features information into _featureList  --------//
		for (int i = 0; (unsigned int)i < GetSize(); ++i)
		{
			cout << "Index(" << i << ") " << (GetFeature(i))->GetFeatureName() << " feature is being translated." << endl;	//DEBUG

			(GetFeature(i))->GetInfo();
		}
	}

	void Part::ToCATIA()
	{
		//-------  Get TransCAD Features information into _featureList  --------//
		for (int i = 0; (unsigned int)i < GetSize(); ++i)
		{
			cout << "Index(" << i << ") " << (GetFeature(i))->GetFeatureName() << " feature is being translated." << endl;	//DEBUG
			(GetFeature(i))->ToCATIA();
		}
	}

	void Part::ExternModeToggle(int mode) {

		ExternMode = mode;
	}

	void Part::InitReferManager() {

		//g_pRefManager->~CATIAReferenceManager();
		ref_index = 1;
		repname_index = 1;
		pt_index = 1;
		ln_index = 1;
		arr_index = 1;
		fac_index = 1;
		lim_index = 1;
		lng_index = 1;
		circle_index = 1;
		sktch = 1;
		holecounterID = 1;
		ang_index = 1;
		DatumPlaneID = 1;
	}
}