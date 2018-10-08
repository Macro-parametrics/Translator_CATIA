#include "stdafx.h"
#include "FeatureSOLIDCreateProtrusionSweep.h"
#include "FSketch.h"
namespace Post {

	FeatureSOLIDCreateProtrusionSweep::FeatureSOLIDCreateProtrusionSweep(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
	}

	FeatureSOLIDCreateProtrusionSweep::~FeatureSOLIDCreateProtrusionSweep(void)
	{
	}

	void FeatureSOLIDCreateProtrusionSweep::GetInfo()
	{
		TransCAD::IStdSolidProtrusionSweepFeaturePtr spFeature = GetTransCADFeature();
		TransCAD::IReferencePtr spProfile = spFeature->ProfileSketch;
		TransCAD::IReferencePtr spGuide = spFeature->GuideCurve;

		string profileName = spProfile->ReferenceeName;
		string guideName = spGuide->ReferenceeName;

		cout << " sweep profilesketch name = " << profileName << endl;
		cout << " sweep guidesketch name = " << guideName << endl;

		_profileSketch = (FSketch*)(GetPart()->GetFeatureByName(profileName));
		_guideSketch = (FSketch*)(GetPart()->GetFeatureByName(guideName));
	}

	void FeatureSOLIDCreateProtrusionSweep::ToCATIA()
	{
		extern int ref_index;

		int profilesktchNum;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_profileSketch->GetTransCADFeature()->GetName()), profilesktchNum))
			return;

		int	guidesktchNum;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_guideSketch->GetTransCADFeature()->GetName()), guidesktchNum))
			return;

		int ribID = 1;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), ribID))
			return;

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromObject(sketch%d)\n\n", ref_index, profilesktchNum);

		ref_index += 1;

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromObject(sketch%d)\n\n", ref_index, guidesktchNum);

		fprintf(GetPart()->file, "Dim rib%d As Rib\n", ribID);
		fprintf(GetPart()->file, "Set rib%d = shapeFactory1.AddNewRibFromRef(reference%d, reference%d)\n\n", ribID, ref_index - 1, ref_index);
		fprintf(GetPart()->file, "part1.UpdateObject rib%d\n\n", ribID);

		ref_index++;
	}
}