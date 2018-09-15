#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include "../Sieving/entropy_ams.h"
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
#define G 8
#define Z (TOT_MEM_IN_BYTES / G / 64)
	algorithms::entropy::EntropyAMS<4, G, Z> *seiving = NULL;



	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		unordered_map<string, int> Real_Freq;
		seiving = new algorithms::entropy::EntropyAMS<4, G, Z>();

		int packet_cnt = (int)traces[datafileCnt - 1].size();
		for(int i = 0; i < packet_cnt; ++i)
		{
			seiving->insert((uint8_t*)(traces[datafileCnt - 1][i].key));

			string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			Real_Freq[str]++;
		}




		printf("%d.dat: ", datafileCnt - 1);
		printf("entropy=%.3lf\n", seiving->get_entropy());



		delete seiving;
		Real_Freq.clear();
	}
}	