#include "Reg.h"
#include <strsafe.h>

//
//   FUNCTION: SetHKCRRegistryKeyAndValue
//
//   PURPOSE: The function creates a HKCR registry key and sets the specified
//   registry value.
//
//   PARAMETERS:
//   * pszSubKey - specifies the registry key under HKCR. If the key does not
//     exist, the function will create the registry key.
//   * pszValueName - specifies the registry value to be set. If pszValueName
//     is NULL, the function will set the default value.
//   * pszData - specifies the string data of the registry value.
//
//   RETURN VALUE:
//   If the function succeeds, it returns S_OK. Otherwise, it returns an
//   HRESULT error code.
//
HRESULT SetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName,
    PCWSTR pszData)
{
    HRESULT hr;
    HKEY hKey = NULL;

    // Creates the specified registry key. If the key already exists, the
    // function opens it.
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(
                            #ifdef CATCHCOPY_ROOT_MODE
                                HKEY_CLASSES_ROOT
                            #else
                                HKEY_CURRENT_USER
                            #endif
                                , pszSubKey, 0,
        NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL, &hKey, NULL));
    
    if (SUCCEEDED(hr))
    {
        if (pszData != NULL)
        {
            // Set the specified value of the key.
            DWORD cbData = lstrlen(pszData) * sizeof(*pszData);
            hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, pszValueName, 0,
                REG_SZ, reinterpret_cast<const BYTE *>(pszData), cbData));
        }

        RegCloseKey(hKey);
    }

    return hr;
}

LONG RecursiveDeleteKey(HKEY hKeyParent,  PCWSTR szKeyChild)    
{    
    // Open the child.    
    HKEY hKeyChild ;    
    LONG lRes = RegOpenKeyEx(hKeyParent, szKeyChild, 0,    
                             KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKeyChild) ;    
    if (lRes != ERROR_SUCCESS)    
    {    
        return lRes ;    
    }    
    // Enumerate all of the decendents of this child.    
    FILETIME time ;    
	wchar_t szBuffer[MAX_PATH];
    DWORD dwSize = MAX_PATH ;
    while (RegEnumKeyEx(hKeyChild, 0, (LPWSTR)szBuffer, &dwSize, NULL,    
                        NULL, NULL, &time) == ERROR_SUCCESS)    
    {    
        // Delete the decendents of this child.    
        lRes = RecursiveDeleteKey(hKeyChild, (PCWSTR)szBuffer) ;   
	    if (lRes != ERROR_SUCCESS)    
        {    
            // Cleanup before exiting.    
            RegCloseKey(hKeyChild) ;    
            return lRes;    
        }    
        dwSize = MAX_PATH ;    
    }    
    
    // Close the child.    
    RegCloseKey(hKeyChild) ;
    
    // Delete this child.    
    return RegDeleteKey(hKeyParent, szKeyChild) ;    
}

//
//   FUNCTION: GetHKCRRegistryKeyAndValue
//
//   PURPOSE: The function opens a HKCR registry key and gets the data for the
//   specified registry value name.
//
//   PARAMETERS:
//   * pszSubKey - specifies the registry key under HKCR. If the key does not
//     exist, the function returns an error.
//   * pszValueName - specifies the registry value to be retrieved. If
//     pszValueName is NULL, the function will get the default value.
//   * pszData - a pointer to a buffer that receives the value's string data.
//   * cbData - specifies the size of the buffer in bytes.
//
//   RETURN VALUE:
//   If the function succeeds, it returns S_OK. Otherwise, it returns an
//   HRESULT error code. For example, if the specified registry key does not
//   exist or the data for the specified value name was not set, the function
//   returns COR_E_FILENOTFOUND (0x80070002).
//
HRESULT GetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName,
    PWSTR pszData, DWORD cbData)
{
    HRESULT hr;
    HKEY hKey = NULL;

    // Try to open the specified registry key.
    hr = HRESULT_FROM_WIN32(RegOpenKeyEx(
                            #ifdef CATCHCOPY_ROOT_MODE
                                HKEY_CLASSES_ROOT
                            #else
                                HKEY_CURRENT_USER
                            #endif
                                , pszSubKey, 0,
        KEY_READ, &hKey));

    if (SUCCEEDED(hr))
    {
        // Get the data for the specified value name.
        hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, pszValueName, NULL,
            NULL, reinterpret_cast<LPBYTE>(pszData), &cbData));

        RegCloseKey(hKey);
    }

    return hr;
}

//
//   FUNCTION: RegisterInprocServer
//
//   PURPOSE: Register the in-process component in the registry.
//
//   PARAMETERS:
//   * pszModule - Path of the module that contains the component
//   * clsid - Class ID of the component
//   * pszFriendlyName - Friendly name
//   * pszThreadModel - Threading model
//
//   NOTE: The function creates the HKCR\CLSID\{<CLSID>} key in the registry.
//
//   HKCR
//   {
//      NoRemove CLSID
//      {
//          ForceRemove {<CLSID>} = s '<Friendly Name>'
//          {
//              InprocServer32 = s '%MODULE%'
//              {
//                  val ThreadingModel = s '<Thread Model>'
//              }
//          }
//      }
//   }
//
HRESULT RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, PCWSTR pszFriendlyName, PCWSTR pszThreadModel)
{
    if (pszModule == NULL || pszThreadModel == NULL)
    {
        return E_INVALIDARG;
    }

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // Create the HKCR\CLSID\{<CLSID>} key.
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey),
                     #ifdef CATCHCOPY_ROOT_MODE
                         L"CLSID\\%s"
                     #else
                         L"Software\\Classes\\CLSID\\%s"
                     #endif
                         , szCLSID);
    if (SUCCEEDED(hr))
    {
        hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszFriendlyName);

        // Create the HKCR\CLSID\{<CLSID>}\InprocServer32 key.
        if (SUCCEEDED(hr))
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey),
                     #ifdef CATCHCOPY_ROOT_MODE
                         L"CLSID\\%s\\InprocServer32"
                     #else
                         L"Software\\Classes\\CLSID\\%s\\InprocServer32"
                     #endif
                                 , szCLSID);
            if (SUCCEEDED(hr))
            {
                // Set the default value of the InprocServer32 key to the
                // path of the COM module.
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszModule);
                if (SUCCEEDED(hr))
                {
                    // Set the threading model of the component.
                    hr = SetHKCRRegistryKeyAndValue(szSubkey, L"ThreadingModel", pszThreadModel);
                }
            }
        }
    }

    return hr;
}


//
//   FUNCTION: UnregisterInprocServer
//
//   PURPOSE: Unegister the in-process component in the registry.
//
//   PARAMETERS:
//   * clsid - Class ID of the component
//
//   NOTE: The function deletes the HKCR\CLSID\{<CLSID>} key in the registry.
//
HRESULT UnregisterInprocServer(const CLSID& clsid)
{
    HRESULT hr = S_OK;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // Delete the HKCR\CLSID\{<CLSID>} key.
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey),
                     #ifdef CATCHCOPY_ROOT_MODE
                         L"CLSID\\%s"
                     #else
                         L"Software\\Classes\\CLSID\\%s"
                     #endif
                         , szCLSID);

	if (SUCCEEDED(hr))
    {
        hr = HRESULT_FROM_WIN32(RecursiveDeleteKey(
                            #ifdef CATCHCOPY_ROOT_MODE
                                HKEY_CLASSES_ROOT
                            #else
                                HKEY_CURRENT_USER
                            #endif
                                    , szSubkey));
    }

    return hr;
}
//
//   FUNCTION: RegisterShellExtContextMenuHandler
//
//   PURPOSE: Register the context menu handler.
//
//   PARAMETERS:
//   * pszFileType - The file type that the context menu handler is
//     associated with. For example, '*' means all file types; '.txt' means
//     all .txt files. The parameter must not be NULL.
//   * clsid - Class ID of the component
//   * pszFriendlyName - Friendly name
//
//   NOTE: The function creates the following key in the registry.
//
//   HKCR
//   {
//      NoRemove <File Type>
//      {
//          NoRemove shellex
//          {
//              NoRemove ContextMenuHandlers
//              {
//                  {<CLSID>} = s '<Friendly Name>'
//              }
//          }
//      }
//   }
//
HRESULT RegisterShellExtContextMenuHandler(
    PCWSTR pszFileType, const CLSID& clsid, PCWSTR pszFriendlyName)
{
    if (pszFileType == NULL)
    {
        return E_INVALIDARG;
    }

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // If pszFileType starts with '.', try to read the default value of the
    // HKCR\<File Type> key which contains the ProgID to which the file type
    // is linked.
    if (*pszFileType == L'.')
    {
        wchar_t szDefaultVal[260];
        hr = GetHKCRRegistryKeyAndValue(pszFileType, NULL, szDefaultVal,
            sizeof(szDefaultVal));

        // If the key exists and its default value is not empty, use the
        // ProgID as the file type.
        if (SUCCEEDED(hr) && szDefaultVal[0] != L'\0')
        {
            pszFileType = szDefaultVal;
        }
    }

    // Create the key HKCR\<File Type>\shellex\DragDropHandlers\{<CLSID>}
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey),
                     #ifdef CATCHCOPY_ROOT_MODE
                         L"%s\\shellex\\DragDropHandlers\\%s"
                     #else
                         L"Software\\Classes\\%s\\shellex\\DragDropHandlers\\%s"
                     #endif
                         , pszFileType, szCLSID);
    if (SUCCEEDED(hr))
    {
        // Set the default value of the key.
        hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszFriendlyName);
    }

    return hr;
}


//
//   FUNCTION: UnregisterShellExtContextMenuHandler
//
//   PURPOSE: Unregister the context menu handler.
//
//   PARAMETERS:
//   * pszFileType - The file type that the context menu handler is
//     associated with. For example, '*' means all file types; '.txt' means
//     all .txt files. The parameter must not be NULL.
//   * clsid - Class ID of the component
//
//   NOTE: The function removes the {<CLSID>} key under
//   HKCR\<File Type>\shellex\ContextMenuHandlers in the registry.
//
HRESULT UnregisterShellExtContextMenuHandler(
    PCWSTR pszFileType, const CLSID& clsid)
{
    if (pszFileType == NULL)
    {
        return E_INVALIDARG;
    }

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // If pszFileType starts with '.', try to read the default value of the
    // HKCR\<File Type> key which contains the ProgID to which the file type
    // is linked.
    if (*pszFileType == L'.')
    {
        wchar_t szDefaultVal[260];
        hr = GetHKCRRegistryKeyAndValue(pszFileType, NULL, szDefaultVal,
            sizeof(szDefaultVal));

        // If the key exists and its default value is not empty, use the
        // ProgID as the file type.
        if (SUCCEEDED(hr) && szDefaultVal[0] != L'\0')
        {
            pszFileType = szDefaultVal;
        }
    }

    // Remove the HKCR\<File Type>\shellex\DragDropHandlers\{<CLSID>} key.
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey),
                     #ifdef CATCHCOPY_ROOT_MODE
                         L"%s\\shellex\\DragDropHandlers\\%s"
                     #else
                         L"Software\\Classes\\%s\\shellex\\DragDropHandlers\\%s"
                     #endif
                         , pszFileType, szCLSID);
    if (SUCCEEDED(hr))
    {
        hr = HRESULT_FROM_WIN32(RecursiveDeleteKey(
                            #ifdef CATCHCOPY_ROOT_MODE
                                HKEY_CLASSES_ROOT
                            #else
                                HKEY_CURRENT_USER
                            #endif
                                    , szSubkey));
    }

    return hr;
}
