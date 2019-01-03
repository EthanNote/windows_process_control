#define _CRT_SECURE_NO_WARNINGS
#include<json/json.h>
#include<json/value.h>
#include "CUDP.h"

#include<WinSock2.h>
#include<thread>
#include<iostream>
#include<sstream>

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


void print_json(Json::Value & data)
{
	try {
		data.getMemberNames();
		Json::StreamWriterBuilder wbuilder;
		std::string outputConfig = Json::writeString(wbuilder, data);
		std::cout << outputConfig << std::endl;
	}
	catch (Json::LogicError e) {
		std::cout << e.what() << std::endl;
	}
}


#include "CScript.h"
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
		//Json::CharReader reader;
		while (true) {
			memset(RecvBuf, 0, BufLen);
			recvfrom(obj->RecvSocket, RecvBuf, BufLen, 0, (SOCKADDR *)&SenderAddr, &SenderAddrSize);
			auto data = std::string(RecvBuf);
			Address addr;
			addr.addr = SenderAddr;
			memcpy(addr.ip_bytes, &SenderAddr.sin_addr.S_un, 4);
			addr.port = SenderAddr.sin_port;
			std::cout << "[" << addr.ip() << ":" << addr.port << "]" << RecvBuf << std::endl;

			if (obj->data_callback) {
				obj->data_callback(data, addr);
			}
			//obj->OnData(data, addr);

			/*std::string data = std::string(RecvBuf);

			auto result = script::on_data(data);
			std::cout << result << std::endl;*/

			//try {
			//	std::istringstream istr;
			//	istr.str(data);
			//	Json::Value root;
			//	istr >> root;

			//	//print_json(root);
			//	Json::StreamWriterBuilder wbuilder;
			//	std::string outputConfig = Json::writeString(wbuilder, root);
			//	std::cout << outputConfig << std::endl;
			//}
			//catch(Json::Exception e){
			//	std::cout << e.what() << std::endl;
			//}
			//auto names = root.getMemberNames();
			//print_json(root);

			//if (reader.parse(data, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
			//{
			//	//std::string upload_id = root["uploadid"].asString();  // 访问节点，upload_id = "UP000000"  
			//	//int code = root["code"].asInt();    // 访问节点，code = 100 
			//	std::cout << "[ type = JSON ]" << std::endl;
			//}
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
	return sendto(this->RecvSocket, str.c_str(), str.length(), 0, (sockaddr*)&addr.addr, sizeof(sockaddr));
}

bool CUDP::OnData(std::function<void(std::string &, Address &)> func)
{
	this->data_callback = func;
	return true;
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
