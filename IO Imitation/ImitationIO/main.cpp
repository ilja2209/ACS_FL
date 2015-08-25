

#include <iostream>
#include <stdio.h>
#include <thread>
#include <mutex>
#include "TCPServClass.h"


using namespace std;


class NetDataInterface
{
	char *buffer_in;
	char *buffer_out;

	int SIZE_BUFFER_OUT, SIZE_BUFFER_IN;

	mutex mutex_buffer_in;
	mutex mutex_buffer_out;
	TCPServClass *tcp;

public:
	NetDataInterface(int size_buffer_out, int size_buffer_in);
	~NetDataInterface();

	void getDataFromSAU_LL(char *buffer, uint32 size);
	void writeDataToSAU_LL(char *buffer, uint32 size);

	void threadReaddataFromSAU_LL();
	void threadWritedataToSAU_LL();

};

NetDataInterface::NetDataInterface(int size_buffer_out, int size_buffer_in) : SIZE_BUFFER_OUT(size_buffer_out), SIZE_BUFFER_IN(size_buffer_in)
{
	tcp = new TCPServClass("config.ini");
	buffer_out = new char[SIZE_BUFFER_OUT];
	buffer_in = new char[SIZE_BUFFER_IN];
}

NetDataInterface::~NetDataInterface() 
{
	tcp->~TCPServClass();
	delete[] buffer_out;
	delete[] buffer_in;
}

void NetDataInterface::threadReaddataFromSAU_LL()
{
	tcp->InitServer();

	for(;;)
	{
		try
		{
			lock_guard<mutex> m(mutex_buffer_in);
			tcp->ReceiveData(buffer_in, SIZE_BUFFER_IN);
			m.~lock_guard();
		}
		catch(char *err)
		{
			printf("Error %s\n", err);
		}
		Sleep(50);	
	}
}

void NetDataInterface::threadWritedataToSAU_LL()
{
	tcp->InitClient();

	for (;;)
	{		
		try
		{
			lock_guard<mutex> m(mutex_buffer_out);
			tcp->TransmitData(buffer_out, SIZE_BUFFER_OUT);
			m.~lock_guard();
			printf("Data has been sant \n");
		}
		catch(char *err)
		{
			printf("Error: %s\n", err);
		}
		Sleep(50);		
	}
}

void NetDataInterface::getDataFromSAU_LL(char *buffer, uint32 size)
{
	if (size > SIZE_BUFFER_OUT)
	{
		size = SIZE_BUFFER_OUT;
		printf("Warning: Size of input data is bigger than that specified at initialization \n");
	}
	lock_guard<mutex> m(mutex_buffer_in);
	memcpy(buffer, buffer_in, size);
	printf("Data has been read \n");
}

void NetDataInterface::writeDataToSAU_LL(char *buffer, uint32 size)
{
	if (size > SIZE_BUFFER_OUT)
	{
		size = SIZE_BUFFER_OUT;
		printf("Warning: Size of input data is bigger than that specified at initialization \n");
	}
	lock_guard<mutex> m(mutex_buffer_out);
	ZeroMemory(buffer_out, size);
	memcpy(buffer_out, buffer, size);
	printf("Data has been written \n");
}


//###################################################################################################


int main(int argc, char *argv[])
{
	cout << "There is I/O simulator of PKV-8" << endl;
	
	try
	{
		NetDataInterface netInterface(20, 20);
		thread threadOutData(&NetDataInterface::threadWritedataToSAU_LL, &netInterface);
		thread threadInputData(&NetDataInterface::threadReaddataFromSAU_LL, &netInterface);
		
		char buffer[20];
		char buffer_in[20];
		char count = 0;

		for (;;) 
		{
			memcpy(&buffer, "Hello world and pea", 20);
			++count;
			if (count > 250)
				count =0;
			buffer[1] = count;
			netInterface.writeDataToSAU_LL(&buffer[0], 20);
			Sleep(50);
			
			netInterface.getDataFromSAU_LL(&buffer_in[0], 20);
			cout << buffer_in << endl;

			Sleep(50);
			
		}
		
	}
	catch (...)
	{
		cout << "Error";
		return -1;
	}

	getchar();
	return 0;
}