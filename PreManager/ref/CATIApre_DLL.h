// 다음 ifdef 블록은 DLL에서 내보내기하는 작업을 쉽게 해 주는 매크로를 만드는 
// 표준 방식입니다. 이 DLL에 들어 있는 파일은 모두 명령줄에 정의된 _EXPORTS 기호로
// 컴파일되며, 동일한 DLL을 사용하는 다른 프로젝트에서는 이 기호를 정의할 수 없습니다.
// 이렇게 하면 소스 파일에 이 파일이 들어 있는 다른 모든 프로젝트에서는 
// DLLUGPOST_API 함수를 DLL에서 가져오는 것으로 보고, 이 DLL은
// 이 DLL은 해당 매크로로 정의된 기호가 내보내지는 것으로 봅니다.
#ifdef CATIAPRE_DLL_EXPORTS
#define DLLCATIAPRE_API extern "C" __declspec(dllexport)
#else
#define DLLCATIAPRE_API extern "C" __declspec(dllimport)
#endif

#import "C:\\Program Files (x86)\\MACRO\\TransCAD\\TransCAD.exe"

#include <stdio.h>
#include <tchar.h>

#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <sstream>
#include <comutil.h>
#include <comdef.h>

using namespace std;
#include "Definition.h"
namespace Pre {
	extern TransCAD::IApplicationPtr g_spApplication;
	// 이 클래스는 dllUGPost.dll에서 내보낸 것입니다.
	DLLCATIAPRE_API int CATIAPreStartService(void);
	DLLCATIAPRE_API int CATIAPreTranslate(char * path);
	DLLCATIAPRE_API int CATIAPreTranslateAssem(int totPartNum, int partDocNum[], double partCoords[][12]);
}