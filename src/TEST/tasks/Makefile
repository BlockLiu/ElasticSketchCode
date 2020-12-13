CFLAGS= -std=c++17 -O2
CXX ?= g++
CC ?= gcc
OBJS = task.o task_flowSize.o task_throughput.o

all: $(OBJS) objmove

task.o: task.cpp task.h
	$(CXX) $(CFLAGS) -c task.cpp 

task_flowSize.o: task_flowSize.cpp task_flowSize.h
	$(CXX) $(CFLAGS) -c task_flowSize.cpp 

task_throughput.o: task_throughput.cpp task_throughput.h
	$(CXX) $(CFLAGS) -c task_throughput.cpp 

objmove:
	cp $(OBJS) ../

clean:
	rm -rf $(OBJS)