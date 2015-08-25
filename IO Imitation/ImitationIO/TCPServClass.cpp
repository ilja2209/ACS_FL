
#include "TCPServClass.h"


TCPServClass::TCPServClass(string IPaddress, string transmitPort, string receivePort):
	IP(IPaddress), transmitPort(transmitPort), receivePort(receivePort)
{
	InitSocket();
}

TCPServClass::TCPServClass(string NameFileConfig)
{
	try
	{
		ptree element;
		read_ini(NameFileConfig, element);
		IP = element.get<string>("IP", "");
		transmitPort = element.get<string>("ClientPort", "0");
		receivePort = element.get<string>("ServerPort", "0");
		
		InitSocket();
		
	}
	catch(const ptree_error &e)
	{
		throw e.what();
	}
}


TCPServClass::~TCPServClass()
{	
	if (m_TransmitSocket)
	{
		closesocket(m_ListenSocket);
		closesocket(m_TransmitSocket);
		closesocket(ClientSocket_Server);
	}

	WSACleanup();
}


void TCPServClass::InitServer()
{

	struct addrinfo *result;
    struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;	
	
	int res = getaddrinfo(NULL, receivePort.c_str(), &hints, &result);

	if (res != NO_ERROR)
	{
		WSACleanup();
		throw "getAddrInfo failed \n ";
	}


	m_ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_ListenSocket == INVALID_SOCKET) 
	{
        freeaddrinfo(result);
        WSACleanup();
        throw "socket failed ";
    }

	res = ::bind(m_ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	
	if (res == SOCKET_ERROR)
	{
		freeaddrinfo(result);
        closesocket(m_ListenSocket);
        WSACleanup();
		throw "bind failed";
	}

	res = listen(m_ListenSocket, 1);
	if (res == SOCKET_ERROR)
	{
		closesocket(m_ListenSocket);
        WSACleanup();
		throw "listen failed";
	}

	freeaddrinfo(result);

	ClientSocket_Server = accept(m_ListenSocket, NULL, NULL);
    if (ClientSocket_Server == INVALID_SOCKET) 
	{
        closesocket(m_ListenSocket);
        WSACleanup();
		throw "Accept failed";
    }

}

void TCPServClass::InitClient()
{

	sockaddr_in clientService;
    clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(IP.c_str());
	clientService.sin_port = htons(atoi(transmitPort.c_str())); 
	
	
	m_TransmitSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_TransmitSocket == INVALID_SOCKET) 
	{       
        WSACleanup();
        throw "transmit socket failed ";
    }

	//Блокируем поток и ждём, когда свершится соединение!
	for(;;)
	{
		int res = connect( m_TransmitSocket, (SOCKADDR *) & clientService, sizeof(clientService));
		if (res == SOCKET_ERROR) 
		{     
			printf("Connection is false\n");
		}
		else break;
	}
}

void TCPServClass::InitSocket()
{
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != NO_ERROR)
	{
		WSACleanup();
		throw "WSAStartup failed with error \n ";
	}
}


int TCPServClass::TransmitData(char *buffer, uint32 lengthBuffer)
{	
    int res = send( m_TransmitSocket, buffer, lengthBuffer, 0 );
    if (res == SOCKET_ERROR) 
	{
		closesocket(m_TransmitSocket);
        WSACleanup();
        throw "Send failed";
    }
	return 0;
}

int TCPServClass::ReceiveData(char *buffer, uint32 lengthBuffer)
{

	//closesocket(m_ListenSocket);
	int iResult = -1;


	iResult = recv(ClientSocket_Server, buffer, lengthBuffer, 0);
	if (iResult > 0)
		printf("Bytes is received %d\n", iResult);
	int res = send(ClientSocket_Server, buffer, iResult, 0);
		
	

	return iResult;
}

