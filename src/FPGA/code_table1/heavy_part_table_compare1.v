///2018.01.21 BY DU Haiying
module heavy_part_table_compare1 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_time_in_wr1,
input	wire	[127:0] ip_addr_time_in1,//[127:64] time,[63:32] ip address ,[31:0] value;the hash vlaue is the ip_addr_time_in1[47:36];
output 	wire	ip_addr_time_in_alf1,

//-------output  addr&data&value to ram1----
input	wire	[95:0] ip_addr_rdvalue1,//[95:64] ip addr,[63:32] value,[31:0] counter
output	reg 	ip_addr_wren1,
output 	reg 	[11:0] ip_addr_wraddr1,
output 	reg 	[95:0] ip_addr_wrvalue1,//[95:64] ip addr,[63:32] value,[31:0] counter
//-------output ip_address&value to table1----
output	reg 	ip_addr_value_out_wr1,
output 	reg 	[95:0]ip_addr_value_out1,//[95:64] ip addr,[63:32] value,[31:0] counter
input 	wire	ip_addr_value_out_alf1
);

//---------regsiters of fifo----------------
reg 	ip_addr_time_in_rd1;
wire	[127:0] ip_addr_time_in_q1;
wire 	ip_addr_time_in_empty1;
wire 	[8:0] ip_addr_time_in_usedw1;
reg 	[127:0] ip_addr_time_in_q_r1;
assign ip_addr_time_in_alf1=ip_addr_time_in_usedw1[8];

//---------regsiters ----------------

reg		 [95:0] ip_addr_wrvalue_reg;

reg 	[63:0] read_time;
reg 	[1:0] current_state;


parameter		idle_s=2'b0,
				write_data_s0=2'b01,
				write_data_s0_r=2'b10;
				

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
		ip_addr_time_in_rd1<=1'b0;
		ip_addr_time_in_q_r1<=128'b0;
		
		ip_addr_wren1<=1'b0;
		ip_addr_wraddr1<=12'b0;
		ip_addr_wrvalue1<=96'b0;
		
		ip_addr_value_out_wr1<=1'b0;
		ip_addr_value_out1<=96'b0;
		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_time_in_rd1<=1'b0;
			ip_addr_time_in_q_r1<=128'b0;
			
			ip_addr_wren1<=1'b0;
			ip_addr_wraddr1<=12'b0;
			ip_addr_wrvalue1<=96'b0;
		
			ip_addr_value_out_wr1<=1'b0;
			ip_addr_value_out1<=96'b0;
			if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
				ip_addr_time_in_rd1<=1'b1;				
			end
			else begin
				current_state<=idle_s;
			end
		end
		
		write_data_s0:begin
			ip_addr_time_in_rd1<=1'b0;					
			ip_addr_wren1<=1'b0;
			ip_addr_value_out_wr1<=1'b0;
			if(read_time-ip_addr_time_in_q1[127:64]==64'd3) begin
				if((ip_addr_rdvalue1==96'b0)||(ip_addr_rdvalue1[95:64]==ip_addr_time_in_q1[63:32])) begin
					ip_addr_wren1<=1'b1;
					ip_addr_wraddr1<=ip_addr_time_in_q1[47:36];
						ip_addr_wrvalue1[95:64]<=ip_addr_time_in_q1[63:32];
						ip_addr_wrvalue1[63:32] <=ip_addr_rdvalue1[63:32]+ip_addr_time_in_q1[31:0];
						ip_addr_wrvalue1[31:0]<=ip_addr_rdvalue1[31:0];					
				//	ip_addr_wrvalue1<={ip_addr_time_in_q1[63:0],32'd0};				
					//ip_addr_wrvalue_reg<={ip_addr_time_in_q1[63:0],32'd0};
					if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
						ip_addr_time_in_rd1<=1'b1;
						current_state<=write_data_s0;
					end
					else begin
						current_state<=idle_s;
					end
				end
				else begin
//					if(ip_addr_rdvalue1[95:64]==ip_addr_time_in_q1[63:32]) begin						
//						ip_addr_wren1<=1'b1;
//						ip_addr_wraddr1<=ip_addr_time_in_q1[47:36];	
//						ip_addr_wrvalue1[95:64]<=ip_addr_rdvalue1[95:64];
//						ip_addr_wrvalue1[63:32] <=ip_addr_rdvalue1[63:32]+ip_addr_time_in_q1[31:0];
//						ip_addr_wrvalue1[31:0]<=ip_addr_rdvalue1[31:0];
//						
//						ip_addr_wrvalue_reg[95:64] <=ip_addr_rdvalue1[95:64];
//						ip_addr_wrvalue_reg[63:31] <=ip_addr_rdvalue1[63:32]+ip_addr_time_in_q1[31:0];	
//						ip_addr_wrvalue_reg[31:0] <=ip_addr_rdvalue1[31:0];	
////						if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
////							ip_addr_time_in_rd1<=1'b1;
////							current_state<=write_data_s0;
////						end
////						else begin
//							current_state<=idle_s;
////						end
//					end
//					else begin
						if(ip_addr_rdvalue1[31:0]+ip_addr_time_in_q1[31:0]>=ip_addr_rdvalue1[63:32]<<3) begin
							ip_addr_wren1<=1'b1;
							ip_addr_wraddr1<=ip_addr_time_in_q1[47:36];							
							ip_addr_wrvalue1<={ip_addr_time_in_q1[63:0],32'd0};
							ip_addr_wrvalue_reg<={ip_addr_time_in_q1[63:0],32'd0};
							
							ip_addr_value_out_wr1<=1'b1;
							ip_addr_value_out1<={ip_addr_rdvalue1[95:32],32'd0};
							if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
								ip_addr_time_in_rd1<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end					
						end
						else begin
							ip_addr_wren1<=1'b1;
							ip_addr_wraddr1<=ip_addr_time_in_q1[47:36];							
							ip_addr_wrvalue1<={ip_addr_rdvalue1[95:32],ip_addr_rdvalue1[31:0]+ip_addr_time_in_q1[31:0]};
							ip_addr_wrvalue_reg<={ip_addr_rdvalue1[95:32],ip_addr_rdvalue1[31:0]+ip_addr_time_in_q1[31:0]};
							
							ip_addr_value_out_wr1<=1'b1;
							ip_addr_value_out1<={ip_addr_time_in_q1[63:0],32'd0};
							if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
								ip_addr_time_in_rd1<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end												
						end										
				//	end
				end
			end
			else if(read_time-ip_addr_time_in_q1[127:64]<64'd3) begin
				ip_addr_time_in_q_r1<=ip_addr_time_in_q1;
				current_state<=write_data_s0_r;
			end
			else begin			
				ip_addr_value_out_wr1<=1'b1;
				ip_addr_value_out1<={ip_addr_time_in_q1[63:0],32'd0};
				if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
					ip_addr_time_in_rd1<=1'b1;
					current_state<=write_data_s0;
				end
				else begin
					current_state<=idle_s;
				end
			end		
		end
		
		write_data_s0_r:begin
			if(read_time-ip_addr_time_in_q_r1[127:64]==64'd3) begin
				if((ip_addr_rdvalue1==96'b0)||(ip_addr_rdvalue1[95:64]==ip_addr_time_in_q_r1[63:32])) begin
					ip_addr_wren1<=1'b1;
					ip_addr_wraddr1<=ip_addr_time_in_q_r1[47:36];
					//ip_addr_wrvalue1<={ip_addr_time_in_q_r1[63:0],32'd0};				
					//ip_addr_wrvalue_reg<={ip_addr_time_in_q_r1[63:0],32'd0};
						ip_addr_wrvalue1[95:64]<=ip_addr_time_in_q_r1[63:32];
						ip_addr_wrvalue1[63:32] <=ip_addr_rdvalue1[63:32]+ip_addr_time_in_q_r1[31:0];
						ip_addr_wrvalue1[31:0]<=ip_addr_rdvalue1[31:0];					
					if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
						ip_addr_time_in_rd1<=1'b1;
						current_state<=write_data_s0;
					end
					else begin
						current_state<=idle_s;
					end
				end
				else begin
//					if(ip_addr_rdvalue1[95:64]==ip_addr_time_in_q_r1[63:32]) begin						
//						ip_addr_wren1<=1'b1;
//						ip_addr_wraddr1<=ip_addr_time_in_q_r1[47:36];	
//						ip_addr_wrvalue1[95:64]<=ip_addr_rdvalue1[95:64];
//						ip_addr_wrvalue1[63:32] <=ip_addr_rdvalue1[63:32]+ip_addr_time_in_q_r1[31:0];
//						ip_addr_wrvalue1[31:0]<=ip_addr_rdvalue1[31:0];
//						
//						ip_addr_wrvalue_reg[95:64] <=ip_addr_rdvalue1[95:64];
//						ip_addr_wrvalue_reg[63:31] <=ip_addr_rdvalue1[63:32]+ip_addr_time_in_q_r1[31:0];	
//						ip_addr_wrvalue_reg[31:0] <=ip_addr_rdvalue1[31:0];	
////						if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
////							ip_addr_time_in_rd1<=1'b1;
////							current_state<=write_data_s0;
////						end
////						else begin
//							current_state<=idle_s;
////						end
//					end
//					else begin
						if(ip_addr_rdvalue1[31:0]+ip_addr_time_in_q_r1[31:0]>=ip_addr_rdvalue1[63:32]<<3) begin
							ip_addr_wren1<=1'b1;
							ip_addr_wraddr1<=ip_addr_time_in_q_r1[47:36];							
							ip_addr_wrvalue1<={ip_addr_time_in_q_r1[63:0],32'd0};
							ip_addr_wrvalue_reg<={ip_addr_time_in_q_r1[63:0],32'd0};
							
							ip_addr_value_out_wr1<=1'b1;
							ip_addr_value_out1<={ip_addr_rdvalue1[95:32],32'd0};
							if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
								ip_addr_time_in_rd1<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end					
						end
						else begin
							ip_addr_wren1<=1'b1;
							ip_addr_wraddr1<=ip_addr_time_in_q_r1[47:36];							
							ip_addr_wrvalue1<={ip_addr_rdvalue1[95:32],ip_addr_rdvalue1[31:0]+ip_addr_time_in_q_r1[31:0]};
							ip_addr_wrvalue_reg<={ip_addr_rdvalue1[95:32],ip_addr_rdvalue1[31:0]+ip_addr_time_in_q_r1[31:0]};
							
							ip_addr_value_out_wr1<=1'b1;
							ip_addr_value_out1<={ip_addr_time_in_q_r1[63:0],32'd0};
							if((ip_addr_time_in_empty1==1'b0)&&(ip_addr_value_out_alf1==1'b0))begin
								ip_addr_time_in_rd1<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end												
						end										
				//	end
				end				
			end
			else begin
				current_state<=write_data_s0_r;
			end				
		end		
		default:current_state<=idle_s;
	endcase
	end
end

 fifo_128_512 heavy_part1_fifo(
	.aclr(!reset),
	.clock(clk),
	.data(ip_addr_time_in1),
	.rdreq(ip_addr_time_in_rd1),
	.wrreq(ip_addr_time_in_wr1),
	.empty(ip_addr_time_in_empty1),
	.q(ip_addr_time_in_q1),
	.usedw(ip_addr_time_in_usedw1) 
 );
 endmodule



			