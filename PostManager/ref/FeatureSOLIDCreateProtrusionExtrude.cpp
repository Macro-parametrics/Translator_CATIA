#include "stdafx.h"
#include "FeatureSOLIDCreateProtrusionExtrude.h"
#include "FSketch.h"

namespace Post {
	FeatureSOLIDCreateProtrusionExtrude::FeatureSOLIDCreateProtrusionExtrude(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
	}

	FeatureSOLIDCreateProtrusionExtrude::~FeatureSOLIDCreateProtrusionExtrude(void)
	{
	}

	void FeatureSOLIDCreateProtrusionExtrude::GetInfo()
	{
		TransCAD::IStdSolidProtrusionExtrudeFeaturePtr spFeature = GetTransCADFeature();

		// StartDepth, EndDepth
		_startDepth = spFeature->StartDepth;
		_endDepth = spFeature->EndDepth;

		// StartCondition, EndCondition
		_startCondition = spFeature->StartCondition;
		_endCondition = spFeature->EndCondition;

		_flip = spFeature->IsFlip ? true : false;	// VARIANT_BOOL (for TransCAD) -> bool Cast

		cout << " startDepth-> " << _startDepth << endl;
		cout << " endDepth-> " << _endDepth << endl;

		TransCAD::IReferencePtr spReference = spFeature->ProfileSketch;
		string sketchName = spReference->ReferenceeName;
		cout << " sketch-> " << sketchName.c_str() << endl;

		_featureProfileSketch = (FSketch*)(GetPart()->GetFeatureByName(sketchName));
	}

	void FeatureSOLIDCreateProtrusionExtrude::ToCATIA()
	{
		extern int lim_index;
		extern int lng_index;

		int padsktchNum;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_featureProfileSketch->GetTransCADFeature()->GetName()), padsktchNum))
			return;

		int padID = 1;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), padID))
			return;

		fprintf(GetPart()->file, "Dim pad%d As Pad\n", padID);
		fprintf(GetPart()->file, "Set pad%d = shapeFactory1.AddNewPad(sketch%d, 0.0)\n\n", padID, padsktchNum);

		// First Limit 설정
		fprintf(GetPart()->file, "Dim limit%d As Limit\n", lim_index);
		fprintf(GetPart()->file, "Set limit%d = pad%d.FirstLimit\n\n", lim_index, padID);

		if (_startCondition == TransCAD::ThroughAll)
			fprintf(GetPart()->file, "limit%d.LimitMode = catUpToLastLimit\n\n", lim_index);
		else if (_startCondition == TransCAD::Blind)
			fprintf(GetPart()->file, "limit%d.LimitMode = catOffsetLimit\n\n", lim_index);

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
		fprintf(GetPart()->file, "Set limit%d = pad%d.SecondLimit\n\n", lim_index, padID);

		if (_endCondition == TransCAD::ThroughAll)
			fprintf(GetPart()->file, "limit%d.LimitMode = catUpToLastLimit\n\n", lim_index);
		else if (_endCondition == TransCAD::Blind)
			fprintf(GetPart()->file, "limit%d.LimitMode = catOffsetLimit\n\n", lim_index);

		if (_endDepth > 0.0)
		{
			fprintf(GetPart()->file, "Dim length%d As Length\n", lng_index);
			fprintf(GetPart()->file, "Set length%d = limit%d.Dimension\n\n", lng_index, lim_index);
			fprintf(GetPart()->file, "length%d.Value = %lf\n\n", lng_index, _endDepth);

			lng_index++;
		}

		lim_index++;

		if (_flip)
			fprintf(GetPart()->file, "pad%d.DirectionOrientation = catInverseOrientation\n\n", padID);
		else
			fprintf(GetPart()->file, "pad%d.DirectionOrientation = catRegularOrientation\n\n", padID);

		fprintf(GetPart()->file, "part1.UpdateObject pad%d\n\n", padID);
	}

}