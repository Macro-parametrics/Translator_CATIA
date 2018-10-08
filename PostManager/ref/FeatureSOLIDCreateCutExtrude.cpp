#include "stdafx.h"
#include "FeatureSOLIDCreateCutExtrude.h"
#include "FSketch.h"
namespace Post {
	FeatureSOLIDCreateCutExtrude::FeatureSOLIDCreateCutExtrude(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
	}

	FeatureSOLIDCreateCutExtrude::~FeatureSOLIDCreateCutExtrude(void)
	{
	}

	void FeatureSOLIDCreateCutExtrude::GetInfo()
	{
		TransCAD::IStdSolidCutExtrudeFeaturePtr spFeature = GetTransCADFeature();

		_startDepth = spFeature->StartDepth;
		_endDepth = spFeature->EndDepth;

		_startCondition = spFeature->StartCondition;
		_endCondition = spFeature->EndCondition;

		_flip = spFeature->IsFlip ? true : false;

		cout << " startDepth-> " << _startDepth << endl;
		cout << " endDepth-> " << _endDepth << endl;

		TransCAD::IReferencePtr spReference = spFeature->ProfileSketch;
		string sketchName = spReference->ReferenceeName;
		cout << " sketch-> " << sketchName.c_str() << endl;

		_featureProfileSketch = (FSketch*)(GetPart()->GetFeatureByName(sketchName));
	}

	void FeatureSOLIDCreateCutExtrude::ToCATIA()
	{
		extern int lim_index;
		extern int lng_index;

		int pocketsktchNum;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_featureProfileSketch->GetTransCADFeature()->GetName()), pocketsktchNum))
			return;

		int pocketID = 1;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), pocketID))
			return;

		fprintf(GetPart()->file, "Dim pocket%d As Pocket\n", pocketID);
		fprintf(GetPart()->file, "Set pocket%d = shapeFactory1.AddNewPocket(sketch%d, 0.0)\n\n", pocketID, pocketsktchNum);

		// First Limit 설정
		fprintf(GetPart()->file, "Dim limit%d As Limit\n", lim_index);
		fprintf(GetPart()->file, "Set limit%d = pocket%d.FirstLimit\n\n", lim_index, pocketID);

		if (_startCondition == TransCAD::Blind)
			fprintf(GetPart()->file, "limit%d.LimitMode = catOffsetLimit\n\n", lim_index);
		else if (_startCondition == TransCAD::ThroughAll)
			fprintf(GetPart()->file, "limit%d.LimitMode = catUpToLastLimit\n\n", lim_index);

		if (_startDepth > 0.0)
		{
			fprintf(GetPart()->file, "Dim length%d As Length\n", lng_index);
			fprintf(GetPart()->file, "Set length%d = limit%d.Dimension\n\n", lng_index, lim_index);
			fprintf(GetPart()->file, "length%d.Value = %lf\n\n", lng_index, _startDepth);

			lng_index++;
		}

		lim_index++;

		// Second Limit 설정
		fprintf(GetPart()->file, "Dim limit%d As Limit\n", lim_index);
		fprintf(GetPart()->file, "Set limit%d = pocket%d.SecondLimit\n\n", lim_index, pocketID);

		if (_endCondition == TransCAD::Blind)
			fprintf(GetPart()->file, "limit%d.LimitMode = catOffsetLimit\n\n", lim_index);
		else if (_endCondition == TransCAD::ThroughAll)
			fprintf(GetPart()->file, "limit%d.LimitMode = catUpToLastLimit\n\n", lim_index);

		if (_endDepth > 0.0)
		{
			fprintf(GetPart()->file, "Dim length%d As Length\n", lng_index);
			fprintf(GetPart()->file, "Set length%d = limit%d.Dimension\n\n", lng_index, lim_index);
			fprintf(GetPart()->file, "length%d.Value = %lf\n\n", lng_index, _endDepth);

			lng_index++;
		}

		lim_index++;

		// 방향 설정
		if (_flip)
			fprintf(GetPart()->file, "pocket%d.DirectionOrientation = catRegularOrientation\n\n", pocketID);
		else
			fprintf(GetPart()->file, "pocket%d.DirectionOrientation = catInverseOrientation\n\n", pocketID);

		fprintf(GetPart()->file, "part1.UpdateObject pocket%d\n\n", pocketID);
	}
}