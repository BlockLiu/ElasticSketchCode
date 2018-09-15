GCC = g++
CFLAGS = -O2 -std=c++14
SSEFLAGS = -msse2 -mssse3 -msse4.1 -msse4.2 -mavx -march=native
FILES = elastic_multicore.out

all: $(FILES)

elastic_multicore.out: multi_thread.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o elastic_multicore.out multi_thread.cpp -lpthread -lrt

clean:
	rm $(all) -f *~ *.o *.out
