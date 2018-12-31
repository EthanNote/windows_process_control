#include "CProcess.h"
#include "CScript.h"
#include "CUDP.h"
#include <signal.h>
#include<iostream>

//#pragma comment(lib, "jsoncpp.lib")

void on_ctrl_c(int n) {
	std::cout << "Terminating all process"<< std::endl;
	process::factory::KillAll();
}

int main() {

	signal(SIGINT, on_ctrl_c);

	auto udp = udp::Create();
	udp->Open(7788);

	script::init();
	script::run_repl();
	auto p = process::factory::Create("test", "python");
	p->Run();

	script::run_repl();

	process::factory::KillAll();
	script::run_repl();
	return 0;
}


