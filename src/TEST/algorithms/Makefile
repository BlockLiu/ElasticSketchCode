CFLAGS= -std=c++17 -O2
CXX ?= g++
CC ?= gcc
OBJS = algorithms.o BOBHash32.o
SUBDIRS = elasticSketch cmSketch

all: subdirs $(OBJS) objmove

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

algorithms.o: algorithms.cpp algorithms.h
	$(CXX) $(CFLAGS) -c algorithms.cpp 

BOBHash32.o: BOBHash32.cpp BOBHash32.h
	$(CXX) $(CFLAGS) -c BOBHash32.cpp

objmove:
	cp $(OBJS) ../

subdirs:
		$(RECURSIVE_MAKE)

dclean:
		$(RECURSIVE_CLEAN)

clean: dclean
	rm -rf $(OBJS)