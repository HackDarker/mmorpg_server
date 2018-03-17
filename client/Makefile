
server:condition.o mutex.o thread.o threadpool.o buffer.o netPacket.o socket.o socketMgr.o  util_time.o  server_new.o 
	g++ -g -o main  condition.o mutex.o thread.o threadpool.o buffer.o netPacket.o socket.o socketMgr.o util_time.o  server_new.o -lpthread

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

netPacket.o:../networklib/netPacket.cpp
	g++ -c ../networklib/netPacket.cpp	

socket.o:../networklib/socket.cpp
	g++ -c ../networklib/socket.cpp	

socketMgr.o:../networklib/socketMgr.cpp
	g++ -c ../networklib/socketMgr.cpp

util_time.o:../common/util_time.cpp
	g++ -c ../common/util_time.cpp	

server_new.o:server_new.cpp 
	g++ -c server_new.cpp
	
clean:
	rm ./*.o