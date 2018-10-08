#include "stdafx.h"
#include "FeatureSOLIDCreateHoleCounterbored.h"

namespace Post {
	FeatureSOLIDCreateHoleCounterbored::FeatureSOLIDCreateHoleCounterbored(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
	}

	FeatureSOLIDCreateHoleCounterbored::~FeatureSOLIDCreateHoleCounterbored(void)
	{
	}

	void FeatureSOLIDCreateHoleCounterbored::GetInfo()
	{
		TransCAD::IStdSolidHoleCounterboredFeaturePtr spFeature = GetTransCADFeature();
		spFeature->GetCenterPoint(&_centerpoint[0], &_centerpoint[1], &_centerpoint[2]);

		TransCAD::IReferencePtr spReference = spFeature->GetTargetFace();
		_referenceName = spReference->ReferenceeName;

		_startRadius = spFeature->GetStartRadius();
		_endRadius = spFeature->GetEndRadius();
		_startDepth = spFeature->GetStartDepth();
		_endDepth = spFeature->GetEndDepth();
		_bottomAngle = spFeature->GetAngle();

		cout << "   RefFullName : " << _referenceName.c_str() << endl;
		cout << "   CenterPoinit: " << "(" << _centerpoint[0] << "," << _centerpoint[1] << "," << _centerpoint[2] << ")" << endl;
		cout << "   StartRadius : " << _startRadius << endl;
		cout << "   EndRadius   : " << _endRadius << endl;
		cout << "   StartDepth  : " << _startDepth << endl;
		cout << "   EndDepth    : " << _endDepth << endl;
		cout << "   BottomAngle	: " << _bottomAngle << endl;
	}

	void FeatureSOLIDCreateHoleCounterbored::ToCATIA()
	{
		extern int ref_index, lim_index, lng_index, ang_index;
		extern int holecounterID;

		string _facename = g_pRefManager->ConvertToBRepFaceForHole(_referenceName);

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromBRepName(\"%s)\n\n", ref_index, _facename.c_str());

		fprintf(GetPart()->file, "Dim hole%d As Hole\n", holecounterID);
		fprintf(GetPart()->file, "Set hole%d = shapeFactory1.AddNewHoleFromPoint", holecounterID);
		fprintf(GetPart()->file, "(%f, %f, %f, reference%d, %f)\n\n", _centerpoint[0], _centerpoint[1], _centerpoint[2], ref_index, _endDepth);

		fprintf(GetPart()->file, "hole%d.Type = catCounterboredHole\n\n", holecounterID);
		fprintf(GetPart()->file, "hole%d.AnchorMode = catExtremPointHoleAnchor\n\n", holecounterID);

		if (_bottomAngle < 180.0)
			fprintf(GetPart()->file, "hole%d.BottomType = catVHoleBottom\n\n", holecounterID);
		else
			fprintf(GetPart()->file, "hole%d.BottomType = catFlatHoleBottom\n\n", holecounterID);

		fprintf(GetPart()->file, "Dim limit%d As Limit\n", lim_index);
		fprintf(GetPart()->file, "Set limit%d = hole%d.BottomLimit\n\n", lim_index, holecounterID);
		fprintf(GetPart()->file, "limit%d.LimitMode = catOffsetLimit\n\n", lim_index);

		lim_index++;

		fprintf(GetPart()->file, "Dim length%d As Length\n", lng_index);
		fprintf(GetPart()->file, "Set length%d = hole%d.Diameter\n\n", lng_index, holecounterID);
		fprintf(GetPart()->file, "length%d.Value = %f\n\n", lng_index, _endRadius * 2);

		lng_index++;

		fprintf(GetPart()->file, "Dim length%d As Length\n", lng_index);
		fprintf(GetPart()->file, "Set length%d = hole%d.HeadDiameter\n\n", lng_index, holecounterID);
		fprintf(GetPart()->file, "length%d.Value = %f\n\n", lng_index, _startRadius * 2);

		lng_index++;

		fprintf(GetPart()->file, "Dim length%d As Length\n", lng_index);
		fprintf(GetPart()->file, "Set length%d = hole%d.HeadDepth\n\n", lng_index, holecounterID);
		fprintf(GetPart()->file, "length%d.Value = %f\n\n", lng_index, _startDepth);

		lng_index++;

		if (_bottomAngle < 180.0)
		{
			fprintf(GetPart()->file, "Dim angle%d As Angle\n", ang_index);
			fprintf(GetPart()->file, "Set angle%d = hole%d.BottomAngle\n\n", ang_index, holecounterID);
			fprintf(GetPart()->file, "angle%d.Value = %f\n\n", ang_index, _bottomAngle);

			ang_index++;
		}

		fprintf(GetPart()->file, "part1.UpdateObject hole%d\n\n", holecounterID);

		ref_index += 1;
		holecounterID += 1;
	}
}