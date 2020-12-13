CFLAGS= -std=c++17 -O2
CXX ?= g++
CC ?= gcc
OBJS = cmsketch.o

all: $(OBJS) objmove

cmsketch.o: cmsketch.cpp cmsketch.h
	$(CXX) $(CFLAGS) -c cmsketch.cpp
	
objmove:
	cp $(OBJS) ../../	

clean:
	rm -rf $(OBJS)