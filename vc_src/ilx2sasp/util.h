

#ifndef _UTIL_H_
#define _UTIL_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif	_WINDOWS_

//////////////////////////////////////////
//  Operation System Definition
#define WINDOW_TYPE_UNKNOWN			0x00000000
#define WINDOW_TYPE_WIN32S			0x00000001
#define WINDOW_TYPE_WIN95			0x00000002
#define WINDOW_TYPE_WIN98			0x00000003
#define WINDOW_TYPE_WINME			0x00000004
#define WINDOW_TYPE_WINNT3			0x00000005
#define WINDOW_TYPE_WINNT4			0x00000006
#define WINDOW_TYPE_WIN2000			0x00000007
#define WINDOW_TYPE_WINXP32			0x00000008
#define WINDOW_TYPE_WINXP64			0x10000008
#define WINDOW_TYPE_2003SRVR32		0x00000009
#define WINDOW_TYPE_2003SRVR64		0x10000009
#define WINDOW_TYPE_VISTA32			0x0000000A
#define WINDOW_TYPE_VISTA64			0x1000000A
//#define WINDOW_TYPE_LONGHORN32		0x0000000B
//#define WINDOW_TYPE_LONGHORN64		0x1000000B
#define WINDOW_TYPE_2008SRVR32		0x0000000B
#define WINDOW_TYPE_2008SRVR64		0x1000000B
#define WINDOW_TYPE_WIN7_32			0x0000000C
#define WINDOW_TYPE_WIN7_64			0x1000000C
#define WINDOW_TYPE_2008SRVR32_R2	0x0000000D
#define WINDOW_TYPE_2008SRVR64_R2	0x1000000D
#define WINDOW_TYPE_WIN8_32			0x0000000E
#define WINDOW_TYPE_WIN8_64			0x1000000E
#define WINDOW_TYPE_WIN8SRVR32		0x0000000F
#define WINDOW_TYPE_WIN8SRVR64		0x1000000F

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

typedef struct
{
    	DWORD	OsPlatform;
        DWORD	OsType;
} OsTypeStruct;

void GetOsVersion(OsTypeStruct* os);

//////////////////////////////////////////
//DWORD GetLaunchDir(DWORD nBufferLength, LPTSTR lpBuffer);
CString GetLaunchDir();
void ExtractFilePath(TCHAR* pszFullPath,TCHAR* pszPathName);
void ShowErrorMessage(LPTSTR msg);
void TraceErrorMessage(LPTSTR msg);
void ProcessMessages();
HBITMAP CopyBitmap(HBITMAP hSourceHbitmap);
static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData);
BOOL BrowseForFolder(HWND hwnd, LPCTSTR szTitle, LPCTSTR szCurrent, LPTSTR szPath);
//BOOL GetMyMusic(DWORD nBufferLength, LPTSTR lpBuffer);
CString GetMyMusic();
BOOL IsDir(CString dir);
BOOL IsFile(CString file);


#endif
