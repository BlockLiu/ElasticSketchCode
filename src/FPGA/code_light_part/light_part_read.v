///2018.01.21 BY DU Haiying
module light_part_read (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_light_in_wr,
input	wire	[95:0] ip_addr_light_in,
output 	wire	ip_addr_light_in_alf,

//-------output  rd&addr to ram3----
output	reg 	ip_addr_rden00,
output 	reg 	[15:0] ip_addr_rdaddr00,
output	reg 	ip_addr_rden01,
output 	reg 	[15:0] ip_addr_rdaddr01,
output	reg 	ip_addr_rden02,
output 	reg 	[15:0] ip_addr_rdaddr02,
output	reg 	ip_addr_rden03,
output 	reg 	[15:0] ip_addr_rdaddr03,
output	reg 	ip_addr_rden04,
output 	reg 	[15:0] ip_addr_rdaddr04,
output	reg 	ip_addr_rden05,
output 	reg 	[15:0] ip_addr_rdaddr05,
output	reg 	ip_addr_rden06,
output 	reg 	[15:0] ip_addr_rdaddr06,
output	reg 	ip_addr_rden07,
output 	reg 	[15:0] ip_addr_rdaddr07,
//-------output ip_address&time to compare----
output	reg 	ip_addr_light_out_wr,
output 	reg 	[127:0]ip_addr_light_out,
input 	wire	ip_addr_light_out_alf

);


//---------regsiters of fifo----------------
reg 	ip_addr_light_in_rd;
wire	[95:0] ip_addr_light_in_q;
wire 	ip_addr_light_in_empty;
wire 	[8:0] ip_addr_light_in_usedw;
reg 	[95:0] ip_addr_light_in_q_r;
assign ip_addr_light_in_alf=ip_addr_light_in_usedw[8];


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
		ip_addr_light_out_wr<=1'b0;
		ip_addr_light_out<=128'b0;
	
		ip_addr_light_in_rd<=1'b0;
	 
		ip_addr_rdaddr00<=16'b0;
		ip_addr_rden00<=1'b0;   
		ip_addr_rdaddr01<=16'b0;
		ip_addr_rden01<=1'b0; 
		ip_addr_rdaddr02<=16'b0;
		ip_addr_rden02<=1'b0;   
		ip_addr_rdaddr03<=16'b0;
		ip_addr_rden03<=1'b0; 
		ip_addr_rdaddr04<=16'b0;
		ip_addr_rden04<=1'b0;   
		ip_addr_rdaddr05<=16'b0;
		ip_addr_rden05<=1'b0; 
		ip_addr_rdaddr06<=16'b0;
		ip_addr_rden06<=1'b0;   
		ip_addr_rdaddr07<=16'b0;
		ip_addr_rden07<=1'b0; 


		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_light_out_wr<=1'b0;
			ip_addr_light_out<=128'b0;
	
			ip_addr_light_in_rd<=1'b0;
			
		ip_addr_rdaddr00<=16'b0;
		ip_addr_rden00<=1'b0;   
		ip_addr_rdaddr01<=16'b0;
		ip_addr_rden01<=1'b0; 
		ip_addr_rdaddr02<=16'b0;
		ip_addr_rden02<=1'b0;   
		ip_addr_rdaddr03<=16'b0;
		ip_addr_rden03<=1'b0; 
		ip_addr_rdaddr04<=16'b0;
		ip_addr_rden04<=1'b0;   
		ip_addr_rdaddr05<=16'b0;
		ip_addr_rden05<=1'b0; 
		ip_addr_rdaddr06<=16'b0;
		ip_addr_rden06<=1'b0;   
		ip_addr_rdaddr07<=16'b0;
		ip_addr_rden07<=1'b0; 		
			if(ip_addr_light_in_empty==1'b0&&ip_addr_light_out_alf==1'b0)begin
				ip_addr_light_in_rd<=1'b1;											
				current_state<=start_read_s;							
			end
			else begin
				current_state<=idle_s;			
			end		
		end
	
	start_read_s:begin
		case(ip_addr_light_in_q[2:0])
		
		3'b0:begin
			ip_addr_light_in_rd<=1'b0;
			ip_addr_rden00<=1'b1; 
			ip_addr_rdaddr00<=ip_addr_light_in_q[95:80];
				
			ip_addr_light_out_wr<=1'b1;
			ip_addr_light_out<={read_time,ip_addr_light_in_q[95:32]};
			if(ip_addr_light_in_empty==1'b0&&ip_addr_light_out_alf==1'b0)begin
				ip_addr_light_in_rd<=1'b1;				
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_light_in_rd<=1'b0;
				ip_addr_rden00<=1'b0; 
				current_state<=idle_s;			
			end											
		end
		3'b001:begin
			ip_addr_light_in_rd<=1'b0;
			ip_addr_rden01<=1'b1; 
			ip_addr_rdaddr01<=ip_addr_light_in_q[95:80];
				
			ip_addr_light_out_wr<=1'b1;
			ip_addr_light_out<={read_time,ip_addr_light_in_q[95:32]};
			if(ip_addr_light_in_empty==1'b0&&ip_addr_light_out_alf==1'b0)begin
				ip_addr_light_in_rd<=1'b1;				
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_light_in_rd<=1'b0;
				ip_addr_rden01<=1'b0; 
				current_state<=idle_s;			
			end											
		end		
		
		3'b010:begin
			ip_addr_light_in_rd<=1'b0;
			ip_addr_rden02<=1'b1; 
			ip_addr_rdaddr02<=ip_addr_light_in_q[95:80];
				
			ip_addr_light_out_wr<=1'b1;
			ip_addr_light_out<={read_time,ip_addr_light_in_q[95:32]};
			if(ip_addr_light_in_empty==1'b0&&ip_addr_light_out_alf==1'b0)begin
				ip_addr_light_in_rd<=1'b1;				
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_light_in_rd<=1'b0;
				ip_addr_rden02<=1'b0; 
				current_state<=idle_s;			
			end											
		end		
		
		3'b011:begin
			ip_addr_light_in_rd<=1'b0;
			ip_addr_rden03<=1'b1; 
			ip_addr_rdaddr03<=ip_addr_light_in_q[95:80];
				
			ip_addr_light_out_wr<=1'b1;
			ip_addr_light_out<={read_time,ip_addr_light_in_q[95:32]};
			if(ip_addr_light_in_empty==1'b0&&ip_addr_light_out_alf==1'b0)begin
				ip_addr_light_in_rd<=1'b1;				
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_light_in_rd<=1'b0;
				ip_addr_rden03<=1'b0; 
				current_state<=idle_s;			
			end											
		end		
		
		3'b100:begin
			ip_addr_light_in_rd<=1'b0;
			ip_addr_rden04<=1'b1; 
			ip_addr_rdaddr04<=ip_addr_light_in_q[95:80];
				
			ip_addr_light_out_wr<=1'b1;
			ip_addr_light_out<={read_time,ip_addr_light_in_q[95:32]};
			if(ip_addr_light_in_empty==1'b0&&ip_addr_light_out_alf==1'b0)begin
				ip_addr_light_in_rd<=1'b1;				
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_light_in_rd<=1'b0;
				ip_addr_rden04<=1'b0; 
				current_state<=idle_s;			
			end											
		end		
		
		
		3'b101:begin
			ip_addr_light_in_rd<=1'b0;
			ip_addr_rden05<=1'b1; 
			ip_addr_rdaddr05<=ip_addr_light_in_q[95:80];
				
			ip_addr_light_out_wr<=1'b1;
			ip_addr_light_out<={read_time,ip_addr_light_in_q[95:32]};
			if(ip_addr_light_in_empty==1'b0&&ip_addr_light_out_alf==1'b0)begin
				ip_addr_light_in_rd<=1'b1;				
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_light_in_rd<=1'b0;
				ip_addr_rden05<=1'b0; 
				current_state<=idle_s;			
			end											
		end		
		
		
		3'b110:begin
			ip_addr_light_in_rd<=1'b0;
			ip_addr_rden06<=1'b1; 
			ip_addr_rdaddr06<=ip_addr_light_in_q[95:80];
				
			ip_addr_light_out_wr<=1'b1;
			ip_addr_light_out<={read_time,ip_addr_light_in_q[95:32]};
			if(ip_addr_light_in_empty==1'b0&&ip_addr_light_out_alf==1'b0)begin
				ip_addr_light_in_rd<=1'b1;				
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_light_in_rd<=1'b0;
				ip_addr_rden06<=1'b0; 
				current_state<=idle_s;			
			end											
		end		
		
		3'b111:begin
			ip_addr_light_in_rd<=1'b0;
			ip_addr_rden07<=1'b1; 
			ip_addr_rdaddr07<=ip_addr_light_in_q[95:80];
				
			ip_addr_light_out_wr<=1'b1;
			ip_addr_light_out<={read_time,ip_addr_light_in_q[95:32]};
			if(ip_addr_light_in_empty==1'b0&&ip_addr_light_out_alf==1'b0)begin
				ip_addr_light_in_rd<=1'b1;				
				current_state<=start_read_s;							
			end
			else begin
				ip_addr_light_in_rd<=1'b0;
				ip_addr_rden07<=1'b0; 
				current_state<=idle_s;			
			end											
		end		
		
		
		
		
		endcase
	end
		default:current_state<=idle_s;
	endcase
	end
end


 fifo_96_512 light_part_fifo_inst(
	.aclr(!reset),
	.clock(clk),
	.data(ip_addr_light_in),
	.rdreq(ip_addr_light_in_rd),
	.wrreq(ip_addr_light_in_wr),
	.empty(ip_addr_light_in_empty),
	.q(ip_addr_light_in_q),
	.usedw(ip_addr_light_in_usedw) 
 ); 


endmodule









