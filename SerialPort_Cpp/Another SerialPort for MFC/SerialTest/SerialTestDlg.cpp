// SerialTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SerialTest.h"
#include "SerialTestDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSerialTestDlg dialog




CSerialTestDlg::CSerialTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSerialTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSerialTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COM, m_cmb_com);
	DDX_Control(pDX, IDC_COMBO_BAUD, m_cmb_baud);
}

BEGIN_MESSAGE_MAP(CSerialTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMM_RXCHAR,OnSerialReceive)
	ON_BN_CLICKED(IDC_BUTTON1, &CSerialTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSerialTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CSerialTestDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CSerialTestDlg message handlers

BOOL CSerialTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_cmb_com.AddString(_T("COM1"));
	m_cmb_com.AddString(_T("COM2"));
	m_cmb_com.AddString(_T("COM3"));
	m_cmb_com.AddString(_T("COM4"));

	m_cmb_com.SetCurSel(0);

	m_cmb_baud.AddString(_T("9600"));
	m_cmb_baud.AddString(_T("19200"));
	m_cmb_baud.AddString(_T("38400"));
	m_cmb_baud.AddString(_T("115200"));

	m_cmb_baud.SetCurSel(1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSerialTestDlg::OnPaint()
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
HCURSOR CSerialTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSerialTestDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here

	CString str,strCOM,strBaud;
	int com,baud;

	GetDlgItemText(IDC_BUTTON1,str);
//	GetDlgItemText(IDC_COMBO_COM,strCOM);
	com=m_cmb_com.GetCurSel()+1;
	GetDlgItemText(IDC_COMBO_BAUD,strBaud);
	baud = _ttoi(strBaud);

	if(str==_T("打开串口"))
	{

		if(m_Serial.OpenPort(this->m_hWnd,com,baud))
		{
			m_Serial.m_OnSeriesRead = m_OnSeriesRead;	//设置回调函数接口
			SetDlgItemText(IDC_BUTTON1,_T("关闭串口"));
		}
	}
	else if(str==_T("关闭串口"))
	{
		m_Serial.ClosePort();
		SetDlgItemText(IDC_BUTTON1,_T("打开串口"));
	}


}

void __stdcall CSerialTestDlg::m_OnSeriesRead(HWND hWnd, unsigned char* readBuf, int actualReadLen)
{
	int i = 0;
	CString str,str1;

	for(i=0;i<actualReadLen;i++)
	{
		str.Format(_T("%c"),readBuf[i]);
		str1+=str;
	}
	str1+=_T("\r\n");
//	::SetDlgItemText(pArg.m_hWnd,IDC_EDIT_OUT,str1);
}

LRESULT CSerialTestDlg::OnSerialReceive(WPARAM wParam, LPARAM lParam)
{
	int i = 0;
	CString str;
	CString str1;
	BYTE data=0;
	BYTE *pch;
	int Len=0;
	char port=0;


	port= ((int)wParam>>24) & 0xFF;
	Len=(int)wParam & 0x0000FFFF;
	pch = (BYTE*)lParam;

	GetDlgItemText(IDC_EDIT_OUT,str1);
	if(Len<4096)
//		m_Serial.WritePort(pch,Len);
	for(i=0;i<Len;i++)
	{
		str.Format(_T("%c"),pch[i]);
		str1+=str;
/*
		if(port == 2)
			str.Format(_T("%c"),pch[i]);
		else if(port == 4)
			str.Format(_T("%c  "),pch[i]);
		str1+=str;
*/
	}
	str1+=_T("\r\n");
	SetDlgItemText(IDC_EDIT_OUT,str1);

	return true;
}
void CSerialTestDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here

	SetDlgItemText(IDC_EDIT_OUT,_T(""));
}

void CSerialTestDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	CString str;
	//////
	char *pch=NULL;
	TCHAR *ptch=NULL;
	int nLen=0;
	char ch[200]={0};

	USES_CONVERSION;	//W2A,A2W
	//////

	GetDlgItemText(IDC_EDIT_SEND,str);
	nLen = str.GetLength();
/*
	nLen=dim(str);
	
	char *p = (LPSTR)(LPCTSTR)str;


	LPTSTR lpsz = new TCHAR[str.GetLength()+1];
	_tcscpy(lpsz, str);
*/
	strcpy_s(ch,W2A(str));


//	_tcscpy(ptch,str);//将字符串拷贝到TCHAR* 的ptch指针
//	pch = new char[100];
//	ptch = (LPCTSTR)str;

//	nLen=wcslen(ptch)+1;//得到ptch长度，宽字节的

//	for(int i= 0;i<nLen;i++)
//	{
//		pch[i]=ptch[i]&0xff;
//	}
//	WideCharToMultiByte(CP_ACP,0,ptch,nLen,pch,nLen*2,NULL,NULL);
/**/
//	GetWindowText (GetDlgItem (this, IDC_EDIT_SEND), szText, dim(szText));
//	StringCchCat (szText, dim(szText), TEXT ("\r\n"));
//	wcstombs_s (&siz, szAnsi, sizeof (szAnsi), szText, _TRUNCATE);

//	const TCHAR* ch = (LPCTSTR)str;
	m_Serial.WritePort((BYTE*)ch,(int)(strlen(ch)));
}
