
test:condition.o mutex.o thread.o threadpool.o test.o 
	g++ -g -o test condition.o mutex.o thread.o threadpool.o test.o -lpthread

condition.o:./condition.cpp
	g++ -c ./condition.cpp

mutex.o:./mutex.cpp
	g++ -c ./mutex.cpp

thread.o:./thread.cpp
	g++ -c ./thread.cpp

threadpool.o:./threadpool.cpp
	g++ -c ./threadpool.cpp

test.o:test.cpp 
	g++ -c test.cpp
	
clean:
	rm test test.o mutex.o condition.o threadpool.o