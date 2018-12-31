#include "CScript.h"
#include "CProcess.h"

#include<iostream>
#include <sstream>
#include<string>
#include<vector>
#include<map>
#include<Windows.h>
using namespace std;

CScript::CScript()
{
}


CScript::~CScript()
{
}

extern "C" {
#include<lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
extern void doREPL(lua_State *L);

//static std::thread* idle_thread = NULL;
lua_State *L = NULL;



static int add(lua_State *L) {
	auto name = luaL_checkstring(L, 1);
	auto cmd = luaL_checkstring(L, 2);

	auto process = process::factory::Create(name, cmd);
	if (process == nullptr) {
		lua_pushboolean(L, 0);
	}
	else {
		lua_pushboolean(L, 1);
	}
	return 1;
}

extern std::map<std::string, Process> processes;

static int ls(lua_State *L) {
	int top = lua_gettop(L);
	if (top == 0) {
		CONSOLE_SCREEN_BUFFER_INFO binfo;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &binfo);
		//cout << binfo.dwSize.X << "  " << binfo.dwSize.Y << endl;
		int buffer_width = binfo.dwSize.X;
		int name_width = 12;
		int content_width = buffer_width - name_width;

		for (auto iter = processes.begin(); iter != processes.end(); iter++) {
			auto name(iter->first);
			auto cmd(iter->second);
			name.resize(name_width-1, ' ');
			name += " ";
			stringstream  s;
			s << *cmd;
			/*int max_len = 64;*/
			auto cmd_str = s.str();
			if (cmd_str.length() > content_width) {
				cmd_str = cmd_str.substr(0, 16) + "..." + cmd_str.substr(cmd_str.length() - content_width + 3 + 16);
				cout << name << cmd_str;
			}
			else
			{
				cout << name << cmd_str << endl;
			}
			/*cout << s.str();*/
		}
		return 0;
	}
	vector<string> names;
	for (int i = 1; i <= top; i++) {
		auto s = luaL_checkstring(L, i);
		names.push_back(s);
	}

	for (auto iter = names.begin(); iter != names.end(); iter++) {
		auto name(*iter);
		name.resize(16, ' ');
		if (processes[*iter] == nullptr) {
			cout << name << 'null' << endl;
		}
		else {
			cout << name << processes[*iter] << endl;
		}
	}
	return 0;

}


static int run(lua_State *L) {
	auto s = luaL_checkstring(L, 1);
	string name(s);
	if (processes[name] == nullptr) {
		lua_pushboolean(L, 0);
		return 0;
	}

	auto process = processes[name];
	process->Run();
	lua_pushboolean(L, 1);
	return 1;

}

static int kill(lua_State *L) {
	auto s = luaL_checkstring(L, 1);
	string name(s);
	if (processes[name] == nullptr) {
		lua_pushboolean(L, 0);
		return 0;
	}

	auto process = processes[name];
	process->Kill();
	lua_pushboolean(L, 1);
	return 1;

}

void script::init()
{
	L = luaL_newstate();  /* create state */
	if (L == NULL) {
		return;
	}
	luaL_checkversion(L);  /* check that interpreter has correct version */
	luaL_openlibs(L);  /* open standard libraries */
	//print_version();

	static const struct luaL_Reg funcs[] = {
	{"add", add},
	{"ls", ls},
	{"run", run},
	{"kill", kill},
	{NULL, NULL},
	};

	lua_getglobal(L, "ps");
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_newtable(L);
	}
	luaL_setfuncs(L, funcs, 0);
	lua_setglobal(L, "ps");

	const char* info = "Process console";
	//lua_writestring(LUA_COPYRIGHT, strlen(LUA_COPYRIGHT));
	lua_writestring(info, strlen(info));
	lua_writeline();
	luaL_dofile(L, "init.lua");
}

//void script::run_script()
//{
//	
//}

void script::run_repl()
{
	doREPL(L);  /* do read-eval-print loop */
	lua_close(L);

}
