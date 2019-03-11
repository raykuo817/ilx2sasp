
// ilx2saspDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ilx2sasp.h"
#include "ilx2saspDlg.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL g_bThreadRun = TRUE;
UINT MonitorFile(LPVOID pVar);

// Cilx2saspDlg dialog

Cilx2saspDlg::Cilx2saspDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cilx2saspDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cilx2saspDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cilx2saspDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEL_PHP_DIR, &Cilx2saspDlg::OnBnClickedButtonSelPhpDir)
	ON_BN_CLICKED(IDC_BUTTON_SEL_PLAYLIST_DIR, &Cilx2saspDlg::OnBnClickedButtonSelPlaylistDir)
	ON_BN_CLICKED(IDC_BUTTON_SEL_XML_FILE, &Cilx2saspDlg::OnBnClickedButtonSelXmlFile)
	ON_BN_CLICKED(IDC_CHECK_AUTORUN, &Cilx2saspDlg::OnBnClickedCheckAutorun)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &Cilx2saspDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &Cilx2saspDlg::OnBnClickedButtonDel)
	ON_LBN_SELCHANGE(IDC_LIST_IGNORE, &Cilx2saspDlg::OnLbnSelchangeListIgnore)
    ON_WM_TIMER()
	ON_WM_CLOSE()
    ON_MESSAGE(WM_SHOWTASK, OnShowTask)
	ON_WM_DESTROY()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// Cilx2saspDlg message handlers

BOOL Cilx2saspDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
//  m_bClose = FALSE;

    //
    m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
    m_nid.hWnd = this->m_hWnd;
    m_nid.uID = IDR_MAINFRAME;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_SHOWTASK;
    m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
    _tcscpy_s(m_nid.szTip, L"iTunes Library XML 2 Synology Audio Station Playlists");
    Shell_NotifyIcon(NIM_ADD, &m_nid);

    BOOL firstRun = FALSE;
    if (!IsFile(GetLaunchDir()+_T("\\ilx2sasp.ini"))) {
        firstRun = true;
    }
    if (firstRun == FALSE) {
        ShowHideWindow(SW_HIDE);
        PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
        SetTimer(5550, 1, 0);
    }

    //Init INI
    m_IniReader = new CIniReader;
    m_IniReader->setINIFileName(GetLaunchDir()+_T("\\ilx2sasp.ini"));
    LoadIni();
    SaveIni();
    RunPHP();

    m_thread = AfxBeginThread(MonitorFile, (LPVOID)this);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cilx2saspDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cilx2saspDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cilx2saspDlg::OnBnClickedButtonSelPhpDir()
{
    TCHAR curDir[MAX_PATH];
    TCHAR szPath[MAX_PATH];
    HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();

    GetDlgItem(IDC_EDIT_PHP_DIR)->GetWindowText(curDir, MAX_PATH);

    if (BrowseForFolder(hWnd, TEXT("Choose PHP folder : "), curDir, szPath)) {
        GetDlgItem(IDC_EDIT_PHP_DIR)->SetWindowText(szPath);
        SaveIni();
    }
}


void Cilx2saspDlg::OnBnClickedButtonSelPlaylistDir()
{
    TCHAR curDir[MAX_PATH];
    TCHAR szPath[MAX_PATH];
    HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();

    GetDlgItem(IDC_EDIT_PLAYLISTS_DIR)->GetWindowText(curDir, MAX_PATH);

    if (BrowseForFolder(hWnd, TEXT("Choose Audio Station Playlists folder : "), curDir, szPath)) {
        GetDlgItem(IDC_EDIT_PLAYLISTS_DIR)->SetWindowText(szPath);
        SaveIni();
    }
}

void Cilx2saspDlg::OnBnClickedButtonSelXmlFile()
{
    TCHAR curXML[MAX_PATH];
    GetDlgItem(IDC_EDIT_XML_FILE)->GetWindowText(curXML, MAX_PATH);

    CFileDialog dlg(TRUE, _T("xml"), curXML, OFN_OVERWRITEPROMPT, _T("XML Files (*.xml)|*.xml||") );
    int iRet = dlg.DoModal();
    if (iRet == IDOK) {
        CString file = dlg.GetPathName();
        GetDlgItem(IDC_EDIT_XML_FILE)->SetWindowText(file);
        SaveIni();
    }
}

void Cilx2saspDlg::OnBnClickedCheckAutorun()
{
    SaveIni();
}

void Cilx2saspDlg::RegAutoRun(bool cmd)
{
	if (cmd == 1)
	{
        CString str;
        str.Format(L"/Create /F /TN \"ilx2sasp\" /SC ONLOGON /RL HIGHEST /TR \"\\\"%s\\ilx2sasp.exe\\\"\"", GetLaunchDir());
        ShellExecute(NULL, _T("open"), _T("schtasks.exe"), str, _T(""), 0);
	}
	else
	{
		//remove
        ShellExecute(NULL, _T("open"), _T("schtasks.exe"), _T("/Delete /F /TN \"ilx2sasp\""), _T(""), 0);
	}
}


void Cilx2saspDlg::LoadIni()
{
    CString iTunesXML1, iTunesXML2;
    CString playlistDir;
    CString phpDir;
    CString phpExe;
    CString autorun;

    if (m_IniReader->sectionExists(_T("CONFIG"))) {
        iTunesXML1 = m_IniReader->getKeyValue(_T("ITUNES_LIBRARY_XML"), _T("CONFIG"));
        playlistDir = m_IniReader->getKeyValue(_T("AUDIO_STATION_PLAYLISTS_DIR"), _T("CONFIG"));
        phpDir = m_IniReader->getKeyValue(_T("PHP_DIR"), _T("CONFIG"));
        autorun = m_IniReader->getKeyValue(_T("AUTORUN"), _T("CONFIG"));

        GetDlgItem(IDC_EDIT_XML_FILE)->SetWindowText(iTunesXML1);
        GetDlgItem(IDC_EDIT_PLAYLISTS_DIR)->SetWindowText(playlistDir);
        GetDlgItem(IDC_EDIT_PHP_DIR)->SetWindowText(phpDir);

        CButton* pAutorun = (CButton*)GetDlgItem(IDC_CHECK_AUTORUN);
        if (autorun == "0") pAutorun->SetCheck(0);
        else pAutorun->SetCheck(1);

        for (int i = 0; i < 1000; i++) {
            CString key;
            key.Format(_T("%d"), i);

            if (!m_IniReader->keyExists(key, _T("IGNORE"))) {
                break;
            }

            CString str = m_IniReader->getKeyValue(key, _T("IGNORE"));
            CListBox *m_list = (CListBox *)GetDlgItem(IDC_LIST_IGNORE);
            m_list->AddString(str);
        }
    }
    else {
        iTunesXML1 = GetMyMusic() + _T("\\iTunes\\iTunes Music Library.xml");
        iTunesXML2 = GetMyMusic() + _T("\\iTunes\\iTunes Library.xml");
        playlistDir = GetMyMusic() + _T("\\iTunes\\iTunes Media\\Music\\playlists");
        phpDir = GetLaunchDir() + _T("\\php");
        phpExe = phpDir + _T("\\php.exe");

        if (IsFile(iTunesXML1)) {
            GetDlgItem(IDC_EDIT_XML_FILE)->SetWindowText(iTunesXML1);
        }
        else if (IsFile(iTunesXML2)) {
                GetDlgItem(IDC_EDIT_XML_FILE)->SetWindowText(iTunesXML2);
        }

        if (IsDir(playlistDir)) {
            GetDlgItem(IDC_EDIT_PLAYLISTS_DIR)->SetWindowText(playlistDir);
        }

        if (IsDir(phpDir)) {
            if (IsFile(phpExe)) {
                GetDlgItem(IDC_EDIT_PHP_DIR)->SetWindowText(phpDir);
            }
        }

        CButton* pAutorun = (CButton*)GetDlgItem(IDC_CHECK_AUTORUN);
        pAutorun->SetCheck(1);
    }
}

void Cilx2saspDlg::SaveIni()
{
    TCHAR str[MAX_PATH];

    GetDlgItem(IDC_EDIT_XML_FILE)->GetWindowText(str, MAX_PATH);
    m_IniReader->setKey(str, _T("ITUNES_LIBRARY_XML"), _T("CONFIG"));

    GetDlgItem(IDC_EDIT_PLAYLISTS_DIR)->GetWindowText(str, MAX_PATH);
    m_IniReader->setKey(str, _T("AUDIO_STATION_PLAYLISTS_DIR"), _T("CONFIG"));

    GetDlgItem(IDC_EDIT_PHP_DIR)->GetWindowText(str, MAX_PATH);
    m_IniReader->setKey(str, _T("PHP_DIR"), _T("CONFIG"));


    CButton* pAutorun = (CButton*)GetDlgItem(IDC_CHECK_AUTORUN);
    if (pAutorun->GetCheck()) {
        m_IniReader->setKey(_T("1"), _T("AUTORUN"), _T("CONFIG"));
        RegAutoRun(1);
    }
    else {
        m_IniReader->setKey(_T("0"), _T("AUTORUN"), _T("CONFIG"));
        RegAutoRun(0);
    }

    m_IniReader->delSection(_T("IGNORE"));
    CListBox *m_list = (CListBox *)GetDlgItem(IDC_LIST_IGNORE);
    for (int i = 0; i < m_list->GetCount(); i++) {
        m_list->GetText(i, str);

        CString key;
        key.Format(_T("%d"),i);
        m_IniReader->setKey(str, key, _T("IGNORE"));
    }
}



void Cilx2saspDlg::OnBnClickedButtonAdd()
{
    TCHAR str[1024];
    TCHAR str2[1024];

    GetDlgItem(IDC_EDIT_IGNORE)->GetWindowText(str, 1024);
    if (str[0] != '\0') {
        CListBox *m_list = (CListBox *)GetDlgItem(IDC_LIST_IGNORE);
        BOOL bFound = false;
        for (int i = 0; i < m_list->GetCount(); i++) {
            m_list->GetText(i, str2);
            if (_tcscmp(str, str2) == 0) {
                bFound = true;
            }
        }
        if (bFound == FALSE) {
            m_list->AddString(str);
            GetDlgItem(IDC_EDIT_IGNORE)->SetWindowText(_T(""));
        }
    }
    SaveIni();
    RunPHP();
}

void Cilx2saspDlg::OnBnClickedButtonDel()
{
    TCHAR str[1024];
    TCHAR str2[1024];

    GetDlgItem(IDC_EDIT_IGNORE)->GetWindowText(str, 1024);
    CListBox *m_list = (CListBox *)GetDlgItem(IDC_LIST_IGNORE);
    BOOL bFound = false;
    for (int i = 0; i < m_list->GetCount(); i++) {
        m_list->GetText(i, str2);
        if (_tcscmp(str, str2) == 0) {
            m_list->DeleteString(i);
            GetDlgItem(IDC_EDIT_IGNORE)->SetWindowText(_T(""));
            break;
        }
    }
    SaveIni();
    RunPHP();
}

void Cilx2saspDlg::OnLbnSelchangeListIgnore()
{
    TCHAR str[1024];

    CListBox *m_list = (CListBox *)GetDlgItem(IDC_LIST_IGNORE);
    int sel = m_list->GetCurSel();
	if (sel >= 0) {
		m_list->GetText(sel, str);
	    GetDlgItem(IDC_EDIT_IGNORE)->SetWindowText(str);
	}
}

void Cilx2saspDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 5550) {
        ShowWindow(SW_HIDE);
        KillTimer(5550);
    }
    CDialog::OnTimer(nIDEvent);
}

void Cilx2saspDlg::OnClose()
{
//  // TODO: Add your message handler code here and/or call default
//  m_bClose = TRUE;
//
//  CDialog::OnClose();
    ShowHideWindow(SW_HIDE);
    PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void Cilx2saspDlg::RunPHP()
{
    TCHAR phpExe[MAX_PATH];

    GetDlgItem(IDC_EDIT_PHP_DIR)->GetWindowText(phpExe, MAX_PATH);
    _tcscat_s(phpExe, _T("\\php.exe"));

    ShellExecute(NULL, _T("open"), phpExe, _T("ilx2sasp.php"), GetLaunchDir(), 0);
}

void Cilx2saspDlg::ShowHideWindow(int cmd)
{
	static CRect appWnd;

	if (cmd == SW_SHOW)
	{
		::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, appWnd.left, appWnd.top, appWnd.Width(), appWnd.Height(), SWP_SHOWWINDOW);
		::SetWindowPos(GetSafeHwnd(), HWND_NOTOPMOST, appWnd.left, appWnd.top, appWnd.Width(), appWnd.Height(), SWP_SHOWWINDOW);
	}
	else if (cmd == SW_HIDE)
	{
		GetWindowRect(appWnd);

        if (appWnd.left == 0 && appWnd.top == 0) {
            int w = GetSystemMetrics(SM_CXSCREEN);
            int h = GetSystemMetrics( SM_CYSCREEN );
            appWnd.left = (w - appWnd.Width()) / 2;
            appWnd.top = (h - appWnd.Height()) / 2;
            appWnd.right += appWnd.left;
            appWnd.bottom += appWnd.top;
        }

		::SetWindowPos(GetSafeHwnd(),HWND_TOPMOST,0,0,0,0,0);
	}
}

LRESULT Cilx2saspDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{
	if (wParam != IDR_MAINFRAME) return 1;

	switch (lParam)
	{
	case WM_RBUTTONUP:
		{
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);
			CMenu menu;
			menu.CreatePopupMenu();

            menu.AppendMenu(MF_STRING, WM_DESTROY, L"Exit");
//          menu.AppendMenu(MF_STRING, ID_APP_EXIT, L"Exit");
            ::SetForegroundWindow(this->m_hWnd);
            menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, lpoint->x, lpoint->y, this);
            ::PostMessage(this->m_hWnd, WM_NULL, 0, 0);
			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
            ShowWindow(SW_SHOWNORMAL);
            ShowHideWindow(SW_SHOW);
		}
		break;
	}
	return 0;
}


void Cilx2saspDlg::OnDestroy()
{
    g_bThreadRun = FALSE;
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	CDialog::OnDestroy();
}

void Cilx2saspDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED)
	{
		ShowWindow(SW_HIDE);
	}
}

UINT MonitorFile(LPVOID pVar)
{
	int		nBufferSize			= 4096;
	char*	buffer				= new char[nBufferSize];
	HANDLE  hDirectoryHandle	= NULL;
    bool bIncludeSubdirectories = false;

    TCHAR   curXmlDir[MAX_PATH];
    TCHAR   curXml[MAX_PATH];

    Cilx2saspDlg *dlg = (Cilx2saspDlg *)pVar;
    dlg->GetDlgItem(IDC_EDIT_XML_FILE)->GetWindowText(curXmlDir, MAX_PATH);
    if (!IsFile(curXmlDir)) {
        return 1;
    }
    LPTSTR tszSlash = _tcsrchr(curXmlDir, _T('\\'));
    if (tszSlash) *tszSlash = _T('\0');
    _tcscpy_s(curXml, ++tszSlash);

	hDirectoryHandle = ::CreateFile(
		curXmlDir,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ
		| FILE_SHARE_WRITE
		| FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS
		| FILE_FLAG_OVERLAPPED,
		NULL);

	if(hDirectoryHandle == INVALID_HANDLE_VALUE)
		return 1;

	while(1)
	{
		DWORD dwBytes = 0;

		memset(buffer, 0, nBufferSize);

		if(!::ReadDirectoryChangesW(
			hDirectoryHandle,
			buffer,
			nBufferSize,
			bIncludeSubdirectories,
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME,
			&dwBytes,
			NULL,
			NULL) || GetLastError() == ERROR_INVALID_HANDLE)
		{
			break;
		}

		if(!dwBytes)
		{
			printf("Buffer overflow~~\r\n");
		}

		PFILE_NOTIFY_INFORMATION record = (PFILE_NOTIFY_INFORMATION)buffer;
		DWORD cbOffset = 0;

		do
		{
			switch (record->Action)
			{
			case FILE_ACTION_ADDED:
				break;
			case FILE_ACTION_REMOVED:
				break;
			case FILE_ACTION_MODIFIED:
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				break;
			default:
				break;
			}

            if (CString(record->FileName).MakeLower() == CString(curXml).MakeLower()) {
                dlg->RunPHP();
//                MessageBeep(-1);
            }

			cbOffset = record->NextEntryOffset;
			record = (PFILE_NOTIFY_INFORMATION)((LPBYTE) record + cbOffset);
		}while(cbOffset);
	}

	delete buffer;

	if(hDirectoryHandle)
		CloseHandle(hDirectoryHandle);

    return 0;
}

BOOL Cilx2saspDlg::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->wParam == VK_ESCAPE)
    {
        return true;
    }

    return CDialog::PreTranslateMessage(pMsg);
}
