CFLAGS= -std=c++17 -O2
SIMDFLAGS = -msse2 -mssse3 -msse4.1 -msse4.2 -mavx -mavx2 -march=native
CXX ?= g++
CC ?= gcc
OBJS = dataManager.o algorithms.o task.o \
		LightPart.o HeavyPart.o ElasticSketch.o BOBHash32.o \
		task_flowSize.o task_throughput.o \
		cmsketch.o

SUBDIRS = data algorithms tasks

all: subdirs test

test: main.cpp $(OBJS)
	$(CXX) -o test main.cpp $(OBJS) $(CFLAGS) $(SIMDFLAGS)

RECURSIVE_MAKE = @for subdir in $(SUBDIRS); \
		do \
		echo "making in $$subdir"; \
		( cd $$subdir && $(MAKE) all -f Makefile) || exit 1; \
		done

RECURSIVE_CLEAN = @for subdir in $(SUBDIRS); \
		do \
		echo "cleaning in $$subdir"; \
		( cd $$subdir && $(MAKE) clean -f Makefile) || exit 1; \
		done

subdirs:
		$(RECURSIVE_MAKE)

dclean:
		$(RECURSIVE_CLEAN)

clean: dclean
	rm -rf $(OBJS) test
