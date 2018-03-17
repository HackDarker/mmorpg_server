#include "netPacket.h"
#include "lua_netPacket.h"

#define	MP_LUA_LIB_NAME "msgpack"
#define MSGPACKET_META_NAME "msgpacket_mate"
#define C_MSGPACKET_META_NAME "c_msgpacket_mate"
#define WORLDPACKET_PACKET_NAME "MsgPacket"
#define MSGPACKET_PACKET_NAME "msgpack_name"

static void lua_msgpack_error(lua_State *L, const char* fmt, ...) {
 	char text[128] = "[MsgPack error]: ";
 
 	va_list args;
 	va_start(args, fmt);
 	vsprintf(text + strlen(text), fmt, args);
 	va_end(args);
 
 	lua_pushstring(L, text);
 	lua_error(L);
}

static void lua_packet_free_basic(lua_State *L, MsgPacket* packet) {
	delete packet;
}

static void (*lua_packet_free_func)(lua_State *L, MsgPacket* packet) = lua_packet_free_basic;

/* --------------------------- lua packet buf op -------------------------- */
int lua_ispacket(lua_State *L, int idx) {
	if( lua_getmetatable(L, idx) == 0 ) {
		return 0;
	}

	lua_pushstring(L, "__name");
	lua_rawget(L, -2);

	char* name = (char*)lua_tostring(L, -1);

	if( !name || strcmp(name, C_MSGPACKET_META_NAME) ) {
		lua_pop(L, 1);
		return 0;
	}

	lua_pop(L, 2);

	return 1;
}

mp_buf* lua_tompbuf(lua_State *L, int idx) {
	MsgPacket* packet;
	if( lua_ispacket(L, idx) == 0 ) {
		return NULL;
	}

	MsgPacket** ud = (MsgPacket**)lua_touserdata(L,idx);
	if (!ud) return NULL;

	packet = *ud;
	if ( !packet ) return NULL;
	
	return packet->getMpBuf();
}

MsgPacket* lua_topacket(lua_State *L, int idx) {
	MsgPacket* packet;
	if( lua_ispacket(L, idx) == 0 ) {
		return NULL;
	}

	MsgPacket** ud = (MsgPacket**)lua_touserdata(L,idx);
	if (!ud) return NULL;

	packet = *ud;

	return packet;
}


#define lua_setpacketmetatable(L, idx) luaL_getmetatable(L, MSGPACKET_META_NAME); lua_setmetatable(L, idx - 1)
#define lua_setcpacketmetatable(L, idx) luaL_getmetatable(L, C_MSGPACKET_META_NAME); lua_setmetatable(L, idx - 1)

void lua_pushpacket(lua_State *L, MsgPacket* packet) {
	MsgPacket** ud = (MsgPacket**)lua_newuserdata(L, sizeof(MsgPacket*));
	*ud = packet;
	lua_setcpacketmetatable(L, -1);
}


static int lua_packet_free(lua_State *L) {
	MsgPacket* packet = lua_topacket(L,1);
	if (!packet) {
		lua_msgpack_error(L,"Packet free needs a packet.");
	}
	
	lua_packet_free_func(L, packet);

	return 0;
}

static void lua_reg_packet_meta(lua_State *L) {
	luaL_newmetatable(L, MSGPACKET_META_NAME);

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_packet_free);
	lua_rawset(L, -3);

	lua_pushstring(L, "__name");
	lua_pushstring(L, MSGPACKET_PACKET_NAME);
	lua_rawset(L, -3);

	lua_pop(L, 1);
}

static void lua_reg_cpacket_meta(lua_State *L) {
	luaL_newmetatable(L, C_MSGPACKET_META_NAME);

	lua_pushstring(L, "__name");
	lua_pushstring(L, C_MSGPACKET_META_NAME);
	lua_rawset(L, -3);

	lua_pop(L, 1);
}

static int lua_mp_buf_get_opcode(lua_State *L) {
	mp_buf* buf = lua_tompbuf(L,1);
	if (!buf) {
		lua_msgpack_error(L,"Buf get opcode needs a packet buf.");
	}

	lua_pushnumber(L, mp_buf_get_opcode(buf));

	return 1;
}

static int lua_mp_buf_set_opcode(lua_State *L) {
	uint16_t opcode;
	mp_buf* buf = lua_tompbuf(L,1);
	if (!buf) {
		lua_msgpack_error(L,"Buf set opcode needs a packet buf.");
	}

	opcode = lua_tonumber(L,2);
	mp_buf_set_opcode(buf, opcode);

	return 0;
}

static int lua_mp_buf_get_pbdata(lua_State* L){
	mp_buf* buf = lua_tompbuf(L,1);
	if (!buf) {
		lua_msgpack_error(L,"Buf get size needs a packet buf.");
	}

	lua_pushlightuserdata(L, mp_buf_get_content(buf));
	lua_pushnumber(L, mp_buf_get_size(buf));

	return 2;
}

static int lua_mp_buf_get_size(lua_State *L) {
	mp_buf* buf = lua_tompbuf(L,1);
	if (!buf) {
		lua_msgpack_error(L,"Buf get size needs a packet buf.");
	}

	lua_pushnumber(L, mp_buf_get_size(buf));

	return 1;
}

/* ---------------------------------------------------------------------------- */
static const struct luaL_reg thislib[] = {
	{"getOpcode", lua_mp_buf_get_opcode},
	{"setOpcode", lua_mp_buf_set_opcode},
	{"getContents",lua_mp_buf_get_pbdata},
	{"getSize", lua_mp_buf_get_size},
    {NULL, NULL}
};

int luaopen_msgpack(lua_State *L) {
    luaL_register(L, MP_LUA_LIB_NAME, thislib);
	lua_reg_packet_meta(L);
	lua_reg_cpacket_meta(L);

    return 1;
}