///2018.01.21 BY DU Haiying
module heavy_part_table_top3 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_in_wr3,
input	wire	[95:0] ip_addr_in3,
output 	wire	ip_addr_in_alf3,
//-------output ip_address&value to table1----
output	wire 	ip_addr_value_out_wr3,
output 	wire 	[95:0]ip_addr_value_out3,
input 	wire	ip_addr_value_out_alf3

);





wire ip_addr_out_wr3;
wire [127:0]ip_addr_out3;
wire ip_addr_out_alf3;

wire ip_addr_rden3;
wire [11:0]ip_addr_rdaddr3;


heavy_part_table_read3 heavy_part_table_read3 (
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input ip address----
.ip_addr_in_wr3(ip_addr_in_wr3),
.ip_addr_in3(ip_addr_in3),
.ip_addr_in_alf3(ip_addr_in_alf3),

//-------output  rd&addr to ram2----
.ip_addr_rden3(ip_addr_rden3),
.ip_addr_rdaddr3(ip_addr_rdaddr3),
//-------output ip_address&time to compare----
.ip_addr_out_wr3(ip_addr_out_wr3),
.ip_addr_out3(ip_addr_out3),
.ip_addr_out_alf3(ip_addr_out_alf3)

);

wire [95:0] ip_addr_rdvalue3;
wire ip_addr_wren3;
wire [11:0]ip_addr_wraddr3;
wire [95:0] ip_addr_wrvalue3;

heavy_part_table_compare3 heavy_part_table_compare3 (
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input ip address----
.ip_addr_time_in_wr3(ip_addr_out_wr3),
.ip_addr_time_in3(ip_addr_out3),
.ip_addr_time_in_alf3(ip_addr_out_alf3),

//-------output  time&addr&data to ram0----
.ip_addr_rdvalue3(ip_addr_rdvalue3),
.ip_addr_wren3(ip_addr_wren3),
.ip_addr_wraddr3(ip_addr_wraddr3),
.ip_addr_wrvalue3(ip_addr_wrvalue3),
//-------output ip_address&value to lightpart----
.ip_addr_value_out_wr3(ip_addr_value_out_wr3),
.ip_addr_value_out3(ip_addr_value_out3),
.ip_addr_value_out_alf3(ip_addr_value_out_alf3)

);



ram_96_4096 ram3_96_4096 (
.data(ip_addr_wrvalue3),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr3),
.rdclock(clk),
.rden(ip_addr_rden3),
.wraddress(ip_addr_wraddr3),
.wrclock(clk),
.wren(ip_addr_wren3),
.q(ip_addr_rdvalue3));


endmodule

