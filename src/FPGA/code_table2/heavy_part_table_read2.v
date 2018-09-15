///2018.01.20 BY DU Haiying
module heavy_part_table_read2 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_in_wr2,
input	wire	[95:0] ip_addr_in2,//[95:64] ip addr,[63:32] value,[31:0] counter,the hash vlaue is the ip_addr_time_in0[83:72]
output 	wire	ip_addr_in_alf2,

//-------output  rd&addr to ram0----
output	reg 	ip_addr_rden2,
output 	reg 	[11:0] ip_addr_rdaddr2,
//-------output ip_address&time to compare----
output	reg 	ip_addr_out_wr2,
output 	reg 	[127:0]ip_addr_out2,
input 	wire	ip_addr_out_alf2

);


//---------regsiters of fifo----------------
reg 	ip_addr_in_rd2;
wire	[127:0] ip_addr_in_q2;
wire 	ip_addr_in_empty2;
wire 	[8:0] ip_addr_in_usedw2;

assign ip_addr_in_alf2=ip_addr_in_usedw2[8];


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
		ip_addr_out_wr2<=1'b0;
		ip_addr_out2<=128'b0;
	
		ip_addr_in_rd2<=1'b0;
	 
		ip_addr_rdaddr2<=12'b0;
		ip_addr_rden2<=1'b0;   
		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_out_wr2<=1'b0;
			ip_addr_out2<=128'b0;
	
			ip_addr_in_rd2<=1'b0;
			
			ip_addr_rdaddr2<=12'b0;
			ip_addr_rden2<=1'b0; 		
			if(ip_addr_in_empty2==1'b0&&ip_addr_out_alf2==1'b0)begin
				ip_addr_in_rd2<=1'b1;											
				current_state<=start_read_s;							
			end
			else begin
				current_state<=idle_s;			
			end		
		end
	
		start_read_s:begin	
			ip_addr_in_rd2<=1'b0;
			ip_addr_rden2<=1'b1; 
			ip_addr_rdaddr2<=ip_addr_in_q2[83:72];
				
			ip_addr_out_wr2<=1'b1;
			ip_addr_out2<={read_time,ip_addr_in_q2[95:32]};//send readtime and ip address and value
			if(ip_addr_in_empty2==1'b0&&ip_addr_out_alf2==1'b0)begin
				ip_addr_in_rd2<=1'b1;												
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_in_rd2<=1'b0;
				ip_addr_rden2<=1'b0; 
				current_state<=idle_s;			
			end												
		end
		default:current_state<=idle_s;
	endcase
	end
end


 fifo_96_512 heavy_part_fifo_inst2(
	.aclr(!reset),
	.clock(clk),
	.data(ip_addr_in2),
	.rdreq(ip_addr_in_rd2),
	.wrreq(ip_addr_in_wr2),
	.empty(ip_addr_in_empty2),
	.q(ip_addr_in_q2),
	.usedw(ip_addr_in_usedw2) 
 ); 


endmodule









