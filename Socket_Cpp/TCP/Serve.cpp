#include "Socket_Tcp.h"
#include <iostream>
using namespace std;

int main()
{
	Socket_Tcp myTest;

	if(myTest.Create("127.0.0.1") == INVALID_SOCKET)
		cout<<"Creat Error"<<endl;

	if(myTest.Bind(2000) == SOCKET_ERROR)
		cout<<"Bind Error"<<endl;

	if(myTest.Listen(40) == SOCKET_ERROR)
		cout<<"Listen Error"<<endl;

	SOCKET ConnectSocket = myTest.Accept();
	myTest.Recive(ConnectSocket,11);
	cout<<myTest.Rec_Data<<endl;

	myTest.Send(ConnectSocket,"收到!",5);
	system("pause");

	myTest.Close();
	return 0;
}
