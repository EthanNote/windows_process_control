#pragma once
#include<string>
#include<memory>
#include<functional>

class Address {
public:
	std::string ip();
	unsigned char ip_bytes[4];
	int port;
};

class CUDP
{
public:
	std::function<void(std::string &, Address &)> data_callback = nullptr;

	virtual int Open(int port)=0;
	virtual int Close()=0;
	virtual int Write(std::string, Address &)=0;
	bool OnData(std::function<void(std::string &, Address &)>);
	CUDP();
	~CUDP();
};

typedef std::shared_ptr<CUDP> UDP;
namespace udp {
	UDP Create();
}