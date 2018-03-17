#ifndef LUANETPACKET_H
#define LUANETPACKET_H

extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
};

class MsgPacket;

void lua_pushpacket(lua_State *L, MsgPacket* packet);

#endif
