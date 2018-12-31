#define _CRT_SECURE_NO_WARNINGS
#include<json/json.h>
#include "CUDP.h"

#include<WinSock2.h>
#include<thread>
#include<iostream>

#pragma comment(lib,"WS2_32.lib")
//#pragma comment(lib,"jsoncpp.lib")
bool isStartup = false;


class CWinUDP : public CUDP {

	SOCKET RecvSocket;
	sockaddr_in RecvAddr;

	std::thread* server_thread = NULL;

	int Open(int port) override;
	int Close() override;
	int Write(std::string, Address &) override;
};


void print_json(Json::Value data)
{
	Json::Value::Members mem = data.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); iter++)
	{
		std::cout << *iter << "\t: ";
		if (data[*iter].type() == Json::objectValue)
		{
			std::cout << std::endl;
			print_json(data[*iter]);
		}
		else if (data[*iter].type() == Json::arrayValue)
		{
			std::cout << std::endl;
			auto cnt = data[*iter].size();
			for (auto i = 0; i < cnt; i++)
			{
				print_json(data[*iter][i]);
			}
		}
		else if (data[*iter].type() == Json::stringValue)
		{
			std::cout << data[*iter].asString() << std::endl;
		}
		else if (data[*iter].type() == Json::realValue)
		{
			std::cout << data[*iter].asDouble() << std::endl;
		}
		else if (data[*iter].type() == Json::uintValue)
		{
			std::cout << data[*iter].asUInt() << std::endl;
		}
		else
		{
			std::cout << data[*iter].asInt() << std::endl;
		}
	}
	return;
}



int CWinUDP::Open(int port)
{
	if (!isStartup) {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}
	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(port);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(RecvSocket, (SOCKADDR *)&RecvAddr, sizeof(RecvAddr));

	server_thread = new std::thread([](CWinUDP* obj) {
		char RecvBuf[1024];
		int BufLen = 1024;
		sockaddr_in SenderAddr;
		int SenderAddrSize = sizeof(SenderAddr);
		Json::Reader reader;
		Json::Value root;
		while (true) {
			memset(RecvBuf, 0, BufLen);
			recvfrom(obj->RecvSocket, RecvBuf, BufLen, 0, (SOCKADDR *)&SenderAddr, &SenderAddrSize);
			auto data = std::string(RecvBuf);
			Address addr;
			memcpy(addr.ip_bytes, &SenderAddr.sin_addr.S_un, 4);
			addr.port = SenderAddr.sin_port;
	
			if (obj->data_callback) {
				obj->data_callback(data, addr);
			}
			//obj->OnData(data, addr);
			std::cout <<"[" <<addr.ip()<<":"<<addr.port << "]" << RecvBuf << std::endl;

			if (reader.parse(data, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
			{
				//std::string upload_id = root["uploadid"].asString();  // 访问节点，upload_id = "UP000000"  
				//int code = root["code"].asInt();    // 访问节点，code = 100 
				std::cout << "[ type = JSON ]" << std::endl;
				print_json(root);
			}
		}
	}, this);

	return 0;
}

int CWinUDP::Close()
{
	closesocket(RecvSocket);
	if (server_thread) {
		server_thread->join();
	}
	return 0;
}

int CWinUDP::Write(std::string str, Address & addr)
{
	return 0;
}

bool CUDP::OnData(std::function<void(std::string &, Address &)>)
{
	return false;
}

//void CUDP::OnData(std::string)
//{
//}

CUDP::CUDP()
{
}


CUDP::~CUDP()
{
}

UDP udp::Create()
{
	return UDP(new CWinUDP);
}

std::string Address::ip()
{
	char ipstring[24]{ 0 };
	sprintf(ipstring, "%d.%d.%d.%d", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);
	return std::string(ipstring);
}
