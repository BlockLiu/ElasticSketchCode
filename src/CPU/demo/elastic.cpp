#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include "../elastic/ElasticSketch.h"
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
	ReadInTraces("../../../data/");


#define HEAVY_MEM (150 * 1024)
#define BUCKET_NUM (HEAVY_MEM / 64)
#define TOT_MEM_IN_BYTES (600 * 1024)
	ElasticSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> *elastic = NULL;



	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		unordered_map<string, int> Real_Freq;
		elastic = new ElasticSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();

		int packet_cnt = (int)traces[datafileCnt - 1].size();
		for(int i = 0; i < packet_cnt; ++i)
		{
			elastic->insert((uint8_t*)(traces[datafileCnt - 1][i].key));
			// elastic->quick_insert((uint8_t*)(traces[datafileCnt - 1][i].key));

			string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			Real_Freq[str]++;
		}

		double ARE = 0;
		for(unordered_map<string, int>::iterator it = Real_Freq.begin(); it != Real_Freq.end(); ++it)
		{
			uint8_t key[4];
			memcpy(key, (it->first).c_str(), 4);
			int est_val = elastic->query(key);
			int dist = std::abs(it->second - est_val);
			ARE += dist * 1.0 / (it->second);
		}
		ARE /= (int)Real_Freq.size();

		vector<double> dist;
		elastic->get_distribution(dist);

#define HEAVY_HITTER_THRESHOLD(total_packet) (total_packet * 1 / 1000)
		vector< pair<string, int> > heavy_hitters;
		elastic->get_heavy_hitters(HEAVY_HITTER_THRESHOLD(packet_cnt), heavy_hitters);

		printf("%d.dat: ARE=%.3lf\t", datafileCnt - 1, ARE);
		printf("est_cardinality=%d\t", elastic->get_cardinality());
		printf("entropy=%.3lf\n", elastic->get_entropy());
		printf("flow size distribution: <flow size, count>\n");
		for(int i = 0, j = 0; i < (int)dist.size(); ++i)
			if(dist[i] > 0)
			{
				printf("<%d, %d>", i, (int)dist[i]);
				if(++j % 10 == 0)
					printf("\n");
				else printf("\t");
			}
		printf("\n");

		printf("heavy hitters: <srcIP, count>, threshold=%d\n", HEAVY_HITTER_THRESHOLD(packet_cnt));
		for(int i = 0, j = 0; i < (int)heavy_hitters.size(); ++i)
		{
			uint32_t srcIP;
			memcpy(&srcIP, heavy_hitters[i].first.c_str(), 4);
			printf("<%.8x, %d>", srcIP, heavy_hitters[i].second);
			if(++j % 5 == 0)
				printf("\n");
			else printf("\t");
		}
		printf("\n");


		delete elastic;
		Real_Freq.clear();
	}
}	