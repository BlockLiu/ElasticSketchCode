///2018.01.20 BY DU Haiying
module heavy_part_table_read1 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input from heavy_part_table_compare0----
input	wire	ip_addr_in_wr1,
input	wire	[95:0] ip_addr_in1,//[95:64] ip addr,[63:32] value,[31:0] counter,the hash vlaue is the ip_addr_time_in0[79:68]
output 	wire	ip_addr_in_alf1,

//-------output  rd&addr to ram1----
output	reg 	ip_addr_rden1,
output 	reg 	[11:0] ip_addr_rdaddr1,
//-------output ip_address&time to compare----
output	reg 	ip_addr_out_wr1,
output 	reg 	[127:0]ip_addr_out1,//[127:64] time,[63:32] ip addr,[31:0] value,
input 	wire	ip_addr_out_alf1

);


//---------regsiters of fifo----------------
reg 	ip_addr_in_rd1;
wire	[95:0] ip_addr_in_q1;
wire 	ip_addr_in_empty1;
wire 	[8:0] ip_addr_in_usedw1;

assign ip_addr_in_alf1=ip_addr_in_usedw1[8];


//---------regsiters ----------------
reg 	[63:0] read_time;
reg 	[1:0] current_state;

parameter		idle_s=2'b0,
				start_read_s=2'b01;

always @ (posedge clk or negedge reset) begin 
	if (!reset) begin
		read_time<=64'b0;
	end
	else begin
		read_time<=read_time+64'd1;
	end
end

always @(posedge clk or negedge reset)	begin   
	if(!reset) begin
		ip_addr_out_wr1<=1'b0;
		ip_addr_out1<=128'b0;
	
		ip_addr_in_rd1<=1'b0;
	 
		ip_addr_rdaddr1<=12'b0;
		ip_addr_rden1<=1'b0;   
		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_out_wr1<=1'b0;
			ip_addr_out1<=128'b0;
	
			ip_addr_in_rd1<=1'b0;
			
			ip_addr_rdaddr1<=12'b0;
			ip_addr_rden1<=1'b0; 		
			if(ip_addr_in_empty1==1'b0&&ip_addr_out_alf1==1'b0)begin
				ip_addr_in_rd1<=1'b1;											
				current_state<=start_read_s;							
			end
			else begin
				current_state<=idle_s;			
			end		
		end
	
		start_read_s:begin	
			ip_addr_in_rd1<=1'b0;
			ip_addr_rden1<=1'b1; 
			ip_addr_rdaddr1<=ip_addr_in_q1[79:68];	
			
			ip_addr_out_wr1<=1'b1;
			ip_addr_out1<={read_time,ip_addr_in_q1[95:32]};	//send readtime and ip address and value
			if(ip_addr_in_empty1==1'b0&&ip_addr_out_alf1==1'b0)begin
				ip_addr_in_rd1<=1'b1;											
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_in_rd1<=1'b0;
				ip_addr_rden1<=1'b0; 
				current_state<=idle_s;			
			end												
		end
		default:current_state<=idle_s;
	endcase
	end
end


 fifo_96_512 heavy_part_fifo_inst1(
	.aclr(!reset),
	.clock(clk),
	.data(ip_addr_in1),
	.rdreq(ip_addr_in_rd1),
	.wrreq(ip_addr_in_wr1),
	.empty(ip_addr_in_empty1),
	.q(ip_addr_in_q1),
	.usedw(ip_addr_in_usedw1) 
 ); 


endmodule









