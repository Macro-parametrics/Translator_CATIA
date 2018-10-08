#include "stdafx.h"
#include "FeatureSOLIDCreateFilletConstant.h"
namespace Post {

	FeatureSOLIDCreateFilletConstant::FeatureSOLIDCreateFilletConstant(Part * pPart, TransCAD::IFeaturePtr spFeature)
		: Feature(pPart, spFeature)
	{
	}

	FeatureSOLIDCreateFilletConstant::~FeatureSOLIDCreateFilletConstant(void)
	{
	}

	void FeatureSOLIDCreateFilletConstant::GetInfo()
	{
		TransCAD::IStdSolidFilletConstantFeaturePtr spFeature = GetTransCADFeature();
		_radius = spFeature->Radius;
		_propagationType = spFeature->GetPropagation();

		TransCAD::IReferencesPtr spReferences = spFeature->FilletEdges;
		_edgeCount = spReferences->Count;

		for (int i = 1; i <= _edgeCount; ++i)
		{
			TransCAD::IReferencePtr spReference = spReferences->GetItem(i);
			cout << "	" << "Name           : " << spReference->Name << endl;
			cout << "	" << "ReferenceeName : " << spReference->ReferenceeName << endl;
			cout << "	" << "Type           : " << spReference->Type << endl;	// 1 : Brep

			_edgeNames.push_back((string)spReference->ReferenceeName);
		}
	}

	void FeatureSOLIDCreateFilletConstant::ToCATIA()
	{
		extern int ref_index;

		fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
		fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromName(\"\")\n\n", ref_index);

		int filletID = 1;
		if (!g_pRefManager->GetFeatureIDByTransCADName(std::string(_spFeature->GetName()), filletID))
			return;

		fprintf(GetPart()->file, "Dim constRadEdgeFillet%d As ConstRadEdgeFillet\n", filletID);
		fprintf(GetPart()->file, "Set constRadEdgeFillet%d = shapeFactory1.AddNewEdgeFilletWithConstantRadius(reference%d", filletID, ref_index);
		fprintf(GetPart()->file, ", catTangencyFilletEdgePropagation, %f)\n\n", _radius);

		// 다수의 Edge를 선택 하였을 시에, 여러 개의 필렛 추가.
		for (int i = 1; i <= _edgeCount; ++i)
		{
			ref_index++;

			std::string edge_name = g_pRefManager->ConvertToBRepEdge(_edgeNames[i - 1]);

			fprintf(GetPart()->file, "Dim reference%d As Reference\n", ref_index);
			fprintf(GetPart()->file, "Set reference%d = part1.CreateReferenceFromBRepName(\"%s)\n\n", ref_index, edge_name.c_str());
			fprintf(GetPart()->file, "constRadEdgeFillet%d.AddObjectToFillet reference%d\n\n", filletID, ref_index);

			if (_propagationType == TransCAD::Minimal)
				fprintf(GetPart()->file, "constRadEdgeFillet%d.EdgePropagation = catMinimalFilletEdgePropagation\n\n", filletID);
			else if (_propagationType == TransCAD::Tangency)
				fprintf(GetPart()->file, "constRadEdgeFillet%d.EdgePropagation = catTangencyFilletEdgePropagation\n\n", filletID);
		}

		fprintf(GetPart()->file, "part1.UpdateObject constRadEdgeFillet%d\n\n", filletID);

		ref_index++;
	}
}