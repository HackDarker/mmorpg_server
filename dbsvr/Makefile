
main:condition.o mutex.o thread.o threadpool.o buffer.o netPacket.o socket.o socketMgr.o lua_tinker.o util_time.o commonServer.o dbModule.o luaEngine.o luaProcMgr.o gameServer.o lua_netPacket.o pbc_lua.o main.o 
	g++ -g -o main condition.o mutex.o thread.o threadpool.o buffer.o netPacket.o socket.o socketMgr.o lua_tinker.o util_time.o commonServer.o dbModule.o luaEngine.o luaProcMgr.o gameServer.o lua_netPacket.o pbc_lua.o main.o  -lpthread -ldl -lpbc -L ../3rd/ -llua

condition.o:../threadlib/condition.cpp
	g++ -c ../threadlib/condition.cpp

mutex.o:../threadlib/mutex.cpp
	g++ -c ../threadlib/mutex.cpp

thread.o:../threadlib/thread.cpp
	g++ -c ../threadlib/thread.cpp

threadpool.o:../threadlib/threadpool.cpp
	g++ -c ../threadlib/threadpool.cpp

buffer.o:../networklib/buffer.cpp
	g++ -c ../networklib/buffer.cpp

lua_netPacket.o:../networklib/lua_netPacket.cpp
	g++ -c ../networklib/lua_netPacket.cpp

pbc_lua.o:../networklib/pbc_lua.c
	g++ -c ../networklib/pbc_lua.c

netPacket.o:../networklib/netPacket.cpp
	g++ -c ../networklib/netPacket.cpp	

socket.o:../networklib/socket.cpp
	g++ -c ../networklib/socket.cpp	

socketMgr.o:../networklib/socketMgr.cpp
	g++ -c ../networklib/socketMgr.cpp

lua_tinker.o:../common/lua_tinker.cpp
	g++ -c ../common/lua_tinker.cpp	

util_time.o:../common/util_time.cpp
	g++ -c ../common/util_time.cpp	

commonServer.o:./commonServer.cpp
	g++ -c ./commonServer.cpp

gameserver.o:./gameServer.cpp
	g++ -c ./gameServer.cpp

dbModule.o:./dbModule.cpp
	g++ -c ./dbModule.cpp

luaEngine.o:./luaEngine.cpp
	g++ -c ./luaEngine.cpp

luaProcMgr.o:./luaProcMgr.cpp
	g++ -c ./luaProcMgr.cpp

main.o:main.cpp 
	g++ -c main.cpp
	
clean:
	rm ./*.o