# Open source of P4 implementation

The source code of Elastic sketch P4 impementation can be seen in `elastic.p4` file.

## Description
We have fully built a P4 prototype of the Elastic sketch on top of a baseline `switch.p4` and compiled on a programmable switch ASIC.

Both heavy part and light part of the hardware version are implemented in registers instead of match-action tables, because those parts require updating the entries directly from the data plane. 

We leverage the Stateful Algorithm and Logical Unit (Stateful ALU) in each stage to lookup and update the entries in register array. 

However, Stateful ALU has its resource limitation: each Stateful ALU can only update a pair of up to 32-bit registers while our hardware version of Elastic needs to access four fields in a bucket for an insertion.

To address this issue, we tailor our Elastic sketch implementation for running in P4 switch at line-rate but with a small accuracy drop.
