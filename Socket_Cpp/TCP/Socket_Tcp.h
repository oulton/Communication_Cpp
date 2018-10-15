#pragma once

//#include<winsock.h>
#include<WinSock2.h>
#include<stdlib.h>
#include<string.h>

#pragma comment(lib, "ws2_32.lib")

class Socket_Tcp
{   
  public:
	  SOCKET m_Listening;
	  char *IPaddress;
	  char *Rec_Data;
  public:
	  int Create(char* IP);
	  void Close();
	  int Bind(int Port);
	  int Connect(int Port);
	  int Listen(int backlog);
	  SOCKET Accept();
	  int Send(SOCKET ConnectSocket,char* data,int nBytes);
	  int Recive(SOCKET SocketTemp,int nBytes);
};
