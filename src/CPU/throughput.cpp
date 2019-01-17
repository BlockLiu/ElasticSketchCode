#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include "./elastic/ElasticSketch.h"
using namespace std;

#define START_FILE_NO 1
#define END_FILE_NO 10


struct FIVE_TUPLE{	char key[13];	};
typedef vector<FIVE_TUPLE> TRACE;
TRACE traces[END_FILE_NO - START_FILE_NO + 1];

void ReadInTraces(const char *trace_prefix)
{
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		char datafileName[100];
		sprintf(datafileName, "%s%d.dat", trace_prefix, datafileCnt - 1);
		FILE *fin = fopen(datafileName, "rb");

		FIVE_TUPLE tmp_five_tuple;
		traces[datafileCnt - 1].clear();
		while(fread(&tmp_five_tuple, 1, 13, fin) == 13)
		{
			traces[datafileCnt - 1].push_back(tmp_five_tuple);
		}
		fclose(fin);

		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt - 1].size());
	}
	printf("\n");
}

int main()
{
	ReadInTraces("./../../data/");


#define HEAVY_MEM (150 * 1024)
#define BUCKET_NUM (HEAVY_MEM / 64)
#define TOT_MEM_IN_BYTES (600 * 1024)
	ElasticSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> *elastic = NULL;



	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		elastic = NULL;

    timespec time1, time2;
		long long resns;
		int packet_cnt = (int)traces[datafileCnt - 1].size();
    
    uint8_t **keys = new uint8_t*[(int)traces[datafileCnt - 1].size()];
		for(int i = 0; i < (int)traces[datafileCnt - 1].size(); ++i)
		{
			keys[i] = new uint8_t[13];
			memcpy(keys[i], traces[datafileCnt - 1][i].key, 13);
		}
    
    clock_gettime(CLOCK_MONOTONIC, &time1);
    for(int t = 0; t < test_cycles; ++t)
    {
      elastic = new ElasticSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();
      for(int i = 0; i < packet_cnt; ++i)
        elastic->insert(keys[i]);
      delete elastic;
    }
    clock_gettime(CLOCK_MONOTONIC, &time2);
    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		double th = (double)1000.0 * test_cycles * packet_cnt / resns;
    
    /* free memory */
		for(int i = 0; i < (int)traces[datafileCnt - 1].size(); ++i)
			delete[] keys[i];
		delete[] keys;
    
    printf("throughput is %lf mbps\n", th);
	}
}
