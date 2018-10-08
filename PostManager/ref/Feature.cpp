#include "stdafx.h"
#include "Feature.h"

namespace Post {
	Feature::Feature(Part * pPart, TransCAD::IFeaturePtr spFeature)
	{
		_pPart = pPart;									// Set Part ptr
		_spFeature = spFeature;							// Set TransCAD Feature ptr
		_nIndex = pPart->GetSize();						// Set index

		if (spFeature)
		{
			_strName = spFeature->Name;					// Set A Feature's Name
			cout << "Feature Name : " << _strName;			//DEBUG
		}
	}

	Feature::~Feature(void)
	{
	}
}