#pragma once
#include<memory>
#include<string>
#include<iostream>

class CProcess
{
	virtual void WriteStream(std::ostream & stream);
public:
	virtual int Run()=0;
	virtual int Kill()=0;
	virtual bool IsRunning()=0;
	std::string name;
	std::string command;
	virtual std::string GetRunningState()=0;

	friend std::ostream & operator<<(std::ostream & stream, CProcess & process);
};


typedef std::shared_ptr<CProcess> Process;


namespace process {
	namespace factory {
		Process Create(std::string name, std::string cmd);
		void KillAll();
	}
}