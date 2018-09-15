#ifndef _hashpipe_H
#define _hashpipe_H

// #include <bits/stdc++.h>
#include <map>
#include "../common/BOBHash32.h"
#define d 6
using namespace std;

class HashPipe
{
    private:
        string key[d][3000005];
        int M,val[d][3000005],i,j;
        BOBHash32 * BOB[d+5];
    public:
        HashPipe (int MEM) {
//            M=MEM/d;
            M = MEM / d / 8;
            for (i=0; i<d; i++)
                for (j=0; j<M; j++)
                    val[i][j]=0;
            for (i=0; i<d; i++)
                BOB[i]=new BOBHash32(1000+i);
        }

        void work(string x,int y)
        {
            for (int i=1; i<d; i++)
            {
                int L=BOB[i]->run(x.c_str(),x.size()) % M;
                if (key[i][L]==x) {val[i][L]+=y; return;}
                if (!val[i][L]) {key[i][L]=x; val[i][L]=y; return;}
                if (val[i][L]<y) {swap(x,key[i][L]); swap(y,val[i][L]);}
            }
        }

        void insert(uint8_t * _x)
        {
            string x = string((const char *)_x, 4);
            int L=BOB[0]->run(x.c_str(),x.size()) % M;
            if (key[0][L]==x) val[0][L]++; else
            if (!val[0][L]) key[0][L]=x,val[0][L]=1; else
            {
                work(key[0][L],val[0][L]);
                key[0][L]=x; val[0][L]=1;
            }
        }
        void get_heavy_hitters(int threshold, vector <pair<uint32_t, int> > &v)
        {
            map<string ,int> mp; mp.clear();
            for (i=0; i<d; i++) for (j=0; j<M; j++) mp[key[i][j]]+=val[i][j];
            for (map <string,int> :: iterator sit = mp.begin(); sit!=mp.end(); sit++)
              if (sit->second >= threshold)
              {
                  v.push_back(make_pair(*(uint32_t *)(sit->first.c_str)(),sit->second));
              }
        }
};
#endif
