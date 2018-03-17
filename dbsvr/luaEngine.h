#ifndef LUAENGINE_H
#define LUAENGINE_H

#include "../common/common.h"
#include "../common/lua_tinker.h"

class MsgPacket;

class LuaEngine
{
public:
	lua_State* L;

public:
	LuaEngine();
	~LuaEngine();
	
	bool Init();

	void ProcQuery(uint32_t accountId, uint32_t playerId, MsgPacket* pack);
	void ProcExec(uint32_t accountId, uint32_t  playerId, MsgPacket* pack);
private:
	bool LoadScripts();
	bool LoadLuaFiles();

	void RegWorldPacketClass();
	void RegDatabase();
	void RegStdString();

	void InitLuaFuncHandle();
private:
	uint32_t   m_fnProcQuery;
	uint32_t   m_fnProcExec;
};

#endif