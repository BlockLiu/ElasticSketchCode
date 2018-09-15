///2018.01.21 BY DU Haiying
module heavy_part_table_top0 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_in_wr0,
input	wire	[31:0] ip_addr_in0,
output 	wire	ip_addr_in_alf0,
//-------output ip_address&value to table1----
output	wire 	ip_addr_value_out_wr0,
output 	wire 	[95:0]ip_addr_value_out0,
input 	wire	ip_addr_value_out_alf0

);





wire ip_addr_out_wr0;
wire [95:0]ip_addr_out0;
wire ip_addr_out_alf0;

wire ip_addr_rden0;
wire [11:0]ip_addr_rdaddr0;

wire ip_addr_checksum_wr;
wire [43:0] ip_addr_checksum;

half32_16bit half32_16bit(
.clk(clk),
.reset(reset),
//-------input ip address----
.ip_address_in_wr(ip_addr_in_wr0),
.ip_address_in(ip_addr_in0),
//-----------output hash value----//
.ip_addr_checksum_wr(ip_addr_checksum_wr),
.ip_addr_checksum(ip_addr_checksum)

);



heavy_part_table_read0 heavy_part_table_read0 (
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input ip address----
.ip_addr_in_wr0(ip_addr_checksum_wr),
.ip_addr_in0(ip_addr_checksum),
.ip_addr_in_alf0(ip_addr_in_alf0),

//-------output  rd&addr to ram0----
.ip_addr_rden0(ip_addr_rden0),
.ip_addr_rdaddr0(ip_addr_rdaddr0),
//-------output ip_address&time to compare----
.ip_addr_out_wr0(ip_addr_out_wr0),
.ip_addr_out0(ip_addr_out0),
.ip_addr_out_alf0(ip_addr_out_alf0)

);

wire [95:0] ip_addr_rdvalue0;
wire ip_addr_wren0;
wire [11:0]ip_addr_wraddr0;
wire [95:0] ip_addr_wrvalue0;

heavy_part_table_compare0 heavy_part_table_compare0 (
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input from heavy_part_table_read0----
.ip_addr_time_in_wr0(ip_addr_out_wr0),
.ip_addr_time_in0(ip_addr_out0),
.ip_addr_time_in_alf0(ip_addr_out_alf0),

//-------output  time&addr&data to ram0----
.ip_addr_rdvalue0(ip_addr_rdvalue0),
.ip_addr_wren0(ip_addr_wren0),
.ip_addr_wraddr0(ip_addr_wraddr0),
.ip_addr_wrvalue0(ip_addr_wrvalue0),
//-------output ip_address&value to table1----
.ip_addr_value_out_wr0(ip_addr_value_out_wr0),
.ip_addr_value_out0(ip_addr_value_out0),
.ip_addr_value_out_alf0(ip_addr_value_out_alf0)

);



ram_96_4096 ram0_96_4096 (
.data(ip_addr_wrvalue0),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr0),
.rdclock(clk),
.rden(ip_addr_rden0),
.wraddress(ip_addr_wraddr0),
.wrclock(clk),
.wren(ip_addr_wren0),
.q(ip_addr_rdvalue0));

endmodule


