
// ilx2saspDlg.h : header file
//

#include "ini.h"

#pragma once

#define WM_SHOWTASK					(WM_USER+101)


// Cilx2saspDlg dialog
class Cilx2saspDlg : public CDialog
{
// Construction
public:
	Cilx2saspDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ILX2SASP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	HICON       m_hIcon;
    CIniReader  *m_IniReader;
    BOOL        m_autorun;
    UINT		m_bClose;
    CWinThread *m_thread;
	NOTIFYICONDATA m_nid;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
    void LoadIni();
    void SaveIni();
	void RegAutoRun(bool cmd);
	void WatchFile();
    void ShowHideWindow(int cmd);
    BOOL PreTranslateMessage(MSG *pMsg);

public:
	afx_msg void OnBnClickedButtonSelPhpDir();
	afx_msg void OnBnClickedButtonSelPlaylistDir();
	afx_msg void OnBnClickedButtonSelXmlFile();
	afx_msg void OnBnClickedCheckAutorun();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnLbnSelchangeListIgnore();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	LRESULT OnShowTask(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void RunPHP();
};
