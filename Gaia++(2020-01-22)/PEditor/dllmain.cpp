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
	// lpReserved�� ����ϴ� ��� ������ �����Ͻʽÿ�.
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("PEditor.DLL�� �ʱ�ȭ�ϰ� �ֽ��ϴ�.\n");
		
		if (!AfxInitExtensionModule(PEditorDLL, hInstance))
			return 0;

		new CDynLinkLibrary(PEditorDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("PEditor.DLL�� �����ϰ� �ֽ��ϴ�.\n");

		AfxTermExtensionModule(PEditorDLL);
	}
	return 1;   // Ȯ��
}
