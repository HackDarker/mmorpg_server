CXX=g++

FLAGS=-c -Wall -O2 -g

LIBS=../3rd/liblua.a
	
SRCDIRS=. ../common ../networklib ./threadlib

SRCS=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
OBJS=$(SRCS:.cpp=.o)
PROG=./dbsvr

$(PROG): $(OBJS)
	CXX -g $(OBJS) -o $(PROG) $(LIBS) -lpthread -ldl -lhiredis
.cpp.o:
	CXX $(FLAGS) -fPIC $< -o $@

clean:
	rm -rf $(OBJS) $(PROG) $(PROG).core