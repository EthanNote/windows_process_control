#include "CProcess.h"
#include "CScript.h"
#include "CUDP.h"
#include <signal.h>
#include<iostream>

//#pragma comment(lib, "jsoncpp.lib")

void on_ctrl_c(int n) {
	std::cout << "Terminating all process" << std::endl;
	process::factory::KillAll();
}

int main() {

	signal(SIGINT, on_ctrl_c);

	auto udp = udp::Create();
	udp->Open(7788);
	udp->OnData( [&udp](std::string & str, Address & addr){
		auto result = script::on_data(str);
		udp->Write(result, addr);
		});
	script::init();
	/*std::string testdata("hello");
	std::cout << script::on_data(testdata) << std::endl;*/
	script::run_repl();

	process::factory::KillAll();
	return 0;
}


