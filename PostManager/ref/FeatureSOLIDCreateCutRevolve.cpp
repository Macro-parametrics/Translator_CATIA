#include "stdafx.h"
#include "FeatureSOLIDCreateCutRevolve.h"
#include "FSketch.h"


FeatureSOLIDCreateCutRevolve::FeatureSOLIDCreateCutRevolve(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart,spFeature)
{
}

FeatureSOLIDCreateCutRevolve::~FeatureSOLIDCreateCutRevolve(void)
{
}

void FeatureSOLIDCreateCutRevolve::GetInfo()
{
	TransCAD::IStdSolidCutRevolveFeaturePtr spFeature = GetTransCADFeature();
	double _angle;

	_endAngle = spFeature->StartAngle;
	_startAngle = spFeature->EndAngle;
	_angle = _startAngle-_endAngle;

	if (abs(_angle) > 360.0)
		_angle = 360.0;

	_flip = spFeature->IsFlip?true:false;
	
	cout << " startAngle-> " << _startAngle << endl;
	cout << " endAngle-> " << _endAngle << endl;

	TransCAD::IReferencePtr spReference = spFeature->ProfileSketch;
	string sketchName = spReference->ReferenceeName;
	cout << " sketch-> " << sketchName.c_str() <<endl;

	cout << " revolveAngle : " << (abs(_angle)/180*PI) << endl;
	cout << " flip : " << _flip <<endl;
	
	_featureProfileSketch = (FSketch*)(GetPart()->GetFeatureByName(sketchName));
}

void FeatureSOLIDCreateCutRevolve::ToCATIA()
{
	extern int ang_index;
	
	int cutrevsktchNum;
	if(!g_pRefManager->GetFeatureIDByTransCADName(std::string(_featureProfileSketch->GetTransCADFeature()->GetName()), cutrevsktchNum))
		return;

	int revolveID = 1;
	if(!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), revolveID))
		return;

	fprintf(GetPart()->file, "Dim groove%d As Groove\n", revolveID);
	fprintf(GetPart()->file, "Set groove%d = shapeFactory1.AddNewGroove(sketch%d)\n\n", revolveID, cutrevsktchNum);

	fprintf(GetPart()->file, "Dim angle%d As Angle\n", ang_index);
	fprintf(GetPart()->file, "Set angle%d = groove%d.FirstAngle\n\n", ang_index, revolveID);
	
	if (!_flip)
		fprintf(GetPart()->file, "angle%d.Value = %lf\n\n", ang_index, _startAngle);
	else
		fprintf(GetPart()->file, "angle%d.Value = %lf\n\n", ang_index, _endAngle);

	ang_index++;

	fprintf(GetPart()->file, "Dim angle%d As Angle\n", ang_index);
	fprintf(GetPart()->file, "Set angle%d = groove%d.SecondAngle\n\n", ang_index, revolveID);
	
	if (!_flip)
		fprintf(GetPart()->file, "angle%d.Value = %lf\n\n", ang_index, _endAngle);
	else
		fprintf(GetPart()->file, "angle%d.Value = %lf\n\n", ang_index, _startAngle);

	ang_index++;

	fprintf(GetPart()->file, "part1.UpdateObject groove%d\n\n", revolveID);
}