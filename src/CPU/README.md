# CPU CODE

For each measurement task, besides Elastic sketch, we have implemented many other algorithms:
- `flow size estimation`: CM sketch, CU sketch, Count sketch.
- `heavy hitter detection`: SpaceSaving, Count/CM sketch with a min-heap (CountHeap/CMHeap), UnivMon, Hashpipe. 
- `heavy change detection`: Revisible sketch, FlowRadar, UnivMon.
- `flow size distribution`: MRAC
- `entropy`: UnivMon, Sieving.
- `cardinality`: Linear counting, UnivMon.

## How to make and test
First `make` in `demo`, and then you can test the above algorithms in `demo`.
- `./elastic.out`: it shows you the ARE of flow size estimation, the estimated heavy hitters, flow size distribution, entropy and cardinality. If you want to do heavy change detection, you can build two Elastic sketch and find heavy changers among the two groups of heavy hitters.
- `./cmheap.out`: it shows you the estimated heavy hitters.
- `./cmsketch.out`: it shows you the ARE of the flow size estimation.
- `./countheap.out`: it shows you the estimated heavy hitters.
- `./countsketch.out`: it shows you the ARE of the flow size estimation.
- `./cusketch.out`: it shows you the ARE of the flow size estimation.
- `./hashpipe.out`: it shows you the estimated heavy hitters.
- `./linearcounting.out`: it shows you the cardinality.
- `./mrac.out`: it shows you the flow size distribution.
- `./sieving.out`: it shows you the entropy.
- `./spacesaving.out`: it shows you the estimated heavy hitters.
- `./univmon.out`: it shows you the entropy, cardinality and the estimated heavy hitters.

## Notice
We recommend you using g++5 or higher version to compile the CPU code.
To compile with g++4 (such as g++ 4.9.2), please use the alternate version of 'std::align' (https://stackoverflow.com/questions/27064791/stdalign-not-supported-by-g4-9). Do not use 'std::align' as the name of the alternate align function, because we use 'using namespace std' in some files. So, if we don't change the function name, there will be a confliction when 'align' is included in 'std' namespace. 
You can use 'alternative_align' or others. And when you call the align function, please remember using the new function name.







