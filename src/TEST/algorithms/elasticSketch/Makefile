CFLAGS= -std=c++17 -O2
SSEFLAGS = -msse2 -mssse3 -msse4.1 -msse4.2 -mavx -mavx2 -march=native
CXX ?= g++
CC ?= gcc
OBJS = LightPart.o HeavyPart.o ElasticSketch.o

all: $(OBJS) objmove

LightPart.o: LightPart.cpp LightPart.h
	$(CXX) $(CFLAGS) -c LightPart.cpp

HeavyPart.o: HeavyPart.cpp HeavyPart.h
	$(CXX) $(CFLAGS) -c HeavyPart.cpp $(SSEFLAGS)

ElasticSketch.o: ElasticSketch.cpp ElasticSketch.h
	$(CXX) $(CFLAGS) -c ElasticSketch.cpp

objmove:
	cp $(OBJS) ../../	

clean:
	rm -rf $(OBJS)