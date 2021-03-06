#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include "../MRAC/MRAC.h"
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
	MRAC<4, TOT_MEM_IN_BYTES> *mrac = NULL;



	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		unordered_map<string, int> Real_Freq;
		mrac = new MRAC<4, TOT_MEM_IN_BYTES>();

		int packet_cnt = (int)traces[datafileCnt - 1].size();
		for(int i = 0; i < packet_cnt; ++i)
		{
			mrac->insert((uint8_t*)(traces[datafileCnt - 1][i].key));
		

			string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			Real_Freq[str]++;
		}


		vector<double> dist;
		mrac->get_distribution(dist);


		printf("%d.dat: ", datafileCnt - 1);
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


		delete mrac;
		Real_Freq.clear();
	}
}	