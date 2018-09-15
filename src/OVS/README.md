# Open vSwitch Integration

## Environment
We have tested on
- Open vSwitch 2.5.1
- DPDK 2.2
- Linux kernel version 3.16

## How to Install
- Suppose Open vSwitch source code is in $OVS_DIR
- Copy ovs/*.h to $OVS_DIR/lib
- Patch ovs/dpif-netdev.diff to $OVS_DIR/lib/dpif-netdev.c
- Re-compile Open vSwitch and link it with DPDK

## How to Start Measurement
- Uncomment SIGCOMM_MEASUREMENT macro in $OVS_DIR/lib/ringbuffer.h (line 21)
- Re-compile Open vSwitch
- Start Open vSwitch with DPDK, set the number of pmd thread = $d$ ($d$=1,2,4,8,...)
- Open user_reader/user_reader.cpp
  - Set #MAX_RINGBUFFER_NUM to $d + 1$
  - `$ make`
  - run `./user_reader`