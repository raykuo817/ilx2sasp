
#include "stdafx.h"
#include "util.h"

//////////////////////////////////////////
BOOL IsDir(CString dir)
{
	DWORD attr;
	attr = GetFileAttributes(dir);
    if ((attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        return true;
    }
    return false;
}

//////////////////////////////////////////
BOOL IsFile(CString file)
{
	DWORD attr;
	attr = GetFileAttributes(file);
    if ((attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_ARCHIVE)) {
        return true;
    }
    return false;
}

//////////////////////////////////////////
CString GetMyMusic()
{
    TCHAR buffer[MAX_PATH] = {'\0'};
    SHGetFolderPathW(0, CSIDL_MYMUSIC , 0, 0, buffer);
    return CString(buffer);
}

//////////////////////////////////////////
CString GetLaunchDir()
{
	DWORD dwReturn = 0;
    TCHAR buffer[MAX_PATH] = {'\0'};

	dwReturn = ::GetModuleFileName(NULL, buffer, MAX_PATH);

	if (dwReturn <= MAX_PATH)
	{
		// Remove filename from path
		LPTSTR tszSlash = _tcsrchr(buffer, _T('\\'));
		if (tszSlash) *tszSlash = _T('\0');
	}

	return CString(buffer);
}

//////////////////////////////////////////
void ExtractFilePath(TCHAR *pszFullPath, TCHAR *pszPathName)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];

	_tsplitpath_s(pszFullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);
	_tmakepath_s(pszPathName, _MAX_PATH, drive, dir, NULL, NULL);

}

//////////////////////////////////////////
void ShowErrorMessage(LPTSTR msg)
{
	TCHAR   msgBuf[1000];
	LPTSTR lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
		);

	SetConsoleOutputCP(GetConsoleCP());
	//_tprintf( _T("Error: (%d) %s"), dwError, lpMsgBuf);


	_tcscpy_s(msgBuf, 1000, msg);
	_tcscat_s(msgBuf, 1000, lpMsgBuf);

	MessageBox(0, msgBuf, _T("ERROR"), MB_OK);

	LocalFree(lpMsgBuf);
}

//////////////////////////////////////////
void TraceErrorMessage(LPTSTR msg)
{
	TCHAR   msgBuf[1000];
	LPTSTR lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
		);

	SetConsoleOutputCP(GetConsoleCP());
	//_tprintf( _T("Error: (%d) %s"), dwError, lpMsgBuf);
//  MessageBox(0, lpMsgBuf, _T("ERROR"), MB_OK);

	_tcscpy_s(msgBuf, 1000, msg);
	_tcscat_s(msgBuf, 1000, lpMsgBuf);

	TRACE(msgBuf);

	LocalFree(lpMsgBuf);
}


//////////////////////////////////////////
void GetOsVersion(OsTypeStruct *os)
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;

	os->OsType = WINDOW_TYPE_UNKNOWN;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx((OSVERSIONINFO *)&osvi))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *)&osvi)) return;
	}

	os->OsPlatform = osvi.dwPlatformId;
	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		os->OsType = WINDOW_TYPE_WIN32S;
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion == 0)) os->OsType = WINDOW_TYPE_WIN95;
		else if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion == 10)) os->OsType = WINDOW_TYPE_WIN98;
		else if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion == 90)) os->OsType = WINDOW_TYPE_WINME;
		break;
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion <= 3) os->OsType = WINDOW_TYPE_WINNT3;
		else if (osvi.dwMajorVersion <= 4) os->OsType = WINDOW_TYPE_WINNT4;
		else if (osvi.dwMajorVersion == 5)
		{
			if (osvi.dwMinorVersion == 0) os->OsType = WINDOW_TYPE_WIN2000;
			else if (osvi.dwMinorVersion == 1) os->OsType = WINDOW_TYPE_WINXP32;
			else if (osvi.dwMinorVersion == 2)
			{
				// Use GetProcAddress to avoid load issues on Windows 2000
				pGNSI = (PGNSI)::GetProcAddress(::GetModuleHandle(_T("kernel32.dll")),
												"GetNativeSystemInfo");
				if (NULL != pGNSI) pGNSI(&si);

				if (osvi.wProductType == VER_NT_WORKSTATION &&
					(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
					 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64 ||
					 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
					 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64))
				{
					os->OsType = WINDOW_TYPE_WINXP64;
				} else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
						   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64 ||
						   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
						   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64)
				{
					os->OsType = WINDOW_TYPE_2003SRVR64;
				} else
				{
					os->OsType = WINDOW_TYPE_2003SRVR32;
				}
			}
		} else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
		{
			// Use GetProcAddress to avoid load issues on Windows 2000
			pGNSI = (PGNSI)::GetProcAddress(::GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
			if (NULL != pGNSI) pGNSI(&si);

			if (osvi.wProductType == VER_NT_WORKSTATION &&
				(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
				 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64 ||
				 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
				 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64))
			{
				os->OsType = WINDOW_TYPE_VISTA64;
			} else if (osvi.wProductType == VER_NT_WORKSTATION)
			{
				os->OsType = WINDOW_TYPE_VISTA32;
			} else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
					   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64 ||
					   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
					   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64)
			{
				//os->OsType = WINDOW_TYPE_LONGHORN64;
				os->OsType = WINDOW_TYPE_2008SRVR64;
			} else
			{
				//os->OsType = WINDOW_TYPE_LONGHORN32;
				os->OsType = WINDOW_TYPE_2008SRVR32;
			}
		} else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
		{
			pGNSI = (PGNSI)::GetProcAddress(::GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
			if (NULL != pGNSI) pGNSI(&si);

			if (osvi.wProductType == VER_NT_WORKSTATION &&
				(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
				 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64 ||
				 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
				 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64))
			{
				os->OsType = WINDOW_TYPE_WIN7_64;
			} else if (osvi.wProductType == VER_NT_WORKSTATION)
			{
				os->OsType = WINDOW_TYPE_WIN7_32;
			} else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
					   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64 ||
					   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
					   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64)
			{
				os->OsType = WINDOW_TYPE_2008SRVR64_R2;
			} else
			{
				os->OsType = WINDOW_TYPE_2008SRVR32_R2;
			}
		} else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
		{
			pGNSI = (PGNSI)::GetProcAddress(::GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
			if (NULL != pGNSI) pGNSI(&si);

			if (osvi.wProductType == VER_NT_WORKSTATION &&
				(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
				 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64 ||
				 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
				 si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64))
			{
				os->OsType = WINDOW_TYPE_WIN8_64;
			} else if (osvi.wProductType == VER_NT_WORKSTATION)
			{
				os->OsType = WINDOW_TYPE_WIN8_32;
			} else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
					   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64 ||
					   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
					   si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64)
			{
				os->OsType = WINDOW_TYPE_WIN8SRVR64;
			} else
			{
				os->OsType = WINDOW_TYPE_WIN8SRVR32;
			}
		}
		break;
	}
}

//////////////////////////////////////////
void ProcessMessages()
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::TranslateMessage(&msg);
	::DispatchMessage(&msg);
}

//////////////////////////////////////////
HBITMAP CopyBitmap(HBITMAP hSourceHbitmap)
{
	CDC sourceDC;
	CDC destDC;
	sourceDC.CreateCompatibleDC(NULL);
	destDC.CreateCompatibleDC(NULL);
	//The bitmap information.
	BITMAP bm = { 0 };
	//Get the bitmap information.
	::GetObject(hSourceHbitmap, sizeof(bm), &bm);
	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), bm.bmWidth, bm.bmHeight);

	HBITMAP hbmOldSource = (HBITMAP)::SelectObject(sourceDC.m_hDC, hSourceHbitmap);
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject(destDC.m_hDC, hbmResult);
	destDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCCOPY);

	// Restore DCs
	::SelectObject(sourceDC.m_hDC, hbmOldSource);
	::SelectObject(destDC.m_hDC, hbmOldDest);
	::DeleteObject(sourceDC.m_hDC);
	::DeleteObject(destDC.m_hDC);

	return hbmResult;
}

//////////////////////////////////////////
static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	// If the BFFM_INITIALIZED message is received
	// set the path to the start path.
	switch (uMsg)
	{
		case BFFM_INITIALIZED:
		{
			if (NULL != lpData)
			{
				::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
			}
		}
	}

	return 1;
}

// HWND is the parent window.
// szCurrent is an optional start folder. Can be NULL.
// szPath receives the selected path on success. Must be MAX_PATH characters in length.
BOOL BrowseForFolder(HWND hwnd, LPCTSTR szTitle, LPCTSTR szCurrent, LPTSTR szPath)
{
	BROWSEINFO   bi = { 0 };
	LPITEMIDLIST pidl;
	TCHAR        szDisplay[MAX_PATH];
	BOOL         retval;

	bi.hwndOwner      = hwnd;
	bi.pszDisplayName = szDisplay;
    bi.lpszTitle      = szTitle;
    bi.ulFlags        = BIF_RETURNONLYFSDIRS /*| BIF_NEWDIALOGSTYLE*/;
    bi.lpfn           = BrowseCallbackProc;
    bi.lParam         = (LPARAM) szCurrent;

	pidl = SHBrowseForFolder(&bi);

	if (NULL != pidl)
	{
		retval = SHGetPathFromIDList(pidl, szPath);
		CoTaskMemFree(pidl);
	}
	else
	{
		retval = FALSE;
	}

	if (!retval)
	{
		szPath[0] = TEXT('\0');
	}

	return retval;
}
//////////////////////////////////////////

