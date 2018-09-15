#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <errno.h>
#include "util.h"
#include "tuple.h"
#include "ringbuffer.h"

// The number of ringbuffer
// **Must** be (# pmd threads + 1)
#define MAX_RINGBUFFER_NUM 9

static inline char* ip2a(uint32_t ip, char* addr) {
    sprintf(addr, "%d.%d.%d.%d", (ip & 0xff), ((ip >> 8) & 0xff), ((ip >> 16) &
            0xff), ((ip >> 24) & 0xff));
    return addr;
}

void print_tuple(FILE* f, tuple_t* t) {
    char ip1[30], ip2[30];

    fprintf(f, "%s(%u) <-> %s(%u) %u %ld\n",
            ip2a(t->key.src_ip, ip1), t->key.src_port,
            ip2a(t->key.dst_ip, ip2), t->key.dst_port,
            t->key.proto, t->size
            );
}

int counter = 0;

void handler(int sig) {
	printf("%d\n", counter);
	counter = 0;
	alarm(1);
}

int main(int argc, char *argv[]) {
    tuple_t t;
    long long tot_cnt = 0;
    HC_TYPE * hc = new HC_TYPE();
    int turn = 0;

    LOG_MSG("Initialize the ringbuffer.\n");

	ringbuffer_t * rbs[MAX_RINGBUFFER_NUM];

	for (int i = 0; i < MAX_RINGBUFFER_NUM; ++i) {
		char name[30];
		sprintf(name, "/rb_%d", i);
		rbs[i] = connect_ringbuffer_shm(name, sizeof(tuple_t));
		printf("%x\n", rbs[i]);
	}
 
	printf("connected.\n");	fflush(stdout);

	int idx = 0;

    // print number of pkts received per 5 sec
	signal(SIGALRM, handler);
	alarm(5);

	while (1) {
        if (t.flag == TUPLE_TERM) {
            break;
        } 
        else {
		    while (read_ringbuffer(rbs[(idx) % MAX_RINGBUFFER_NUM], &t) < 0) {
                idx = (idx + 1) % MAX_RINGBUFFER_NUM;
            }
            counter++;
            // Insert to sketch here
        }
    } 

	return 0;
}
