// SerialTestDlg.h : header file
//

#pragma once

#include "Serial.h"
#include "afxwin.h"

// CSerialTestDlg dialog
class CSerialTestDlg : public CDialog
{
// Construction
public:
	CSerialTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SERIALTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
public:
	CSerial m_Serial;

	static void __stdcall m_OnSeriesRead(HWND hWnd,unsigned char* readBuf, int actualReadLen);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmb_com;
	CComboBox m_cmb_baud;
	afx_msg LRESULT OnSerialReceive(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
