///2018.01.20 BY DU Haiying
module heavy_part_table_read3 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_in_wr3,//[95:64] ip addr,[63:32] value,[31:0] counter,the hash vlaue is the ip_addr_time_in3[87:76]
input	wire	[95:0] ip_addr_in3,
output 	wire	ip_addr_in_alf3,

//-------output  rd&addr to ram3----
output	reg 	ip_addr_rden3,
output 	reg 	[11:0] ip_addr_rdaddr3,
//-------output ip_address&time to compare----
output	reg 	ip_addr_out_wr3,
output 	reg 	[127:0]ip_addr_out3,
input 	wire	ip_addr_out_alf3

);


//---------regsiters of fifo----------------
reg 	ip_addr_in_rd3;
wire	[95:0] ip_addr_in_q3;
wire 	ip_addr_in_empty3;
wire 	[8:0] ip_addr_in_usedw3;

assign ip_addr_in_alf3=ip_addr_in_usedw3[8];


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
		ip_addr_out_wr3<=1'b0;
		ip_addr_out3<=128'b0;
	
		ip_addr_in_rd3<=1'b0;
	 
		ip_addr_rdaddr3<=12'b0;
		ip_addr_rden3<=1'b0;   
		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_out_wr3<=1'b0;
			ip_addr_out3<=128'b0;
	
			ip_addr_in_rd3<=1'b0;
			
			ip_addr_rdaddr3<=12'b0;
			ip_addr_rden3<=1'b0; 		
			if(ip_addr_in_empty3==1'b0&&ip_addr_out_alf3==1'b0)begin
				ip_addr_in_rd3<=1'b1;											
				current_state<=start_read_s;							
			end
			else begin
				current_state<=idle_s;			
			end		
		end
	
		start_read_s:begin	
			ip_addr_rden3<=1'b0; 
			ip_addr_rdaddr3<=ip_addr_in_q3[87:76];
				
			ip_addr_out_wr3<=1'b1;
			ip_addr_out3<={read_time,ip_addr_in_q3[95:32]};
			if(ip_addr_in_empty3==1'b0&&ip_addr_out_alf3==1'b0)begin
				ip_addr_in_rd3<=1'b1;												
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_in_rd3<=1'b0;
				ip_addr_rden3<=1'b0; 
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
	.data(ip_addr_in3),
	.rdreq(ip_addr_in_rd3),
	.wrreq(ip_addr_in_wr3),
	.empty(ip_addr_in_empty3),
	.q(ip_addr_in_q3),
	.usedw(ip_addr_in_usedw3) 
 ); 


endmodule









