
#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;
using namespace boost;
using namespace property_tree;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


class TCPServClass
{
private:
	string transmitPort;
	string receivePort;	
	string IP; //IP порт для передатчика

	WSAData wsaData;

	SOCKET m_TransmitSocket;
	SOCKET m_ListenSocket;
	SOCKET ClientSocket_Server;

	void InitSocket();
	
public:
	void InitServer();
	void InitClient();
	TCPServClass(string IPaddress, string transmitPort, string receivePort);
	TCPServClass(string NameFileConfig);
	~TCPServClass();
	int TransmitData(char *buffer, uint32 lengthBuffer);
	int ReceiveData(char *buffer, uint32 lengthBuffer);
};