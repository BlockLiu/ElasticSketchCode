///2018.01.20 BY DU Haiying
module heavy_part_table_read0 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_in_wr0,
input	wire	[43:0] ip_addr_in0,
output 	wire	ip_addr_in_alf0,

//-------output  rd&addr to ram0----
output	reg 	ip_addr_rden0,
output 	reg 	[11:0] ip_addr_rdaddr0,
//-------output ip_address&time to compare----
output	reg 	ip_addr_out_wr0,
output 	reg 	[107:0]ip_addr_out0,//[95:32] time,[31:0] ip address
input 	wire	ip_addr_out_alf0

);


//---------regsiters of fifo----------------
reg 	ip_addr_in_rd0;
wire	[43:0] ip_addr_in_q0;
wire 	ip_addr_in_empty0;
wire 	[8:0] ip_addr_in_usedw0;

assign ip_addr_in_alf0=ip_addr_in_usedw0[8];

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
		ip_addr_out_wr0<=1'b0;
		ip_addr_out0<=108'b0;
	
		ip_addr_in_rd0<=1'b0;
	
		ip_addr_rdaddr0<=12'b0;
		ip_addr_rden0<=1'b0;   
		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_out_wr0<=1'b0;
			ip_addr_out0<=108'b0;
	
			ip_addr_in_rd0<=1'b0;
	
			ip_addr_rdaddr0<=12'b0;
			ip_addr_rden0<=1'b0; 		
			if(ip_addr_in_empty0==1'b0&&ip_addr_out_alf0==1'b0)begin
				ip_addr_in_rd0<=1'b1;											
				current_state<=start_read_s;							
			end
			else begin
				current_state<=idle_s;			
			end		
		end
	
		start_read_s:begin	
			ip_addr_in_rd0<=1'b0;	
			ip_addr_rden0<=1'b1; 
			ip_addr_rdaddr0<=ip_addr_in_q0[11:0];
				
			ip_addr_out_wr0<=1'b1;
			ip_addr_out0<={read_time,ip_addr_in_q0};//send readtime and ip address
			if(ip_addr_in_empty0==1'b0&&ip_addr_out_alf0==1'b0)begin
				ip_addr_in_rd0<=1'b1;												
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_in_rd0<=1'b0;
				ip_addr_rden0<=1'b0; 
			current_state<=idle_s;			
			end												
		end
		default:current_state<=idle_s;
	endcase
	end
end


 fifo_44_512 heavy_part_fifo_inst0(
	.aclr(!reset),
	.clock(clk),
	.data(ip_addr_in0),
	.rdreq(ip_addr_in_rd0),
	.wrreq(ip_addr_in_wr0),
	.empty(ip_addr_in_empty0),
	.q(ip_addr_in_q0),
	.usedw(ip_addr_in_usedw0) 
 ); 


endmodule









