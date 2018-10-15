#include "Socket_Tcp.h"
#include <iostream>
using namespace std;

int main()
{
	Socket_Tcp myTest;

	if(myTest.Create("127.0.0.1") == INVALID_SOCKET)
		cout<<"Creat Error"<<endl;

	if(myTest.Connect(2000) == SOCKET_ERROR)
		cout<<"Connect Error"<<endl;

	int a;
	a = myTest.Send(myTest.m_Listening,"收到请回答!",11);
	myTest.Recive(myTest.m_Listening,5);
	cout<<myTest.Rec_Data<<endl;

	system("pause");
	myTest.Close();
	return 0;
}
