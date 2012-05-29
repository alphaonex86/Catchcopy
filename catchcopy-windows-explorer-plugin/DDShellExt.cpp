// DDShellExt.cpp : Implementation of CDDShellExt

#include "stdafx.h"
#include "DDShellExt.h"
#include "ClientCatchcopy.h"

#ifndef _M_X64
const CLSID CLSID_DDShellExt = {0x68D44A27,0xFFB6,0x4B89,{0xA3,0xE5,0x7B,0x0E,0x50,0xA7,0xAB,0x33}};
#else
const CLSID CLSID_DDShellExt = {0x68ff37c4,0x51bc,0x4c2a,{0xa9,0x92,0x7e,0x39,0xbc,0xe,0x70,0x6f}};
#endif

STDMETHODIMP CDDShellExt::Initialize(LPCITEMIDLIST pidlFolder,LPDATAOBJECT pDO,HKEY hProgID)
{
	if(!connected)
	{
		bool b = m_ac.connectToServer();

		if (b==true)
		{
			connected=true;
			bool f = m_ac.sendProtocol();
			if(f!=true)
				return E_FAIL;
			#if defined(_M_X64)
				m_ac.setClientName(L"Windows Explorer 64Bits");
			#else
				m_ac.setClientName(L"Windows Explorer 32Bits");
			#endif
		}
		else
			return E_FAIL;
	}

	FORMATETC fmt={CF_HDROP,NULL,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};
	STGMEDIUM stg={TYMED_HGLOBAL};
	HDROP hDrop;

	fDestDir[0]=0;
	if (!SHGetPathFromIDList(pidlFolder,fDestDir))
	{
		#ifdef CATCHCOPY_EXPLORER_PLUGIN_DEBUG
		MessageBox(NULL,L"Initialize",L"E_FAIL 1",MB_OK);
		#endif // CATCHCOPY_EXPLORER_PLUGIN_DEBUG
		return E_FAIL;
	}

	// Detect if it's explorer that started the operation by enumerating available
	// clipboard formats and searching for one that only explorer uses
	IEnumFORMATETC *en;
	FORMATETC fmt2;
	WCHAR fmtName[256]=L"\0";
	fFromExplorer=false;
	pDO->EnumFormatEtc(DATADIR_GET,&en);
	while(en->Next(1,&fmt2,NULL)==S_OK){
		GetClipboardFormatName(fmt2.cfFormat,fmtName,256);
		if (!wcscmp(fmtName,CFSTR_SHELLIDLIST)) fFromExplorer=true;
	}
	en->Release();

	// Look for CF_HDROP data in the data object. If there
	// is no such data, return an error back to Explorer.
	if (FAILED(pDO->GetData(&fmt,&stg)))
	{
		#ifdef CATCHCOPY_EXPLORER_PLUGIN_DEBUG
		MessageBox(NULL,L"Initialize",L"E_INVALIDARG 2",MB_OK);
		#endif // CATCHCOPY_EXPLORER_PLUGIN_DEBUG
		return E_INVALIDARG;
	}

	// Get a pointer to the actual data.
	hDrop=(HDROP)GlobalLock(stg.hGlobal);

	// Make sure it worked.
	if (hDrop==NULL)
	{
		#ifdef CATCHCOPY_EXPLORER_PLUGIN_DEBUG
		MessageBox(NULL,L"Initialize",L"E_INVALIDARG 1",MB_OK);
		#endif // CATCHCOPY_EXPLORER_PLUGIN_DEBUG
		return E_INVALIDARG;
	}

	UINT numFiles,i;
	WCHAR fn[MAX_PATH]=L"";

	numFiles=DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);

	if (numFiles)
	{
		for(i=0;i<numFiles;++i)
		{
			if(DragQueryFile(hDrop,i,fn,MAX_PATH))
				sources.push_back(fn);
		}
	}

	GlobalUnlock(stg.hGlobal);
	ReleaseStgMedium(&stg);

	return S_OK;
}

STDMETHODIMP CDDShellExt::QueryContextMenu(HMENU hmenu,UINT uMenuIndex,UINT uidFirstCmd,UINT uidLastCmd,UINT uFlags)
{
	if(!m_ac.isConnected())
	{
		if(!m_ac.connectToServer())
			return E_FAIL;
	}
	if (uFlags&CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,0);

	int x=uidFirstCmd;

	InsertMenu(hmenu,uMenuIndex++,MF_STRING|MF_BYPOSITION,x++,_T("Copy"));
	InsertMenu(hmenu,uMenuIndex++,MF_STRING|MF_BYPOSITION,x++,_T("Move"));

	int defItem=GetMenuDefaultItem(hmenu,false,0);
	if (defItem==1) // 1: Copy
	{
		if (fFromExplorer) 
			SetMenuDefaultItem(hmenu,uidFirstCmd+defItem-1,false);
	}
	else if (defItem==2)
	{
		SetMenuDefaultItem(hmenu,uidFirstCmd+defItem-1,false);
	}
	return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,2);
}


STDMETHODIMP CDDShellExt::InvokeCommand ( LPCMINVOKECOMMANDINFO pInfo )
{
	if(HIWORD(pInfo->lpVerb))
		return E_INVALIDARG;
	switch(LOWORD(pInfo->lpVerb))
	{
		case 0:// copy
			if(!m_ac.addCopyWithDestination(sources,fDestDir))
				return E_FAIL;
		break;
		case 1:// move
			if(!m_ac.addMoveWithDestination(sources,fDestDir))
				return E_FAIL;
		break;
		default :
			return S_OK;
	}
	return S_OK;
}
