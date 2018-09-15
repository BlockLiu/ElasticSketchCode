GCC = g++
CFLAGS = -O2 -std=c++14
SSEFLAGS = -msse2 -mssse3 -msse4.1 -msse4.2 -mavx -march=native
FILES = elastic.out cmheap.out cmsketch.out cusketch.out countheap.out countsketch.out \
		linearcounting.out mrac.out hashpipe.out sieving.out spacesaving.out univmon.out \
		css.out flowradar.out reversiblesketch.out

all: $(FILES) 

elastic.out: elastic.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o elastic.out elastic.cpp

cmheap.out: cmheap.cpp
	$(GCC) $(CFLAGS) -o cmheap.out cmheap.cpp

countheap.out: countheap.cpp
	$(GCC) $(CFLAGS) -o countheap.out countheap.cpp

cmsketch.out: cmsketch.cpp
	$(GCC) $(CFLAGS) -o cmsketch.out cmsketch.cpp

cusketch.out: cusketch.cpp
	$(GCC) $(CFLAGS) -o cusketch.out cusketch.cpp

countsketch.out: countsketch.cpp
	$(GCC) $(CFLAGS) -o countsketch.out countsketch.cpp

hashpipe.out: hashpipe.cpp
	$(GCC) $(CFLAGS) -o hashpipe.out hashpipe.cpp

linearcounting.out: linearcounting.cpp
	$(GCC) $(CFLAGS) -o linearcounting.out linearcounting.cpp

mrac.out: mrac.cpp
	$(GCC) $(CFLAGS) -o mrac.out mrac.cpp

sieving.out:
	$(GCC) $(CFLAGS) -o sieving.out sieving.cpp

spacesaving.out:
	$(GCC) $(CFLAGS) -o spacesaving.out spacesaving.cpp

univmon.out:
	$(GCC) $(CFLAGS) -o univmon.out univmon.cpp

css.out:
	$(GCC) $(CFLAGS) -o css.out css.cpp

flowradar.out:
	$(GCC) $(CFLAGS) -o flowradar.out flowradar.cpp	

reversiblesketch.out: common.o os_galoisField.o os_mangler.o
	$(GCC) $(CFLAGS) -o reversiblesketch.out reversiblesketch.cpp common.o os_galoisField.o os_mangler.o

common.o:
	$(GCC) $(CFLAGS) -c ../common/common.cc

os_galoisField.o:
	$(GCC) $(CFLAGS) -c ../common/os_galoisField.cc

os_mangler.o:
	$(GCC) $(CFLAGS) -c ../common/os_mangler.cc


clean:
	rm $(all) -f *~ *.o *.out
