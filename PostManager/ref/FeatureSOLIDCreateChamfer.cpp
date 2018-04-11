#include "stdafx.h"
#include "FeatureSOLIDCreateChamfer.h"


FeatureSOLIDCreateChamfer::FeatureSOLIDCreateChamfer(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart,spFeature)
{
}

FeatureSOLIDCreateChamfer::~FeatureSOLIDCreateChamfer(void)
{
}

void FeatureSOLIDCreateChamfer::GetInfo(void)
{
	TransCAD::IStdSolidChamferFeaturePtr spFeature = GetTransCADFeature();
	TransCAD::IReferencesPtr spEdges = spFeature->SelectedEdges;

	_edgeCount = spEdges->Count;
	_length = spFeature->Length;

	for (int i = 1; i <= _edgeCount; ++i)
	{
		TransCAD::IReferencePtr spReference = spEdges->GetItem(i);
		cout << "	" << "Name           : " << spReference->Name << endl;
		cout << "	" << "ReferenceeName : " << spReference->ReferenceeName << endl;
		cout << "	" << "Type           : " << spReference->Type << endl;	// 1 : Brep
		
		_edgeNames.push_back((string)spReference->ReferenceeName);
	}
 }

void FeatureSOLIDCreateChamfer::ToCATIA(void)
{
	extern int ref_index;
	
	fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
	fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromName(\"\")\n\n", ref_index);

	int chamferID = 1;
	if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), chamferID))
		return;

	fprintf(GetPart()->file, "Dim chamfer%d As Chamfer\n", chamferID);
	fprintf(GetPart()->file, "Set chamfer%d = shapeFactory1.AddNewChamfer(reference%d", chamferID, ref_index);
	fprintf(GetPart()->file, ", catMinimalChamfer, catTwoLengthChamfer, catNoReverseChamfer, %f, %f)\n\n", _length, _length);

	// 다수의 Edge를 선택 하였을 시에, 여러 개의 필렛 추가.
	for (int i = 1; i <= _edgeCount; ++i) 
	{
		ref_index++;

		std::string edge_name = g_pRefManager->ConvertToBRepEdge(_edgeNames[i-1]);
	
		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromBRepName(\"%s)\n\n", ref_index, edge_name.c_str());

		fprintf(GetPart()->file, "chamfer%d.AddElementToChamfer reference%d\n\n", chamferID, ref_index);
		fprintf(GetPart()->file, "chamfer%d.Propagation = catMinimalChamfer\n\n", chamferID);
		fprintf(GetPart()->file, "chamfer%d.Mode = catTwoLengthChamfer\n\n", chamferID);
		fprintf(GetPart()->file, "chamfer%d.Orientation = catNoReverseChamfer\n\n", chamferID);
	}

 	fprintf(GetPart()->file, "part1.UpdateObject chamfer%d\n\n", chamferID);
	
	ref_index += 1;
}


