#include <dirent.h>
#include <sys/stat.h>
#include <functional>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <queue>
#include <list>
#include <algorithm>
#include "../networklib/worldPacket.h"
#include "luaEngine.h"

#define LUA_MSGPACK "msgpack"
LUALIB_API int luaopen_msgpack (lua_State *L);

extern "C"
{
	LUALIB_API void my_lua_openlibs(lua_State *L) {
		static const luaL_Reg lualibs[] = {
			{LUA_MSGPACK, luaopen_msgpack},
			{NULL, NULL}
		};

		const luaL_Reg *lib = lualibs;
		for (; lib->func; lib++) {
			lua_pushcfunction(L, lib->func);
			lua_pushstring(L, lib->name);
			lua_call(L, 1, 0);
		}
	}
};

static void str_tolower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(tolower));
}


struct MyFileGreater : public std::binary_function<std::string, std::string, bool>
{
	bool operator()(const std::string& left, const std::string& right) const
	{
		std::string x1(left);
		std::string x2(right);
		str_tolower(x1);
		str_tolower(x2);
		return (x1 > x2);
	}
};
static bool ScanDir(const std::string& strdir, std::list<std::string>& luaFiles)
{
	std::priority_queue<std::string, std::vector<std::string>, MyFileGreater > files;
	std::priority_queue<std::string, std::vector<std::string>, MyFileGreater > fpaths;
	std::string strtemp;
	char*  ext;

	struct dirent** nlist;
	int n = scandir(strdir.c_str(), &nlist, 0, 0);
	if(n <= 0 || !nlist)
		return false;

	struct stat dir;
	while(n--) {
		if(strcmp(nlist[n]->d_name, ".") == 0 || strcmp(nlist[n]->d_name, "..") == 0) {
			free(nlist[n]);
			continue;
		}

		strtemp = strdir;
		strtemp += "/";
		strtemp += nlist[n]->d_name;
		if(0 == stat(strtemp.c_str(), &dir) && S_ISDIR(dir.st_mode)) { // path
			fpaths.push(strtemp);
		} else {
			ext = strrchr(nlist[n]->d_name, '.');
			if(ext != NULL && (0 == strcasecmp(ext, ".lua") || 0 == strcasecmp(ext, ".luc"))) {
				files.push(strtemp);
			}
		}
		free(nlist[n]);
	}
	free(nlist);

	while(!files.empty()) {
		luaFiles.push_back(files.top());
		files.pop();
	}
	while(!fpaths.empty()) {
		ScanDir(fpaths.top(), luaFiles);
		fpaths.pop();
	}
	return true;
}

LuaEngine::LuaEngine()
{
	L = luaL_newstate();
}

LuaEngine::~LuaEngine()
{
	lua_close(L);
}

bool LuaEngine::Init()
{
	if (LoadScripts()){
		InitLuaFuncHandle();
		lua_checkstack(L, 800);
		return true;
	}
	return false;
}

bool LuaEngine::LoadLuaFiles()
{
	std::list<std::string> luaFiles;
	lua_tinker::table t = lua_tinker::get<lua_tinker::table>(L, "loadLuaFiles");

	const char* value;
	bool bFirst = true;
	while(t.can_get_next(bFirst)) {
		value = t.get_next_value<const char*>();
		std::string strDir("dbscripts/");
		strDir =  strDir + value;
		ScanDir(strDir, luaFiles);
	}
	for(std::list<std::string>::iterator itr = luaFiles.begin(); itr != luaFiles.end(); ++itr) {
		lua_tinker::dofile(L, (*itr).c_str());
	}
	return true;
}

bool LuaEngine::LoadScripts()
{	
	luaL_openlibs(L);
	my_lua_openlibs(L);

	lua_tinker::dofile(L, "/home/mmorpg_sever/luaScripts/dbscripts/test.lua");
	lua_tinker::dofile(L, "/home/mmorpg_sever/luaScripts/dbscripts/loader.lua");
	LoadLuaFiles();
	lua_tinker::dofile(L, "/home/mmorpg_sever/luaScripts/dbscripts/interface.lua");

	return true;
}

void LuaEngine::InitLuaFuncHandle()
{
	m_fnProcQuery = lua_tinker::make_function_hanlde(L, "c_procQuery");
	printf("InitLuaFuncHandle===============%u\n", m_fnProcQuery);
	assert(m_fnProcQuery != LUA_REFNIL);
	m_fnProcExec = lua_tinker::make_function_hanlde(L, "c_procExec");
	assert(m_fnProcExec != LUA_REFNIL);
}

void LuaEngine::ProcQuery(uint32_t accountId, uint32_t playerId, WorldPacket* pack)
{
	printf("222222222222222222222222222ProcQuery=====================%u\n",m_fnProcQuery);
	lua_tinker::call<void, uint32_t, uint32_t, WorldPacket*>(L, m_fnProcQuery, accountId, playerId, pack);
}

void LuaEngine::ProcExec(uint32_t accountId, uint32_t playerId, WorldPacket* pack)
{
	lua_tinker::call<void, uint32_t, uint32_t, WorldPacket*>(L, m_fnProcExec, accountId, playerId, pack);
}
