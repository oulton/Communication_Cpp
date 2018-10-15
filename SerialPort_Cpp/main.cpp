#include <iostream>

#include "SerialPort.h"
using namespace std;
 
int main()
{
	CSerialPort serialPort("COM6",115200,8,1,0);
 
	BYTE sendData[5] = {1,2,128,4,3};
	char recvBuf[100] = { 0 };
 
	serialPort.openComm();
	serialPort.writeToComm(sendData, 5);
	serialPort.readFromComm(recvBuf, 5);
 
	serialPort.closeComm();
 
	system("pause");
	return 0;
}
