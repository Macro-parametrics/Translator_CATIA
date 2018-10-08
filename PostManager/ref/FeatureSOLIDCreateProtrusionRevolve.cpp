#include "stdafx.h"
#include "FeatureSOLIDCreateProtrusionRevolve.h"
#include "FSketch.h"

namespace Post {
	FeatureSOLIDCreateProtrusionRevolve::FeatureSOLIDCreateProtrusionRevolve(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
	}

	FeatureSOLIDCreateProtrusionRevolve::~FeatureSOLIDCreateProtrusionRevolve(void)
	{
	}

	void FeatureSOLIDCreateProtrusionRevolve::GetInfo()
	{
		TransCAD::IStdSolidProtrusionRevolveFeaturePtr spFeature = GetTransCADFeature();

		double _angle;

		_endAngle = spFeature->StartAngle;
		_startAngle = spFeature->EndAngle;
		_angle = _startAngle - _endAngle;

		if (abs(_angle) > 360.0)
			_angle = 360.0;

		_flip = spFeature->IsFlip ? true : false;

		cout << " startAngle-> " << _startAngle << endl;
		cout << " endAngle-> " << _endAngle << endl;

		TransCAD::IReferencePtr spReference = spFeature->ProfileSketch;
		string sketchName = spReference->ReferenceeName;
		cout << " sketch-> " << sketchName.c_str() << endl;

		cout << " revolveAngle : " << (abs(_angle) / 180 * PI) << endl;
		cout << " flip : " << _flip << endl;

		_featureProfileSketch = (FSketch*)(GetPart()->GetFeatureByName(sketchName));
	}

	void FeatureSOLIDCreateProtrusionRevolve::ToCATIA()
	{
		extern int ang_index;

		int prorevsktchNum;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_featureProfileSketch->GetTransCADFeature()->GetName()), prorevsktchNum))
			return;

		int revolveID = 1;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), revolveID))
			return;

		fprintf(GetPart()->file, "Dim shaft%d As Shaft\n", revolveID);
		fprintf(GetPart()->file, "Set shaft%d = shapeFactory1.AddNewShaft(sketch%d)\n\n", revolveID, prorevsktchNum);

		fprintf(GetPart()->file, "Dim angle%d As Angle\n", ang_index);
		fprintf(GetPart()->file, "Set angle%d = shaft%d.FirstAngle\n\n", ang_index, revolveID);

		if (!_flip)
			fprintf(GetPart()->file, "angle%d.Value = %lf\n\n", ang_index, _startAngle);
		else
			fprintf(GetPart()->file, "angle%d.Value = %lf\n\n", ang_index, _endAngle);

		ang_index++;

		fprintf(GetPart()->file, "Dim angle%d As Angle\n", ang_index);
		fprintf(GetPart()->file, "Set angle%d = shaft%d.SecondAngle\n\n", ang_index, revolveID);

		if (!_flip)
			fprintf(GetPart()->file, "angle%d.Value = %lf\n\n", ang_index, _endAngle);
		else
			fprintf(GetPart()->file, "angle%d.Value = %lf\n\n", ang_index, _startAngle);

		ang_index++;

		fprintf(GetPart()->file, "part1.UpdateObject shaft%d\n\n", revolveID);
	}
}