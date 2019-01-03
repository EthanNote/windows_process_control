#pragma once
#include<string>
class CScript
{
public:
	CScript();
	~CScript();
};

namespace script {
	void init();
	std::string on_data(std::string& data);
	void run_repl();
}