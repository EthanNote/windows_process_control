#include "CProcess.h"
#include<iostream>
#include<Windows.h>
#include<map>


class SimpleProcess : public CProcess {
	STARTUPINFO stStartUpInfo;
	PROCESS_INFORMATION stProcessInfo;
	std::string cmd;
	friend Process process::factory::Create(std::string name, std::string cmd);
	virtual void WriteStream(std::ostream & stream) override;
public:
	virtual int Run() override;
	virtual int Kill() override;
	virtual bool IsRunning() override;
};


void SimpleProcess::WriteStream(std::ostream & stream)
{
	stream << this->cmd;
}

int SimpleProcess::Run()
{
	::memset(&stStartUpInfo, 0, sizeof(stStartUpInfo));
	stStartUpInfo.cb = sizeof(stStartUpInfo);
	::memset(&stProcessInfo, 0, sizeof(stProcessInfo));

	return CreateProcess(
		NULL,
		(LPSTR)cmd.c_str(),
		NULL,
		NULL,
		false,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&stStartUpInfo,
		&stProcessInfo);
}

std::string ok_or_fail(int v) {
	return v ? "[ok]" : "[fail]";
}

int SimpleProcess::Kill()
{
	std::cout << "Kill process " << this->stProcessInfo.hProcess << ", ";
	auto result = TerminateProcess(this->stProcessInfo.hProcess, 0);
	std::cout << ok_or_fail(result) << std::endl;
	return 0;
}


bool SimpleProcess::IsRunning()
{
	return false;
}






std::map<std::string, Process> processes;

Process process::factory::Create(std::string name, std::string cmd)
{
	if (processes[name] != nullptr) {
		return nullptr;
	}

	auto ptr = new SimpleProcess();
	ptr->cmd = cmd;
	auto result = Process(ptr);
	processes[name] = result;

	return result;
}



void process::factory::KillAll()
{
	auto iter = processes.begin();
	while (iter != processes.end())
	{
		iter->second->Kill();
		iter++;
	}
}



std::ostream & operator<<(std::ostream & stream, CProcess & process)
{
	// TODO: 在此处插入 return 语句
	process.WriteStream(stream);
	return stream;
}


void CProcess::WriteStream(std::ostream & stream)
{
	stream << "[ Process@" << this << " ]";
}
