///2018.01.21 BY DU Haiying
module light_part_top(
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_light_in_wr,
input	wire	[95:0] ip_addr_light_in,
output 	wire	ip_addr_light_in_alf


);





wire ip_addr_light_out_wr;
wire [127:0]ip_addr_light_out;
wire ip_addr_light_out_alf;

wire ip_addr_rden00;
wire [15:0]ip_addr_rdaddr00;
wire ip_addr_rden01;
wire [15:0]ip_addr_rdaddr01;

wire ip_addr_rden02;
wire [15:0]ip_addr_rdaddr02;
wire ip_addr_rden03;
wire [15:0]ip_addr_rdaddr03;

wire ip_addr_rden04;
wire [15:0]ip_addr_rdaddr04;
wire ip_addr_rden05;
wire [15:0]ip_addr_rdaddr05;

wire ip_addr_rden06;
wire [15:0]ip_addr_rdaddr06;
wire ip_addr_rden07;
wire [15:0]ip_addr_rdaddr07;



light_part_read  light_part_read(
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input ip address----
.ip_addr_light_in_wr(ip_addr_light_in_wr),
.ip_addr_light_in(ip_addr_light_in),
.ip_addr_light_in_alf(ip_addr_light_in_alf),

//-------output  rd&addr to ram3----
.ip_addr_rden00(ip_addr_rden00),
.ip_addr_rdaddr00(ip_addr_rdaddr00),
.ip_addr_rden01(ip_addr_rden01),
.ip_addr_rdaddr01(ip_addr_rdaddr01),
.ip_addr_rden02(ip_addr_rden02),
.ip_addr_rdaddr02(ip_addr_rdaddr02),
.ip_addr_rden03(ip_addr_rden03),
.ip_addr_rdaddr03(ip_addr_rdaddr03),
.ip_addr_rden04(ip_addr_rden04),
.ip_addr_rdaddr04(ip_addr_rdaddr04),
.ip_addr_rden05(ip_addr_rden05),
.ip_addr_rdaddr05(ip_addr_rdaddr05),
.ip_addr_rden06(ip_addr_rden06),
.ip_addr_rdaddr06(ip_addr_rdaddr06),
.ip_addr_rden07(ip_addr_rden07),
.ip_addr_rdaddr07(ip_addr_rdaddr07),

//-------output ip_address&time to compare----
.ip_addr_light_out_wr(ip_addr_light_out_wr),
.ip_addr_light_out(ip_addr_light_out),
.ip_addr_light_out_alf(ip_addr_light_out_alf)

);

wire [7:0] ip_addr_rdvalue00;
wire ip_addr_wren00;
wire [15:0]ip_addr_wraddr00;
wire [7:0] ip_addr_wrvalue00;

wire [7:0] ip_addr_rdvalue01;
wire ip_addr_wren01;
wire [15:0]ip_addr_wraddr01;
wire [7:0] ip_addr_wrvalue01;

wire [7:0] ip_addr_rdvalue02;
wire ip_addr_wren02;
wire [15:0]ip_addr_wraddr02;
wire [7:0] ip_addr_wrvalue02;

wire [7:0] ip_addr_rdvalue03;
wire ip_addr_wren03;
wire [15:0]ip_addr_wraddr03;
wire [7:0] ip_addr_wrvalue03;

wire [7:0] ip_addr_rdvalue04;
wire ip_addr_wren04;
wire [15:0]ip_addr_wraddr04;
wire [7:0] ip_addr_wrvalue04;

wire [7:0] ip_addr_rdvalue05;
wire ip_addr_wren05;
wire [15:0]ip_addr_wraddr05;
wire [7:0] ip_addr_wrvalue05;

wire [7:0] ip_addr_rdvalue06;
wire ip_addr_wren06;
wire [15:0]ip_addr_wraddr06;
wire [7:0] ip_addr_wrvalue06;

wire [7:0] ip_addr_rdvalue07;
wire ip_addr_wren07;
wire [15:0]ip_addr_wraddr07;
wire [7:0] ip_addr_wrvalue07;

light_part_compare light_part_compare (
//--------clk&reset----------------
.clk(clk),  //clock
.reset(reset),	//reset
//-------input ip address----
.ip_addr_time_light_in_wr(ip_addr_light_out_wr),
.ip_addr_time_light_in(ip_addr_light_out),
.ip_addr_time_light_in_alf(ip_addr_light_out_alf),

//-------output  time&addr&data to ram3----
.ip_addr_rdvalue00(ip_addr_rdvalue00),
.ip_addr_wren00(ip_addr_wren00),
.ip_addr_wraddr00(ip_addr_wraddr00),
.ip_addr_wrvalue00(ip_addr_wrvalue00),
.ip_addr_rdvalue01(ip_addr_rdvalue01),
.ip_addr_wren01(ip_addr_wren01),
.ip_addr_wraddr01(ip_addr_wraddr01),
.ip_addr_wrvalue01(ip_addr_wrvalue01),
.ip_addr_rdvalue02(ip_addr_rdvalue02),
.ip_addr_wren02(ip_addr_wren02),
.ip_addr_wraddr02(ip_addr_wraddr02),
.ip_addr_wrvalue02(ip_addr_wrvalue02),
.ip_addr_rdvalue03(ip_addr_rdvalue03),
.ip_addr_wren03(ip_addr_wren03),
.ip_addr_wraddr03(ip_addr_wraddr03),
.ip_addr_wrvalue03(ip_addr_wrvalue03),
.ip_addr_rdvalue04(ip_addr_rdvalue04),
.ip_addr_wren04(ip_addr_wren04),
.ip_addr_wraddr04(ip_addr_wraddr04),
.ip_addr_wrvalue04(ip_addr_wrvalue04),
.ip_addr_rdvalue05(ip_addr_rdvalue05),
.ip_addr_wren05(ip_addr_wren05),
.ip_addr_wraddr05(ip_addr_wraddr05),
.ip_addr_wrvalue05(ip_addr_wrvalue05),
.ip_addr_rdvalue06(ip_addr_rdvalue06),
.ip_addr_wren06(ip_addr_wren06),
.ip_addr_wraddr06(ip_addr_wraddr06),
.ip_addr_wrvalue06(ip_addr_wrvalue06),
.ip_addr_rdvalue07(ip_addr_rdvalue07),
.ip_addr_wren07(ip_addr_wren07),
.ip_addr_wraddr07(ip_addr_wraddr07),
.ip_addr_wrvalue07(ip_addr_wrvalue07)


);



ram_8_65536 ram_8_65536_inst0 (
.data(ip_addr_wrvalue00),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr00),
.rdclock(clk),
.rden(ip_addr_rden00),
.wraddress(ip_addr_wraddr00),
.wrclock(clk),
.wren(ip_addr_wren00),
.q(ip_addr_rdvalue00));


ram_8_65536 ram_8_65536_inst1 (
.data(ip_addr_wrvalue01),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr01),
.rdclock(clk),
.rden(ip_addr_rden01),
.wraddress(ip_addr_wraddr01),
.wrclock(clk),
.wren(ip_addr_wren01),
.q(ip_addr_rdvalue01));



ram_8_65536 ram_8_65536_inst2 (
.data(ip_addr_wrvalue02),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr02),
.rdclock(clk),
.rden(ip_addr_rden02),
.wraddress(ip_addr_wraddr02),
.wrclock(clk),
.wren(ip_addr_wren02),
.q(ip_addr_rdvalue02));


ram_8_65536 ram_8_65536_inst3 (
.data(ip_addr_wrvalue03),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr03),
.rdclock(clk),
.rden(ip_addr_rden03),
.wraddress(ip_addr_wraddr03),
.wrclock(clk),
.wren(ip_addr_wren03),
.q(ip_addr_rdvalue03));



ram_8_65536 ram_8_65536_inst4 (
.data(ip_addr_wrvalue04),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr04),
.rdclock(clk),
.rden(ip_addr_rden04),
.wraddress(ip_addr_wraddr04),
.wrclock(clk),
.wren(ip_addr_wren04),
.q(ip_addr_rdvalue04));


ram_8_65536 ram_8_65536_inst5 (
.data(ip_addr_wrvalue05),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr05),
.rdclock(clk),
.rden(ip_addr_rden05),
.wraddress(ip_addr_wraddr05),
.wrclock(clk),
.wren(ip_addr_wren05),
.q(ip_addr_rdvalue05));



ram_8_65536 ram_8_65536_inst6 (
.data(ip_addr_wrvalue06),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr06),
.rdclock(clk),
.rden(ip_addr_rden06),
.wraddress(ip_addr_wraddr06),
.wrclock(clk),
.wren(ip_addr_wren06),
.q(ip_addr_rdvalue06));


ram_8_65536 ram_8_65536_inst7 (
.data(ip_addr_wrvalue07),
.rd_aclr(!reset),
.rdaddress(ip_addr_rdaddr07),
.rdclock(clk),
.rden(ip_addr_rden07),
.wraddress(ip_addr_wraddr07),
.wrclock(clk),
.wren(ip_addr_wren07),
.q(ip_addr_rdvalue07));



endmodule

