#include "lua_tinker.h"

int main(int argc, char const *argv[])
{
	lua_State* L = lua_open();
	luaL_openlibs(L);
   	lua_tinker::dofile(L, "/home/source2222/luaScripts/dbscripts/test.lua");
    return 0;
}