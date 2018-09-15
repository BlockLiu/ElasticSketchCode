#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include "../UnivMon/UnivMon.h"
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


#define TOT_MEM_IN_BYTES (600 * 1024)
	UnivMon<4, TOT_MEM_IN_BYTES> *um = NULL;



	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		unordered_map<string, int> Real_Freq;
		um = new UnivMon<4, TOT_MEM_IN_BYTES>();

		int packet_cnt = (int)traces[datafileCnt - 1].size();
		for(int i = 0; i < packet_cnt; ++i)
		{
			um->insert((uint8_t*)(traces[datafileCnt - 1][i].key));
			

			string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			Real_Freq[str]++;
		}



#define HEAVY_HITTER_THRESHOLD(total_packet) (total_packet * 1 / 1000)
		vector< pair<uint32_t, int> > heavy_hitters;
		um->get_heavy_hitters(HEAVY_HITTER_THRESHOLD(packet_cnt), heavy_hitters);

		printf("%d.dat: ", datafileCnt - 1);
		printf("est_cardinality=%d\t", (int)um->get_cardinality());
		printf("entropy=%.3lf\n", um->get_entropy());

		printf("heavy hitters: <srcIP, count>, threshold=%d\n", HEAVY_HITTER_THRESHOLD(packet_cnt));
		for(int i = 0, j = 0; i < (int)heavy_hitters.size(); ++i)
		{
			uint32_t srcIP = heavy_hitters[i].first;
			printf("<%.8x, %d>", srcIP, heavy_hitters[i].second);
			if(++j % 5 == 0)
				printf("\n");
			else printf("\t");
		}
		printf("\n");


		delete um;
		Real_Freq.clear();
	}
}	