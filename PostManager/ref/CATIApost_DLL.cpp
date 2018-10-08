// dllCATIApost.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "CATIApost_DLL.h"
#include "Part.h"
#include "FSKETCH.h"
#include <iostream>
#include <fstream>
#include "CATIAReferenceManager.h"



	// 내보낸 변수의 예제입니다.
	

	int ref_index = 1;
	int repname_index = 1;
	int pt_index = 1;
	int ln_index = 1;
	int arr_index = 1;
	int fac_index = 1;
	int lim_index = 1;
	int lng_index = 1;
	int circle_index = 1;
	int sktch = 1;
	int holecounterID = 1;
	int ang_index = 1;
	int DatumPlaneID = 1;
namespace Post {
	TransCAD::IApplicationPtr g_spApplication;
	CATIAReferenceManager *g_pRefManager = NULL;

	// 내보낸 함수의 예제입니다.
	int CATIAPostStartService()
	{
		g_pRefManager = new CATIAReferenceManager();

		int status = true;

		HRESULT hr = ::CoInitialize(NULL);		// COM Automation API Initialization
		if (FAILED(hr)) {

			return status;
		}

		// create TransCAD
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));

		if (FAILED(hr))
			status = 0;

		// visualize TransCAD
		//g_spApplication->Visible = VARIANT_TRUE;

		return status;

	}

	int CATIAPostTranslate(char argv[])
	{
		int status = true;

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

		Part * pPart;

		if (argv == "")
			pPart = new Part();
		else
			pPart = new Part(argv);

		if (g_spApplication->ActiveDocument != NULL)
		{
			pPart->GetInfo();
			pPart->ToCATIA();

			//delete Part class
			delete pPart;
		}
		else
		{
			//g_spApplication->Visible = VARIANT_FALSE;
			status = false;
		}

		// Clear TransCAD
		//g_spApplication.Release();

		::CoUninitialize();						// COM Automation API Uninitialization

		if (g_pRefManager)
			delete g_pRefManager;

		return status;

	}
}