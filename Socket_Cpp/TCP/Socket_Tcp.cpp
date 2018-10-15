
#include "Socket_Tcp.h"

int Socket_Tcp::Create(char* IP)
{
	//WSAStartup();
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 1 );
    err = WSAStartup( wVersionRequested, &wsaData );
	if(err!=0)
		return INVALID_SOCKET;

	IPaddress = IP;

	m_Listening=socket(AF_INET,SOCK_STREAM,0);
    if (m_Listening==INVALID_SOCKET) 
		return INVALID_SOCKET;
	return true;
}

void Socket_Tcp::Close()
{
	closesocket(m_Listening);
	WSACleanup();
}

int Socket_Tcp::Bind(int Port)
{
	struct sockaddr_in serv_addr;
    
    serv_addr.sin_family        = AF_INET;       
    //serv_addr.sin_addr.s_addr   = htonl(INADDR_ANY);
	serv_addr.sin_addr.s_addr   = inet_addr("127.0.0.1");      //·þÎñÆ÷¶ËIPµØÖ·
    serv_addr.sin_port          = htons(Port);                
     
    if(bind(m_Listening, (struct sockaddr *)&serv_addr, sizeof (serv_addr))==SOCKET_ERROR)
		return SOCKET_ERROR;
	return true;
}

int Socket_Tcp::Connect(int Port)
{
	struct sockaddr_in serv_addr;
    
    serv_addr.sin_family        = AF_INET;       
	serv_addr.sin_addr.s_addr   = inet_addr(IPaddress);
    serv_addr.sin_port          = htons(Port);                
     
    if(connect(m_Listening, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==SOCKET_ERROR) 
		return SOCKET_ERROR;
    return true;
}

int Socket_Tcp::Listen(int backlog)
{
	return(listen(m_Listening,backlog));
}

SOCKET Socket_Tcp::Accept()
{
	SOCKET acc_soc;

    struct sockaddr_in client_addr;
    int client_addr_len =sizeof(client_addr);
    acc_soc=accept(m_Listening, (struct sockaddr *)&client_addr, &client_addr_len);
    
    return acc_soc;
}

int Socket_Tcp::Send(SOCKET ConnectSocket,char* data,int nBytes)
{
	int x;
    x=send(ConnectSocket,data,strlen(data),0);
    return x;  
}

int Socket_Tcp::Recive(SOCKET SocketTemp,int nBytes)
{
	char *buf;
    buf =(char*) malloc(nBytes);     
    recv(SocketTemp,buf,nBytes,0);  
	Rec_Data = buf;
	return 1;
}
