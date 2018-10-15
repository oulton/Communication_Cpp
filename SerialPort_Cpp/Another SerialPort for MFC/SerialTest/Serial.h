// Serial.h: interface for the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIAL_H__B0D1DE90_996A_4034_92E9_6190452ED811__INCLUDED_)
#define AFX_SERIAL_H__B0D1DE90_996A_4034_92E9_6190452ED811__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include "struct.h"
#define WM_COMM_RXCHAR				WM_USER+7	// A character was received and placed in the input buffer. 

//定义串口接收数据函数类型
typedef void (CALLBACK* ONSERIALREAD)(HWND hWnd,BYTE* buf,int bufLen);
//typedef void (* ONSERIALREAD)(HWND hWnd,BYTE* buf,int bufLen);

class CSerial  
{
public:
	CSerial();
	virtual ~CSerial();

public:
	//打开串口
	BOOL OpenPort(HWND hPortOwner,			/*使用串口类，窗体句柄*/
				  UINT portNo	= 1,		/*串口号*/
				  UINT baud		= 9600,		/*波特率*/
				  UINT parity	= NOPARITY, /*奇偶校验*/
				  UINT databits	= 8,		/*数据位*/
				  UINT stopbits	= 0			/*停止位*/
				  );
	//关闭串口
	void ClosePort();
	//向串口写入数据
	BOOL WritePort(const BYTE *buf,DWORD bufLen);

private:
	//关闭读线程
	void CloseReadThread();

private:

	
	//读线程句柄
	HANDLE m_hReadThread;
	//读写线程ID标识
	DWORD m_dwReadThreadID;
		

public:
//	void ProcessErrorMessage(char* ErrorText);

public:
	
	//已打开的串口句柄
	HWND m_hPortOwner;

	bool m_bSerialAlive;//串口打开标识
	
	UINT m_portNr;//串口号
	
	CWnd* m_pPortOwner;//打开串口的窗体指针

	HANDLE	m_hComm;				//static

	ONSERIALREAD m_OnSeriesRead; //串口读取回调函数

	//设置串口超时事件
	COMMTIMEOUTS CommTimeOuts;

	//异步I/O 结构体
	OVERLAPPED			m_ov;

	//DCB
	DCB commParamm;

	HANDLE				m_hEventArray[3];
	HANDLE				m_hWriteEvent; //串口写数据事件
	HANDLE				m_hShutdownEvent;//串口关闭事件

	unsigned char*		m_szWriteBuffer;	// 写缓冲区
	DWORD				m_WriteLength;

};

#endif // !defined(AFX_SERIAL_H__B0D1DE90_996A_4034_92E9_6190452ED811__INCLUDED_)
