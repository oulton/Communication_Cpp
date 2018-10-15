// Serial.cpp: implementation of the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Serial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


static DWORD WINAPI ReadThreadFunc(LPVOID lparam);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CSerial类的构造函数
CSerial::CSerial()
{
	m_hPortOwner = NULL;
	m_hComm = INVALID_HANDLE_VALUE;//串口句柄初始值为无效
	memset(&m_ov, 0, sizeof(OVERLAPPED));

//--------------------------------
	// init events
	m_ov.hEvent = NULL;
	m_hWriteEvent = NULL;
	m_hShutdownEvent = NULL;

	m_hReadThread = NULL;
	m_bSerialAlive = false;
//--------------------------------
}

//CSerial类的析构函数
CSerial::~CSerial()
{
//	ClosePort();
	do
	{
		SetEvent(m_hShutdownEvent);		// 设置关闭串口信号
	}while (m_bSerialAlive);

	if(m_ov.hEvent)
		CloseHandle(m_ov.hEvent);

	if(m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}

	if(m_hReadThread)
		CloseHandle(m_hReadThread);

}

/*
*函数介绍：打开串口
*入口参数：pPortOwner	:使用此串口类的窗体句柄
		   portNo		:串口号
		   baud			:波特率
		   parity		:奇偶校验
		   databits		:数据位
		   stopbits		:停止位
*出口参数：(无)
*返回值：TRUE:成功打开串口;FALSE:打开串口失败
*/
BOOL CSerial::OpenPort(HWND hPortOwner,/*使用串口类的窗体句柄*/
					   UINT portNum,	/*端口号*/
					   UINT baud,		/*波特率*/
					   UINT parity,		/*校验位*/
					   UINT databits,	/*数据位*/
					   UINT stopbits	/*停止位*/
					   )
{
	
	TCHAR sPort[15];

	if(m_hComm != INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}

	ASSERT(hPortOwner != NULL);
//---------------------------------------------------------
	// create events
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// initialize the event objects
	m_hEventArray[0] = m_hShutdownEvent;	// highest priority
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent;
//------------------------------------------------------------
	m_portNr = portNum;
	m_hPortOwner = hPortOwner;


	//设置串口名
	wsprintf(sPort,L"COM%d:",portNum);
	//打开串口
	m_hComm = CreateFile(
		sPort,							/*端口号*/
		GENERIC_READ | GENERIC_WRITE,	/*可读写*/
		0,								
		NULL,							
		OPEN_EXISTING,					/*打开而不是创建*/
		FILE_FLAG_OVERLAPPED,//0,		/*异步IO方式打开*/						
		NULL							
		);
	
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		//返回无效句柄
//		TRACE(_T("CreateFile 返回无效句柄"));
		MessageBox(m_hPortOwner,_T("打开串口失败!"),_T("提示"),MB_OK);
		return FALSE;
	}

	//获得当前串口的配置属性
	if(!GetCommState(m_hComm,&commParamm))
	{
		return FALSE;
	}

	commParamm.fBinary = TRUE;		/*二进制模式*/
	commParamm.fParity = TRUE;		/*支持奇偶校验*/
	commParamm.BaudRate = baud;		/*波特率*/
	commParamm.ByteSize = databits;	/*数据位*/
	commParamm.Parity = parity;		/*奇偶校验*/
	commParamm.StopBits = stopbits;	/*停止位*/

	commParamm.fOutxCtsFlow = FALSE;				// No CTS output flow control 
	commParamm.fOutxDsrFlow = FALSE;				// No DSR output flow control 
	commParamm.fDtrControl = DTR_CONTROL_ENABLE; 
	// DTR flow control type 
	commParamm.fDsrSensitivity = FALSE;			// DSR sensitivity 
	commParamm.fTXContinueOnXoff = TRUE;			// XOFF continues Tx 
	commParamm.fOutX = FALSE;					// No XON/XOFF out flow control 
	commParamm.fInX = FALSE;						// No XON/XOFF in flow control 
	commParamm.fErrorChar = FALSE;				// Disable error replacement 
	commParamm.fNull = FALSE;					// Disable null stripping 
	commParamm.fRtsControl = RTS_CONTROL_ENABLE; 
	// RTS flow control 
	commParamm.fAbortOnError = FALSE;			// 当串口发生错误，并不终止串口读写

	if (!SetCommState(m_hComm, &commParamm))
	{
//		TRACE(_T("SetCommState error"));
//		AfxMessageBox(_T(""));
		MessageBox(m_hPortOwner,_T("设置串口失败!"),_T("提示"),MB_OK);
		return FALSE;
	}

    //设置串口读写超时
	
	GetCommTimeouts (m_hComm, &CommTimeOuts);
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD; 
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;  
	CommTimeOuts.ReadTotalTimeoutConstant = 0;    
	CommTimeOuts.WriteTotalTimeoutMultiplier = 10;  
	CommTimeOuts.WriteTotalTimeoutConstant = 1000;  
	
	if(!SetCommTimeouts(m_hComm, &CommTimeOuts ))
	{
//		TRACE( _T("SetCommTimeouts 返回错误") );
//		AfxMessageBox(_T(""));

		return FALSE;
	}


	//设置端口监视的事件子集
	SetCommMask(m_hComm,EV_RXCHAR);

	//分配设备缓冲区
	SetupComm(m_hComm,1024,1024);

	//初始化缓冲区中的信息
	PurgeComm(m_hComm,PURGE_TXCLEAR | PURGE_RXCLEAR);
	

	//创建读线程句柄
	m_hReadThread = CreateThread(NULL,0,ReadThreadFunc,this,0,&m_dwReadThreadID);
	
//	TRACE(_T("串口打开成功"));
	
	return TRUE; 
}

/*
*函数介绍：关闭串口
*入口参数：(无)
*出口参数：(无)
*返回值：  (无)
*/
void CSerial::ClosePort()
{
	//串口还没打开
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	//关闭线程
	CloseReadThread();

	//关闭串口
	CloseHandle(m_hComm);//lx
	m_hComm = INVALID_HANDLE_VALUE;
}



/*
*函数介绍：向串口发送数据
*入口参数：buf		: 将要往串口写入的数据的缓冲区
		   bufLen	: 将要往串口写入的数据的缓冲区长度
*出口参数：(无)
*返回值：TRUE:表示成功地将要发送的数据传递到写线程消息队列。
		 FALSE:表示将要发送的数据传递到写线程消息队列失败。
		 注视：此处的TRUE,不直接代表数据一定成功写入到串口了。
*/
BOOL CSerial::WritePort(const BYTE* buf,DWORD bufLen)
{
//	DWORD dwWriteLen;		//读取字节数
	DWORD dwHaveWriteLen = 0;	//已读取字节数

	if(m_hComm == INVALID_HANDLE_VALUE)
		return false;
	m_WriteLength = bufLen;//最多一次发送150字节左右，超过会出错(有待改进)
//	strcpy((char *)m_szWriteBuffer,(const char *)buf);
/*
	for(int i=0;i<bufLen;i++)
	{
		m_szWriteBuffer[i] = (unsigned char)buf[i];
	}
*/
	m_szWriteBuffer = (unsigned char *)buf;
	// set event for write
	SetEvent(m_hWriteEvent);

/*
	do{
		BOOL bl = WriteFile(m_hComm,buf+dwHaveWriteLen,bufLen-dwHaveWriteLen,&dwWriteLen,&m_ov);
		if(bl)
		{
			dwHaveWriteLen = dwHaveWriteLen + dwWriteLen;
			if(dwHaveWriteLen >= bufLen)
			{
				break ;
			}
		}
		else
		{
			return FALSE;
		}
	}while(TRUE);
*/
	return TRUE;

}

void CSerial::CloseReadThread()
{
	SetEvent(m_hShutdownEvent);	

	//清空所有将要读的数据
	PurgeComm(m_hComm, PURGE_TXCLEAR|PURGE_RXCLEAR);
    //等待1秒，如果读线程没有退出，则强制退出
    if (WaitForSingleObject(m_hReadThread,50) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hReadThread,0);
	}

	if(m_hReadThread)
		CloseHandle(m_hReadThread);//lx
	m_hReadThread = NULL;
	
}
//串口读线程函数
static DWORD WINAPI ReadThreadFunc(LPVOID lparam)
{
	CSerial *pSerial = (CSerial*) lparam;

	BYTE* readBuf = NULL;	//存放读取缓冲区
	DWORD willReadLen = 0;		//要读取的字节数
	DWORD actualReadLen = 0;	//实际读取的字节数

	DWORD evtMask;			//接收事件掩码
	DWORD dwReadErrors;		//通信错误掩码
	COMSTAT comState;		//COMSTAT型变量

	DWORD dw_Evt = 0;
	DWORD dwError = 0;


	//检查串口是否打开
	ASSERT(pSerial->m_hComm != INVALID_HANDLE_VALUE);
	//清空串口(清空输入、输出缓存)
	PurgeComm(pSerial->m_hComm,PURGE_TXCLEAR | PURGE_TXCLEAR);
	//设置串口的事件子集
//	SetCommMask(pSerial->m_hComm,EV_RXCHAR);
	pSerial->m_bSerialAlive = true;

	while(TRUE)
	{
		if(WaitCommEvent(pSerial->m_hComm,&evtMask,&pSerial->m_ov))
		{
			ClearCommError(pSerial->m_hComm,&dwReadErrors,&comState);
			if(comState.cbInQue < 0)
			{
				continue ;
			}
			
		}
		else
		{
			switch (dwError = GetLastError()) 
			{ 
			case ERROR_IO_PENDING: 	
				{ 
					// This is a normal return value if there are no bytes
					// to read at the port.
					// Do nothing and continue
					break;
				}
			case 87:
				{
					// Under Windows NT, this value is returned for some reason.
					// I have not investigated why, but it is also a valid reply
					// Also do nothing and continue.
					break;
				}
			default:
				{
					// All other error codes indicate a serious error has
					// occured.  Process this error.
//					pSerial->ProcessErrorMessage(_T("WaitCommEvent()"));
					break;
				}
			}
		}

		//---------------------------------------------------
		dw_Evt = WaitForMultipleObjects(3, pSerial->m_hEventArray, FALSE, INFINITE);

		switch(dw_Evt)
		{
		case 0://关闭串口事件
			pSerial->m_bSerialAlive = false;

			AfxEndThread(100);
			break;
		case 1://串口收到数据事件
			GetCommMask(pSerial->m_hComm, &evtMask);  // 获取事件掩码
			if(evtMask & EV_RXCHAR)
			{

				willReadLen = 4096;
				

				readBuf = new BYTE[willReadLen];
				ReadFile(pSerial->m_hComm,readBuf,willReadLen,&actualReadLen,&pSerial->m_ov);

				if(actualReadLen > 0 && actualReadLen < 0x0000FFFF)
				{
					//将串口号也通过消息发送到父窗口，串口号放到actualReadLen的高两位
					actualReadLen |= (pSerial->m_portNr<<24);
					//触发回调函数
					//pSerial->m_OnSeriesRead(pSerial->m_hPortOwner,readBuf,actualReadLen);

					::SendMessage(pSerial->m_hPortOwner, WM_COMM_RXCHAR, actualReadLen, (LPARAM)readBuf);
					//处理串口数据
					//SerialDataProc(readBuf,actualReadLen);

					//pSerial->WritePort(readBuf,actualReadLen);//test

				}

				delete []readBuf;
			}
			break;
		case 2://发送数据事件
			DWORD dwWriteLen;		//读取字节数
//			DWORD dwHaveWriteLen = 0;	//已读取字节数\

			ResetEvent(pSerial->m_hWriteEvent);

			// Initailize variables
			pSerial->m_ov.Offset = 0;
			pSerial->m_ov.OffsetHigh = 0;

			// Clear buffer
			PurgeComm(pSerial->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
			if(!WriteFile(pSerial->m_hComm,pSerial->m_szWriteBuffer,pSerial->m_WriteLength,&dwWriteLen,&pSerial->m_ov))
			{
				dwWriteLen =dwWriteLen;
				break;//异步IO返回失败可能是还没有发送完
			}

			break;
		}
		//---------------------------------------------------

	}
	return 0;
}
/*
void CSerial::ProcessErrorMessage(CString str)
{
	char *Temp = new char[200];

	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

	sprintf(Temp, "WARNING:  %s Failed with the following error: \n%s\nPort: %d\n", (char*)ErrorText, lpMsgBuf, m_portNr); 
	MessageBox(NULL, (LPCTSTR)Temp, (LPCTSTR)"Application Error", MB_ICONSTOP);

	LocalFree(lpMsgBuf);
	delete[] Temp;
}
*/