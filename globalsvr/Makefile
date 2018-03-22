
FLAGS=-c -Wall -O2 -g

LIBS=../3rd/liblua.a
	
SRCDIRS=. ../common ../threadlib ../networklib

SRCS=$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
OBJS=$(SRCS:.cpp=.o)
PROG=./global

$(PROG): $(OBJS)
	g++ -g $(OBJS) -o $(PROG) $(LIBS) -lpthread -ldl
.cpp.o:
	g++ $(FLAGS) -fPIC $< -o $@

clean:
	rm -rf $(OBJS) $(PROG) $(PROG).core
