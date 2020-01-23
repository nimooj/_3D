#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE PEditorDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// lpReserved를 사용하는 경우 다음을 제거하십시오.
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("PEditor.DLL을 초기화하고 있습니다.\n");
		
		if (!AfxInitExtensionModule(PEditorDLL, hInstance))
			return 0;

		new CDynLinkLibrary(PEditorDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("PEditor.DLL을 종료하고 있습니다.\n");

		AfxTermExtensionModule(PEditorDLL);
	}
	return 1;   // 확인
}
