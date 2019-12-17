// CATIApre_DLL.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "CATIApre_DLL.h"
#include <iostream>
#include "Part.h"



//extern "C" __declspec(dllexport) int CATIAPreStartService(void);    //입출력 숫자형
//extern "C" __declspec(dllexport) int CATIAPreTranslate(char * path);  //입출력 문자열형

namespace Pre {
	
	TransCAD::IApplicationPtr g_spApplication;

	// 내보낸 함수의 예제입니다.
	int CATIAPreStartService()
	{

		//g_pRefManager = new CATIAReferenceManager();

		int status = true;
		HRESULT hr = ::CoInitialize(NULL);		// COM Automation API Initialization
		if (FAILED(hr)) {

			return status;
		}


		// create TransCAD
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		if (FAILED(hr)) status = 0;

		// visualize TransCAD
		//g_spApplication->Visible = VARIANT_TRUE;

		return status;

	}

	int CATIAPreTranslate(char * path)
	{

		CATIAPreStartService();
		//int status = true;

		Part* pPart = new Part(path);

		pPart->GetInfo();
		pPart->ToTransCAD();

		// 어셈블리 변환을 위한 정보. 파트 문서의 순서 정보
		int partDocNum = pPart->GetPartDocNumber();

		delete pPart;

		::CoUninitialize();

		return partDocNum;
	}

	int CATIAPreTranslateAssem(int totPartNum, int partDocNum[], double partCoords[][12])
	{
		// 어셈블리 문서 생성
		CATIAPreStartService();
		TransCAD::IAssemDocumentPtr _spAssemDocument;
		TransCAD::IAssemPtr _spAssem;
		TransCAD::IComponentPtr _spComp;
		TransCAD::IPartDocumentPtr	_spDocument;
		TransCAD::IPartPtr			_spPart;

		_spAssemDocument = g_spApplication->GetDocuments()->AddAssemDocument();
		_spAssem = _spAssemDocument->GetAssem();

		// 이미 생성된 파트 문서를 하나씩 가져와서 어셈블리 문서에 추가/배치 
		for (int i = 1; i <= totPartNum; ++i) {

			_spDocument = g_spApplication->GetDocuments()->GetItem(partDocNum[i - 1]);
			_spPart = _spDocument->Part;
			_spComp = _spAssem->CreateComponent();
			_spComp->Name = _spPart->GetName();
			_spComp->AddPart(_spPart);

			for (int j = 0; j < 12; ++j)
			{
				_spComp->SetPartPlacement(_spPart, partCoords[i - 1][9], partCoords[i - 1][10], partCoords[i - 1][11],
					partCoords[i - 1][6], partCoords[i - 1][7], partCoords[i - 1][8],
					partCoords[i - 1][0], partCoords[i - 1][1], partCoords[i - 1][2]);
			}
			_spAssem->AddComponent(_spComp);
		}

		return true;
	}
}