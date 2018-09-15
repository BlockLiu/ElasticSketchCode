///2018.01.20 BY DU Haiying
module light_part_compare (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_time_light_in_wr,
input	wire	[127:0] ip_addr_time_light_in,
output 	wire	ip_addr_time_light_in_alf,

//-------output  time&addr&data to ram3----
input	wire	[7:0] ip_addr_rdvalue00,
output	reg 	ip_addr_wren00,
output 	reg 	[15:0] ip_addr_wraddr00,
output 	reg 	[7:0] ip_addr_wrvalue00,
input	wire	[7:0] ip_addr_rdvalue01,
output	reg 	ip_addr_wren01,
output 	reg 	[15:0] ip_addr_wraddr01,
output 	reg 	[7:0] ip_addr_wrvalue01,
input	wire	[7:0] ip_addr_rdvalue02,
output	reg 	ip_addr_wren02,
output 	reg 	[15:0] ip_addr_wraddr02,
output 	reg 	[7:0] ip_addr_wrvalue02,
input	wire	[7:0] ip_addr_rdvalue03,
output	reg 	ip_addr_wren03,
output 	reg 	[15:0] ip_addr_wraddr03,
output 	reg 	[7:0] ip_addr_wrvalue03,
input	wire	[7:0] ip_addr_rdvalue04,
output	reg 	ip_addr_wren04,
output 	reg 	[15:0] ip_addr_wraddr04,
output 	reg 	[7:0] ip_addr_wrvalue04,
input	wire	[7:0] ip_addr_rdvalue05,
output	reg 	ip_addr_wren05,
output 	reg 	[15:0] ip_addr_wraddr05,
output 	reg 	[7:0] ip_addr_wrvalue05,
input	wire	[7:0] ip_addr_rdvalue06,
output	reg 	ip_addr_wren06,
output 	reg 	[15:0] ip_addr_wraddr06,
output 	reg 	[7:0] ip_addr_wrvalue06,
input	wire	[7:0] ip_addr_rdvalue07,
output	reg 	ip_addr_wren07,
output 	reg 	[15:0] ip_addr_wraddr07,
output 	reg 	[7:0] ip_addr_wrvalue07

);

//---------regsiters of fifo----------------
reg 	ip_addr_time_light_in_rd;
wire	[127:0] ip_addr_time_light_in_q;
wire 	ip_addr_time_light_in_empty;
wire 	[8:0] ip_addr_time_light_in_usedw;
reg 	[127:0]ip_addr_time_light_in_q_r;
assign ip_addr_time_light_in_alf=ip_addr_time_light_in_usedw[8];

//---------regsiters ----------------

reg 	[63:0] read_time;
reg 	[1:0] current_state;

parameter		idle_s=2'b0,
				write_data_s=2'b01,
				write_data_s_r=2'b10;
				

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
		ip_addr_time_light_in_rd<=1'b0;
		ip_addr_time_light_in_q_r<=128'b0;
		
		ip_addr_wren00<=1'b0;
		ip_addr_wraddr00<=16'b0;
		ip_addr_wrvalue00<=8'b0;		
		ip_addr_wren01<=1'b0;
		ip_addr_wraddr01<=16'b0;
		ip_addr_wrvalue01<=8'b0;
		ip_addr_wren02<=1'b0;
		ip_addr_wraddr02<=16'b0;
		ip_addr_wrvalue02<=8'b0;		
		ip_addr_wren03<=1'b0;
		ip_addr_wraddr03<=16'b0;
		ip_addr_wrvalue03<=8'b0;
		ip_addr_wren04<=1'b0;
		ip_addr_wraddr04<=16'b0;
		ip_addr_wrvalue04<=8'b0;		
		ip_addr_wren05<=1'b0;
		ip_addr_wraddr05<=16'b0;
		ip_addr_wrvalue05<=8'b0;
		ip_addr_wren06<=1'b0;
		ip_addr_wraddr06<=16'b0;
		ip_addr_wrvalue06<=8'b0;		
		ip_addr_wren07<=1'b0;
		ip_addr_wraddr07<=16'b0;
		ip_addr_wrvalue07<=8'b0;
		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_time_light_in_rd<=1'b0;
			ip_addr_time_light_in_q_r<=128'b0;
		
		ip_addr_wren00<=1'b0;
		ip_addr_wraddr00<=16'b0;
		ip_addr_wrvalue00<=8'b0;		
		ip_addr_wren01<=1'b0;
		ip_addr_wraddr01<=16'b0;
		ip_addr_wrvalue01<=8'b0;
		ip_addr_wren02<=1'b0;
		ip_addr_wraddr02<=16'b0;
		ip_addr_wrvalue02<=8'b0;		
		ip_addr_wren03<=1'b0;
		ip_addr_wraddr03<=16'b0;
		ip_addr_wrvalue03<=8'b0;
		ip_addr_wren04<=1'b0;
		ip_addr_wraddr04<=16'b0;
		ip_addr_wrvalue04<=8'b0;		
		ip_addr_wren05<=1'b0;
		ip_addr_wraddr05<=16'b0;
		ip_addr_wrvalue05<=8'b0;
		ip_addr_wren06<=1'b0;
		ip_addr_wraddr06<=16'b0;
		ip_addr_wrvalue06<=8'b0;		
		ip_addr_wren07<=1'b0;
		ip_addr_wraddr07<=16'b0;
		ip_addr_wrvalue07<=8'b0;
			if(ip_addr_time_light_in_empty==1'b0)begin
				ip_addr_time_light_in_rd<=1'b1;	
				current_state<=write_data_s;
			end
			else begin
				current_state<=idle_s;
			end
		end

	write_data_s:begin
	case(ip_addr_time_light_in_q[34:32])
	3'b0:begin
			ip_addr_time_light_in_rd<=1'b0;	
			ip_addr_wren00<=1'b0;
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren00<=1'b1;
				ip_addr_wraddr00<=ip_addr_time_light_in_q[63:48];
				ip_addr_wrvalue00<=ip_addr_rdvalue00+ip_addr_time_light_in_q[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				ip_addr_time_light_in_q_r<=ip_addr_time_light_in_q;
				current_state<=write_data_s_r;
			end
	end
	3'b001:begin
			ip_addr_time_light_in_rd<=1'b0;	
			ip_addr_wren01<=1'b0;
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren01<=1'b1;
				ip_addr_wraddr01<=ip_addr_time_light_in_q[63:48];
				ip_addr_wrvalue01<=ip_addr_rdvalue01+ip_addr_time_light_in_q[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				ip_addr_time_light_in_q_r<=ip_addr_time_light_in_q;
				current_state<=write_data_s_r;
			end	
	
	
	
	end	
	
	3'b010:begin
			ip_addr_time_light_in_rd<=1'b0;	
			ip_addr_wren02<=1'b0;
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren02<=1'b1;
				ip_addr_wraddr02<=ip_addr_time_light_in_q[63:48];
				ip_addr_wrvalue02<=ip_addr_rdvalue02+ip_addr_time_light_in_q[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				ip_addr_time_light_in_q_r<=ip_addr_time_light_in_q;
				current_state<=write_data_s_r;
			end	
	
	end	
	3'b011:begin
				ip_addr_time_light_in_rd<=1'b0;	
			ip_addr_wren03<=1'b0;
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren03<=1'b1;
				ip_addr_wraddr03<=ip_addr_time_light_in_q[63:48];
				ip_addr_wrvalue03<=ip_addr_rdvalue03+ip_addr_time_light_in_q[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				ip_addr_time_light_in_q_r<=ip_addr_time_light_in_q;
				current_state<=write_data_s_r;
			end	
	end
	3'b100:begin
				ip_addr_time_light_in_rd<=1'b0;	
			ip_addr_wren04<=1'b0;
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren04<=1'b1;
				ip_addr_wraddr04<=ip_addr_time_light_in_q[63:48];
				ip_addr_wrvalue04<=ip_addr_rdvalue04+ip_addr_time_light_in_q[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				ip_addr_time_light_in_q_r<=ip_addr_time_light_in_q;
				current_state<=write_data_s_r;
			end	
	end	
	3'b101:begin
				ip_addr_time_light_in_rd<=1'b0;	
			ip_addr_wren05<=1'b0;
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren05<=1'b1;
				ip_addr_wraddr05<=ip_addr_time_light_in_q[63:48];
				ip_addr_wrvalue05<=ip_addr_rdvalue05+ip_addr_time_light_in_q[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				ip_addr_time_light_in_q_r<=ip_addr_time_light_in_q;
				current_state<=write_data_s_r;
			end	
	end	
	
	3'b110:begin
				ip_addr_time_light_in_rd<=1'b0;	
			ip_addr_wren06<=1'b0;
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren06<=1'b1;
				ip_addr_wraddr06<=ip_addr_time_light_in_q[63:48];
				ip_addr_wrvalue06<=ip_addr_rdvalue06+ip_addr_time_light_in_q[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				ip_addr_time_light_in_q_r<=ip_addr_time_light_in_q;
				current_state<=write_data_s_r;
			end	
	end	
	
	3'b111:begin
			ip_addr_time_light_in_rd<=1'b0;	
			ip_addr_wren07<=1'b0;
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren07<=1'b1;
				ip_addr_wraddr07<=ip_addr_time_light_in_q[63:48];
				ip_addr_wrvalue07<=ip_addr_rdvalue07+ip_addr_time_light_in_q[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				ip_addr_time_light_in_q_r<=ip_addr_time_light_in_q;
				current_state<=write_data_s_r;
			end	
	end	
	
	endcase
	end



	write_data_s_r:begin
	case(ip_addr_time_light_in_q[34:32])
	3'b0:begin
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren00<=1'b1;
				ip_addr_wraddr00<=ip_addr_time_light_in_q_r[63:48];
				ip_addr_wrvalue00<=ip_addr_rdvalue00+ip_addr_time_light_in_q_r[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				current_state<=write_data_s_r;
			end	
	end
	3'b001:begin
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren01<=1'b1;
				ip_addr_wraddr01<=ip_addr_time_light_in_q_r[63:48];
				ip_addr_wrvalue01<=ip_addr_rdvalue01+ip_addr_time_light_in_q_r[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				current_state<=write_data_s_r;
			end
	
	
	
	end	
	
	3'b010:begin
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren02<=1'b1;
				ip_addr_wraddr02<=ip_addr_time_light_in_q_r[63:48];
				ip_addr_wrvalue02<=ip_addr_rdvalue02+ip_addr_time_light_in_q_r[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				current_state<=write_data_s_r;
			end
	
	end	
	3'b011:begin
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren03<=1'b1;
				ip_addr_wraddr03<=ip_addr_time_light_in_q_r[63:48];
				ip_addr_wrvalue03<=ip_addr_rdvalue03+ip_addr_time_light_in_q_r[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				current_state<=write_data_s_r;
			end
	end
	3'b100:begin
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren04<=1'b1;
				ip_addr_wraddr04<=ip_addr_time_light_in_q_r[63:48];
				ip_addr_wrvalue04<=ip_addr_rdvalue04+ip_addr_time_light_in_q_r[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				current_state<=write_data_s_r;
			end
	end	
	3'b101:begin
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren05<=1'b1;
				ip_addr_wraddr05<=ip_addr_time_light_in_q_r[63:48];
				ip_addr_wrvalue05<=ip_addr_rdvalue05+ip_addr_time_light_in_q_r[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				current_state<=write_data_s_r;
			end
	end	
	
	3'b110:begin
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren06<=1'b1;
				ip_addr_wraddr06<=ip_addr_time_light_in_q_r[63:48];
				ip_addr_wrvalue06<=ip_addr_rdvalue06+ip_addr_time_light_in_q_r[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				current_state<=write_data_s_r;
			end	end	
	
	3'b111:begin
			if(read_time-ip_addr_time_light_in_q[127:64]>=64'd3) begin
				ip_addr_wren07<=1'b1;
				ip_addr_wraddr07<=ip_addr_time_light_in_q_r[63:48];
				ip_addr_wrvalue07<=ip_addr_rdvalue07+ip_addr_time_light_in_q_r[7:0];				
				if(ip_addr_time_light_in_empty==1'b0)begin
					ip_addr_time_light_in_rd<=1'b1;	
					current_state<=write_data_s;
				end
				else begin
					current_state<=idle_s;
				end				
			end
			else begin
				current_state<=write_data_s_r;
			end
	end	
	
	endcase
	end

		
		default:current_state<=idle_s;
	endcase
	end
end

 fifo_128_512 light_part_fifo(
	.aclr(!reset),
	.clock(clk),
	.data(ip_addr_time_light_in),
	.rdreq(ip_addr_time_light_in_rd),
	.wrreq(ip_addr_time_light_in_wr),
	.empty(ip_addr_time_light_in_empty),
	.q(ip_addr_time_light_in_q),
	.usedw(ip_addr_time_light_in_usedw) 
 );
 endmodule



			