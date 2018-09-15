for ((i=100000; i<=1000000; i+=100000))  
do  
    nvcc elastic_gpu.cu -DBATCH_SIZE=$i -std=c++11 -Wno-deprecated-gpu-targets
    ./a.out
done 