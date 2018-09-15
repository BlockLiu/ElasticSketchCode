#ifdef _BIND_THREAD_WITH_CPU_
#define __USE_GNU  
#include <sched.h>  
#include <sys/types.h>  
#include <sys/sysinfo.h> 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string> 
#include <pthread.h>
#include <random>
#include <unistd.h>

#include "../CPU/elastic/ElasticSketch.h"
#include "setup.h"

using namespace std;


#define BUFFER_MAX_PACKET_NUM 100000		
uint8_t* packet_buffer[THREAD_NUM][BUFFER_MAX_PACKET_NUM];
volatile int cur_packet_num_pull[THREAD_NUM];	
volatile int cur_packet_num_push[THREAD_NUM];	
volatile int insert_finish[THREAD_NUM];
int begin_insert[THREAD_NUM];

#define MAIN_THREAD_BUFFER_SIZE 1000
uint8_t* main_buffer[THREAD_NUM][BUFFER_MAX_PACKET_NUM]; 
int main_buffer_cnt[THREAD_NUM];



void* ElasticRecord(void* args)
{
	int buffer_no = *((int*)args);	
	int buffer_index;
#ifdef _BIND_THREAD_WITH_CPU_
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(buffer_no + 1, &mask);
	if(sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1){
		printf("Warning: failed to bind %d-th thread to %d-th CPU core\n", buffer_no, buffer_no);
	}
#endif

/* init a elastic sketch */
	ElasticSketch<LS_BUCKET_NUM_4, TOTAL_MEM_IN_BYTES> *es;
	es = new ElasticSketch<LS_BUCKET_NUM_4, TOTAL_MEM_IN_BYTES>();
	begin_insert[buffer_no] = 1;
	printf("%d-th thread (tid:%llu) is ready for insertion\n", buffer_no, (long long unsigned int)pthread_self());

	timespec time1, time2;
	long long resns, tot_resns = 0;

/* check the mutex */
	int insertion_cnt = 0;
	uint8_t key[13];
	while(1)
	{
		int val_pull = cur_packet_num_pull[buffer_no];
		int val_push = cur_packet_num_push[buffer_no];
		if(val_push != val_pull){	
			while(val_push != val_pull)
			{
				buffer_index = val_pull % BUFFER_MAX_PACKET_NUM;
				es->insert(packet_buffer[buffer_no][buffer_index]);
				val_pull++;
			}
			insertion_cnt += val_push - cur_packet_num_pull[buffer_no];
			cur_packet_num_pull[buffer_no] = val_pull;
		}
		else{	
			if(insert_finish[buffer_no])
			{
				delete es;
				printf("%d-th thread (tid:%llu) finish %d insertions\n", buffer_no, (long long unsigned int)pthread_self(), insertion_cnt);
				return NULL;
			}
		}							
	}
}


int main(int argc, char *argv[])
{
	ReadInTraces("../../data/");
#ifdef _BIND_THREAD_WITH_CPU_
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	if(sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1){
		printf("Warning: failed to bind main thread to 0-th CPU core\n");
	}
#endif

	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		timespec time1, time2;
		long long resns, tot_resns = 0;
		int packet_cnt = (int)traces[datafileCnt - 1].size();


			uint8_t **keys = new uint8_t*[packet_cnt];
			for(int i = 0; i < packet_cnt; ++i)
			{
				keys[i] = new uint8_t[13];
				memcpy(keys[i], traces[datafileCnt - 1][i].key, 13);
			}


		printf("***************************************************************\n");
		for(int tt = 0; tt < test_cycles; ++tt)
		{
		/* initial */
			std::random_device rd;
			BOBHash32 *bobhash=  new BOBHash32(rd() % MAX_PRIME32);
			memset(packet_buffer, 0, sizeof(packet_buffer));
			memset(main_buffer, 0, sizeof(main_buffer));
			memset(main_buffer_cnt, 0, sizeof(main_buffer_cnt));
			memset(begin_insert, 0, sizeof(begin_insert));
			for(int i = 0; i < THREAD_NUM; ++i)
			{
				cur_packet_num_pull[i] = 0;
				cur_packet_num_push[i] = 0;
				insert_finish[i] = 0;
			}


		/* create child threads */
			pthread_t tids[THREAD_NUM];
			int buffer_no[THREAD_NUM];
			for(int i = 0; i < THREAD_NUM; ++i)
			{
				buffer_no[i] = i;
				int ret = pthread_create(&tids[i], NULL, ElasticRecord, (void*)&buffer_no[i]);
				if(ret != 0)
					printf("%d-th thread create error: error_code=%d\n", i, ret);
			}


		/* check if all thread are ready */
			usleep(1000000);		// wait 2 seconds
			while(1){
				int tmpRes = 0;
				for(int i = 0; i < THREAD_NUM; ++i)
					tmpRes += begin_insert[i];
				if(tmpRes == THREAD_NUM)
					break;
			}


		/* open a file, and read in */
			printf("Begin insertion.\n");
			fflush(stdout);
			clock_gettime(CLOCK_MONOTONIC, &time1);
			for(int i = 0; i < packet_cnt; ++i)
			{
				int thread_no = (*(uint32_t*)keys[i]) % THREAD_NUM;
				main_buffer[thread_no][main_buffer_cnt[thread_no]++] = keys[i];

				if(main_buffer_cnt[thread_no] == MAIN_THREAD_BUFFER_SIZE)
				{
					while(1)
					{
						if((cur_packet_num_push[thread_no] - cur_packet_num_pull[thread_no] + MAIN_THREAD_BUFFER_SIZE) <= BUFFER_MAX_PACKET_NUM)
						{
							int prev_index = cur_packet_num_push[thread_no] % BUFFER_MAX_PACKET_NUM;
							int last_index = (cur_packet_num_push[thread_no] + MAIN_THREAD_BUFFER_SIZE - 1) % BUFFER_MAX_PACKET_NUM;

							if(last_index > prev_index){
								memcpy(&packet_buffer[thread_no][prev_index], main_buffer[thread_no], MAIN_THREAD_BUFFER_SIZE * sizeof(uint8_t*));
							}else{
								int latter_len = BUFFER_MAX_PACKET_NUM - prev_index;
								int former_len = MAIN_THREAD_BUFFER_SIZE - latter_len;
								memcpy(&packet_buffer[thread_no][prev_index], main_buffer[thread_no], latter_len * sizeof(uint8_t*));
								memcpy(&packet_buffer[thread_no][0], &main_buffer[thread_no][latter_len], former_len * sizeof(uint8_t*));
							}

							cur_packet_num_push[thread_no] += MAIN_THREAD_BUFFER_SIZE;
							main_buffer_cnt[thread_no] = 0;
							break;
						}
					}
				}
			}
			clock_gettime(CLOCK_MONOTONIC, &time2);
			for(int i = 0; i < THREAD_NUM; ++i)
				insert_finish[i] = 1;
			delete bobhash;

		/* wait child thread finishing */
			void* status;
			for(int i = 0; i < THREAD_NUM; ++i)
				pthread_join(tids[i], &status);
			printf("%d.dat insertion finished!\n\n", datafileCnt - 1);
			resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
			tot_resns += resns;
		}
		double th_ls_4 = (double)1000.0 * test_cycles * packet_cnt / tot_resns;
		printf("CAIDA_%d: Elastic sketch inserting throughtput: %.6lf Mips\n", datafileCnt - 1, th_ls_4);

	}
	return 0;
}
