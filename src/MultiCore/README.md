# Multi-core implementation

We divide an Elastic sketch into several part, and each part is allocated to a thread.
There is a main thread to control children threads.
Each thread (including main thread and children threads) is bind to a single core.

For example, consider the total memory allocated for Elastic sketch is 600 KBytes, 1/4 of which is for the heavy part, and we use 4 children thread.
Then, each thread has a 150 KBytes Elastic sketch, the heavy part of which is 37.5 KBytes.

# Notice
You can change the parameters `THREAD_NUM` in `setup.h` to change the number of thread you want.
It's recommended that the number of threads is less than the logical core number in your CPU.

# How to make
- `make`

# How to test
- `./elastic_multicore.out`
