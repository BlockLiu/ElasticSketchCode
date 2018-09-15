## FPGA implementation

# Overall design
 				
| ip_addrress --> | heavy_part_table0 | heavy_part_table1 | heavy_part_table2| heavy_part_table3 | light_part_table |
| :-: | :-: | :-: | :-: | :-: | -: |
				
- In each of `heavy_part_table0~3`, there is a RAM (width:96bits, depth:4096). 
- The light part contains a RAM (width:8bit, depth:2^19). 
- Every table has its own FIFO queue to buffer IP addresses from upper module, making the procedure can be pipelined.


# Introduction to each table:

- `heavy_part_table0`: Recieve an IP address, find a bucket using hash function h0(.). Then, record the IP address in this bucket or send the IP address to heavy_part_table1 according to some rules.
- `heavy_part_table1`: Recieve an IP address from heavy_part_table0, find a bucket using hash function h1(.). Then, record the IP address in this bucket or send the IP address to heavy_part_table2 according to some rules.
- `heavy_part_table2`: Recieve an IP address from heavy_part_table1, find a bucket using hash function h2(.). Then, record the IP address in this bucket or send the IP address to heavy_part_table3 according to some rules.
- `heavy_part_table3`: Recieve an IP address from heavy_part_table2, find a bucket using hash function h3(.). Then, record the IP address in this bucket or send the IP address to light_part_table according to some rules.
- `light_part_table`: Recieve an IP address from heavy_part_table3, find a bucket using hash function h4(.). Then, record the IP address in this bucket.

# Our experiments setups

- compile: `Quartus II 15.0`
- language: `Verilog`
- FPGA version: `Altera  StratixV：5SGXEA7H3F35C3`
- frequency: `Fmax` and `Restricted Fmax` are both 150.11 MHz.



Resource report can be found in `ip_addr_top.fit.rpt`.
Time series analysis can be found in `ip_addr_top.sta.rpt`.

