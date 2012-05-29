// Implementation of DLL Exports.
#include "stdafx.h"
#include "resource.h"
#include "DDShellExt.h"

#pragma comment(lib,"wsock32")

CComModule _AtlModule;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_DDShellExt, CDDShellExt)
END_OBJECT_MAP()

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;

	switch(dwReason){
		case DLL_PROCESS_ATTACH:
			//MessageBox ( NULL,L"DLL_PROCESS_ATTACH", L"DLL_PROCESS_ATTACH", MB_OK);
		break;
		case DLL_PROCESS_DETACH:
			//MessageBox ( NULL,L"DLL_PROCESS_DETACH", L"DLL_PROCESS_DETACH", MB_OK);
	  break;
	}

	return _AtlModule.DllMain(hInstance, dwReason, lpReserved,ObjectMap,NULL); 
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{	
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer(FALSE);
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	m_ac.disconnectFromServer();
	HRESULT hr = _AtlModule.DllUnregisterServer(FALSE);
	return hr;
}
