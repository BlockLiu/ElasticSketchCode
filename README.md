# ElasticSketch

## Introduction
When network is undergoing problems such as congestion, scan attack, DDoS attack, etc., measurements are much more important than usual. In this case, traffic characteristics including available bandwidth, packet rate, and flow size distribution vary drastically, significantly degrading the performance of measurements. To address this issue, we propose the Elastic sketch. It is adaptive to currently traffic characteristics. Besides, it is generic to measurement tasks and platforms. We implement the Elastic sketch on six platforms: P4, FPGA, GPU, CPU, multi-core CPU, and OVS, to process six typical measurement tasks. Experimental results and theoretical analysis show that the Elastic sketch can adapt well to traffic characteristics. Compared to the state-of-the-art, the Elastic sketch achieves 44.6 ∼ 45.2 times faster speed and 2.0 ∼ 273.7 smaller error rate.

## About this repo
- `elastic_sketch_technical_report` can help you understand the paper better.
- `data`: traces for test, each 13 bytes in a trace is a (SrcIP:SrcPort, DstIP:DstPort, protocol)
- `src` contains codes impelemented on different platforms:
  - `CPU`: Elastic sketch and other algorithms implemented on CPU
  - `FPGA`: Elastic sketch implemented on FPGA
  - `GPU`: Elastic sketch implemented on GPU4
  - `MultiCore`: Elastic sketch using multi-core
  - `OVS`: Elastic sketch implemented on OVS
  - `P4`: Elastic sketch implemented on P4
- more details can be found above six folders.

## Requirements
- SIMD instructions are used in Elastic sketch to achieve higher speed, so the CPU must support AVX2 instruction set.
- g++
- cuda

## How to make
- `cd ./src; ./auto_make;` then you can find executable file in CPU, GPU and MultiCore directories (there are documents to tell you how to execute).
- FPGA, OVS and P4 implementation need specific platforms, you can try them on real device or virtual machine.


