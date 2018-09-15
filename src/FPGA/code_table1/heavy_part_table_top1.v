///2018.01.21 BY DU Haiying
module heavy_part_table_top1 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_in_wr1,
input	wire	[95:0] ip_addr_in1,
output 	wire	ip_addr_in_alf1,
//-------output ip_address&value to table1----
output	wire 	ip_addr_value_out_wr1,
output 	wire 	[95:0]ip_addr_value_out1,
input 	wire	ip_addr_value_out_alf1

);





wire ip_addr_out_wr1;
wire [127:0]ip_addr_out1;
wire ip_addr_out_alf1;

wire ip_addr_rden1;
wire [11:0]ip_addr_rdaddr1;


heavy_part_table_read1 heavy_part_table_read1 (
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input ip address----
.ip_addr_in_wr1(ip_addr_in_wr1),
.ip_addr_in1(ip_addr_in1),
.ip_addr_in_alf1(ip_addr_in_alf1),

//-------output  rd&addr to ram0----
.ip_addr_rden1(ip_addr_rden1),
.ip_addr_rdaddr1(ip_addr_rdaddr1),
//-------output ip_address&time to compare----
.ip_addr_out_wr1(ip_addr_out_wr1),
.ip_addr_out1(ip_addr_out1),
.ip_addr_out_alf1(ip_addr_out_alf1)

);

wire [95:0] ip_addr_rdvalue1;
wire ip_addr_wren1;
wire [11:0]ip_addr_wraddr1;
wire [95:0] ip_addr_wrvalue1;

heavy_part_table_compare1 heavy_part_table_compare1 (
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input ip address----
.ip_addr_time_in_wr1(ip_addr_out_wr1),
.ip_addr_time_in1(ip_addr_out1),
.ip_addr_time_in_alf1(ip_addr_out_alf1),

//-------output  time&addr&data to ram0----
.ip_addr_rdvalue1(ip_addr_rdvalue1),
.ip_addr_wren1(ip_addr_wren1),
.ip_addr_wraddr1(ip_addr_wraddr1),
.ip_addr_wrvalue1(ip_addr_wrvalue1),
//-------output ip_address&value to table1----
.ip_addr_value_out_wr1(ip_addr_value_out_wr1),
.ip_addr_value_out1(ip_addr_value_out1),
.ip_addr_value_out_alf1(ip_addr_value_out_alf1)

);



ram_96_4096 ram1_96_4096 (
.data(ip_addr_wrvalue1),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr1),
.rdclock(clk),
.rden(ip_addr_rden1),
.wraddress(ip_addr_wraddr1),
.wrclock(clk),
.wren(ip_addr_wren1),
.q(ip_addr_rdvalue1));

endmodule


