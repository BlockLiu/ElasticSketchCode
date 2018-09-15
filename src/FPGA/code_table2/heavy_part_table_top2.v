///2018.01.21 BY DU Haiying
module heavy_part_table_top2 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_in_wr2,
input	wire	[95:0] ip_addr_in2,
output 	wire	ip_addr_in_alf2,
//-------output ip_address&value to table1----
output	wire 	ip_addr_value_out_wr2,
output 	wire 	[95:0]ip_addr_value_out2,
input 	wire	ip_addr_value_out_alf2

);





wire ip_addr_out_wr2;
wire [127:0]ip_addr_out2;
wire ip_addr_out_alf2;

wire ip_addr_rden2;
wire [11:0]ip_addr_rdaddr2;


heavy_part_table_read2 heavy_part_table_read2 (
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input ip address----
.ip_addr_in_wr2(ip_addr_in_wr2),
.ip_addr_in2(ip_addr_in2),
.ip_addr_in_alf2(ip_addr_in_alf2),

//-------output  rd&addr to ram2----
.ip_addr_rden2(ip_addr_rden2),
.ip_addr_rdaddr2(ip_addr_rdaddr2),
//-------output ip_address&time to compare----
.ip_addr_out_wr2(ip_addr_out_wr2),
.ip_addr_out2(ip_addr_out2),
.ip_addr_out_alf2(ip_addr_out_alf2)

);

wire [95:0] ip_addr_rdvalue2;
wire ip_addr_wren2;
wire [11:0]ip_addr_wraddr2;
wire [95:0] ip_addr_wrvalue2;

heavy_part_table_compare2 heavy_part_table_compare2 (
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input ip address----
.ip_addr_time_in_wr2(ip_addr_out_wr2),
.ip_addr_time_in2(ip_addr_out2),
.ip_addr_time_in_alf2(ip_addr_out_alf2),

//-------output  time&addr&data to ram0----
.ip_addr_rdvalue2(ip_addr_rdvalue2),
.ip_addr_wren2(ip_addr_wren2),
.ip_addr_wraddr2(ip_addr_wraddr2),
.ip_addr_wrvalue2(ip_addr_wrvalue2),
//-------output ip_address&value to table1----
.ip_addr_value_out_wr2(ip_addr_value_out_wr2),
.ip_addr_value_out2(ip_addr_value_out2),
.ip_addr_value_out_alf2(ip_addr_value_out_alf2)

);



ram_96_4096 ram2_96_4096 (
.data(ip_addr_wrvalue2),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr2),
.rdclock(clk),
.rden(ip_addr_rden2),
.wraddress(ip_addr_wraddr2),
.wrclock(clk),
.wren(ip_addr_wren2),
.q(ip_addr_rdvalue2));


endmodule

