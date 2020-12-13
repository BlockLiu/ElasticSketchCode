CFLAGS= -std=c++17 -O2
CXX ?= g++
CC ?= gcc
OBJS = dataManager.o

all: $(OBJS) objmove

dataManager.o: dataManager.cpp dataManager.h
	$(CXX) $(CFLAGS) -c dataManager.cpp

objmove:
	cp $(OBJS) ../	

clean:
	rm -rf $(OBJS)