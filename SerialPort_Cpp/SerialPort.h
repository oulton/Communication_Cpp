#ifndef _SERIALPORT_H
#define _SERIALPORT_H
 
#include <string>
#include <Windows.h>
#include <cstddef>
#include <cstdlib>
#include <cassert>
 
// 定义串口类型
#ifdef _UNICODE
typedef CString porttype;
#else
typedef std::string porttype;
#endif // _UNICODE
 
typedef unsigned long ulong;
typedef unsigned char uchar;
 
 
class CSerialPort
{
public:
	CSerialPort(
		const porttype & portNum,		// 串口号
		DWORD baudRate = 9600,			// 波特率
		BYTE  byteSize = 8,				// 数据位
		BYTE  stopBit = ONESTOPBIT,		// 停止位
		BYTE  parityBit = NOPARITY		// 检验位		
		);
 
	~CSerialPort();
 
public:
 
	bool openComm();										// 打开串口
	void closeComm();										// 关闭串口
	bool writeToComm(BYTE data[], DWORD dwLegnth);			// 发送数据
	bool readFromComm(char buffer[], DWORD dwLength);		// 读取数据
 
private:
 
	HANDLE m_hComm;		// 通信设备
	porttype m_portNum; // 串口号
	DWORD m_dwBaudRate; // 波特率
	BYTE  m_byteSize;	// 数据位
	BYTE  m_stopBit;	// 停止位
	BYTE  m_parityBit;  // 校验位	
	bool  m_bOpen;		// 串口开关标志
private:
	
	enum BufferSize
	{
		MIN_BUFFER_SIZE = 256,
		BUFFER_SIZE = 512,
		MAX_BUFFER_SIZE = 1024
	};
 
	// 设置串口号
	void setPortNum(const porttype &portNum)
	{
		this->m_portNum = portNum;
	}
	// 设置波特率
	void setBaudRate(const ulong baudRate)
	{
		this->m_dwBaudRate = baudRate;
	}
	// 设置数据位
	void setByteSize(const uchar byteSize)
	{
		this->m_byteSize = byteSize;
	}
	// 设置检验位
	void setParityBit(const uchar parityBit)
	{
		this->m_parityBit = parityBit;
	}
	// 设置停止位
	void setStopBit(const uchar stopBit)
	{
		this->m_stopBit = stopBit;
	}
 
	// 获取串口号
	porttype getPortNum() { return m_portNum; }
	// 获取波特率
	ulong getBaudRate() { return m_dwBaudRate; }
	// 获取数据位
	uchar getByteSize() { return m_byteSize; }
	// 获取检验位
	uchar getParityBit() { return m_parityBit; }
	// 获取停止位
	uchar getStopBit() { return m_stopBit; }
};
 
 
#endif		// _SERIALPORT_H
