#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "configMgr.h"

extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
};

ConfigMgr::ConfigMgr()
{

}

ConfigMgr::~ConfigMgr()
{

}

bool ConfigMgr::LoadConfig(const char* filename)
{
	/*static const char * load_config = "\
		local result = {}\n\
		local function getenv(name) return assert(os.getenv(name), [[os.getenv() failed: ]] .. name) end\n\
		local sep = package.config:sub(1,1)\n\
		local current_path = [[.]]..sep\n\
		local function include(filename)\n\
			local last_path = current_path\n\
			local path, name = filename:match([[(.*]]..sep..[[)(.*)$]])\n\
			if path then\n\
				if path:sub(1,1) == sep then	-- root\n\
					current_path = path\n\
				else\n\
					current_path = current_path .. path\n\
				end\n\
			else\n\
				name = filename\n\
			end\n\
			local f = assert(io.open(current_path .. name))\n\
			local code = assert(f:read [[*a]])\n\
			code = string.gsub(code, [[%$([%w_%d]+)]], getenv)\n\
			f:close()\n\
			assert(load(code,[[@]]..filename,[[t]],result))()\n\
			current_path = last_path\n\
		end\n\
		setmetatable(result, { __index = { include = include } })\n\
		local config_name = ...\n\
		include(config_name)\n\
		setmetatable(result, nil)\n\
		return result\n\
	";

	struct lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	int err = luaL_loadbuffer(L, load_config, strlen(load_config), "=[sever config]");
	assert(err == 0);
	lua_pushstring(L, filename);

	err = lua_pcall(L, 1, 1, 0);
	if (err) {
		fprintf(stderr,"%s\n",lua_tostring(L,-1));
		lua_close(L);
		return false;
	}

	//set config key value
	{
		lua_pushnil(L);  
		while (lua_next(L, -2) != 0) {
			int keyt = lua_type(L, -2);
			if (keyt != LUA_TSTRING) {
				fprintf(stderr, "Invalid config table\n");
				exit(1);
			}
			const char * key = lua_tostring(L,-2);
			if (lua_type(L,-1) == LUA_TBOOLEAN) {
				int b = lua_toboolean(L,-1);
				m_confMap[key] = (b ? "true" : "false");
			} else {
				const char * value = lua_tostring(L,-1);
				if (value == NULL) {
					fprintf(stderr, "Invalid config table key = %s\n", key);
					exit(1);
				}
				m_confMap[key] = value;
			}
			lua_pop(L,1);
		}
		lua_pop(L,1);
	}

	lua_close(L); */

	return true;
}

bool ConfigMgr::GetBool(std::string key,int opt)
{
	// std::map<std::string, std::string>::iterator itr = m_confMap.find(key);
	// if(itr != m_confMap.end()){
	// 	return strcmp(itr->second.c_str(),"true") == 0;
	// }

	// return opt;
}

int ConfigMgr::GetInt(std::string key,int opt)
{
	// std::map<std::sting, std::string>::iterator itr = m_confMap.find(key);
	// if(itr != m_confMap.end()){
	// 	return strtol(itr->second.c_str(), NULL, 10);
	// }

	// return opt;
}

char* ConfigMgr::GetString(std::string,char* opt)
{
	// std::map<std::string, std::string>::iterator itr = m_confMap.find(key);
	// if(itr != m_confMap.end()){
	// 	return itr->second.c_str();
	// }

	// return opt;
}