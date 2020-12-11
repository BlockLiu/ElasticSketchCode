CFLAGS=-Wall -std=c++17 -O2
CXX ?= g++
CC ?= gcc
OBJS = LightPart.o HeavyPart.o ElasticSketch.o

all: $(OBJS)

LightPart.o: LightPart.cpp LightPart.h
	$(CXX) $(CFLAGS) -c LightPart.cpp

HeavyPart.o: HeavyPart.cpp HeavyPart.h
	$(CXX) $(CFLAGS) -c HeavyPart.cpp

ElasticSketch.o: ElasticSketch.cpp ElasticSketch.h
	$(CXX) $(CFLAGS) -c ElasticSketch.cpp

clean:
	rm -rf $(OBJS)