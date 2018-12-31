#pragma once
class CScript
{
public:
	CScript();
	~CScript();
};

namespace script {
	void init();
	void run_repl();
}