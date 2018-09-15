///2018.01.21 BY DU Haiying
module heavy_part_table_compare3 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_time_in_wr3,
input	wire	[127:0] ip_addr_time_in3,//[95:32] time,[31:0] ip address ;the hash vlaue is the ip_addr_time_in3[55:44]
output 	wire	ip_addr_time_in_alf3,

//-------output  time&addr&data to ram3----
input	wire	[95:0] ip_addr_rdvalue3,
output	reg 	ip_addr_wren3,
output 	reg 	[11:0] ip_addr_wraddr3,
output 	reg 	[95:0] ip_addr_wrvalue3,
//-------output ip_address&value to table1----
output	reg 	ip_addr_value_out_wr3,
output 	reg 	[95:0]ip_addr_value_out3,
input 	wire	ip_addr_value_out_alf3

);

//---------regsiters of fifo----------------
reg 	ip_addr_time_in_rd3;
wire	[127:0] ip_addr_time_in_q3;
wire 	ip_addr_time_in_empty3;
wire 	[8:0] ip_addr_time_in_usedw3;
reg 	[127:0] ip_addr_time_in_q_r3;
assign ip_addr_time_in_alf3=ip_addr_time_in_usedw3[8];

//---------regsiters ----------------

reg [95:0] ip_addr_wrvalue_reg;


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
		ip_addr_time_in_rd3<=1'b0;
		ip_addr_time_in_q_r3<=128'b0;
		
		ip_addr_wren3<=1'b0;
		ip_addr_wraddr3<=12'b0;
		ip_addr_wrvalue3<=96'b0;
		
		ip_addr_value_out_wr3<=1'b0;
		ip_addr_value_out3<=96'b0;
		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_time_in_rd3<=1'b0;
			ip_addr_time_in_q_r3<=128'b0;
			
			ip_addr_wren3<=1'b0;
			ip_addr_wraddr3<=12'b0;
			ip_addr_wrvalue3<=96'b0;
		
			ip_addr_value_out_wr3<=1'b0;
			ip_addr_value_out3<=96'b0;
			if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
				ip_addr_time_in_rd3<=1'b1;	
				current_state<=write_data_s0;			
			end
			else begin
				current_state<=idle_s;
			end
		end
		
		write_data_s0:begin
			ip_addr_time_in_rd3<=1'b0;
			ip_addr_wren3<=1'b0;	
			ip_addr_value_out_wr3<=1'b0;
			if(read_time-ip_addr_time_in_q3[127:64]==64'd3) begin
				if((ip_addr_rdvalue3==96'b0)||(ip_addr_rdvalue3[95:64]==ip_addr_time_in_q3[63:32]))begin
					ip_addr_wren3<=1'b1;
					ip_addr_wraddr3<=ip_addr_time_in_q3[55:44];
				//	ip_addr_wrvalue3<={ip_addr_time_in_q3[63:0],32'd0};				
				//	ip_addr_wrvalue_reg<={ip_addr_time_in_q3[63:0],32'd0};
						ip_addr_wrvalue3[95:64]<=ip_addr_time_in_q3[63:32];
						ip_addr_wrvalue3[63:32] <=ip_addr_rdvalue3[63:32]+ip_addr_time_in_q3[31:0];
						ip_addr_wrvalue3[31:0]<=ip_addr_rdvalue3[31:0];				
					if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
						ip_addr_time_in_rd3<=1'b1;
						current_state<=write_data_s0;
					end
					else begin
						current_state<=idle_s;
					end
				end
				else begin
//					if(ip_addr_rdvalue3[95:64]==ip_addr_time_in_q3[63:32]) begin						
//						ip_addr_wren3<=1'b1;
//						ip_addr_wraddr3<=ip_addr_time_in_q3[55:44];	
//						ip_addr_wrvalue3[95:64]<=ip_addr_rdvalue3[95:64];
//						ip_addr_wrvalue3[63:32] <=ip_addr_rdvalue3[63:32]+ip_addr_time_in_q3[31:0];
//						ip_addr_wrvalue3[31:0]<=ip_addr_rdvalue3[31:0];
//						
//						ip_addr_wrvalue_reg[95:64] <=ip_addr_rdvalue3[95:64];
//						ip_addr_wrvalue_reg[63:31] <=ip_addr_rdvalue3[63:32]+ip_addr_time_in_q3[31:0];	
//						ip_addr_wrvalue_reg[31:0] <=ip_addr_rdvalue3[31:0];	
////						if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
////							ip_addr_time_in_rd3<=1'b1;
////							current_state<=write_data_s0;
////						end
////						else begin
//							current_state<=idle_s;
////						end
//					end
//					else begin
						if(ip_addr_rdvalue3[31:0]+ip_addr_time_in_q3[31:0]>=ip_addr_rdvalue3[63:32]<<3) begin
							ip_addr_wren3<=1'b1;
							ip_addr_wraddr3<=ip_addr_time_in_q3[55:44];							
							ip_addr_wrvalue3<={ip_addr_time_in_q3[63:0],32'd0};
							ip_addr_wrvalue_reg<={ip_addr_time_in_q3[63:0],32'd0};
							
							ip_addr_value_out_wr3<=1'b1;
							ip_addr_value_out3<={ip_addr_rdvalue3[95:32],32'd0};
							if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
								ip_addr_time_in_rd3<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end					
						end
						else begin
							ip_addr_wren3<=1'b1;
							ip_addr_wraddr3<=ip_addr_time_in_q3[55:44];							
							ip_addr_wrvalue3<={ip_addr_rdvalue3[95:32],ip_addr_rdvalue3[31:0]+ip_addr_time_in_q3[31:0]};
							ip_addr_wrvalue_reg<={ip_addr_rdvalue3[95:32],ip_addr_rdvalue3[31:0]+ip_addr_time_in_q3[31:0]};
							
							ip_addr_value_out_wr3<=1'b1;
							ip_addr_value_out3<={ip_addr_time_in_q3[63:0],32'd0};
							if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
								ip_addr_time_in_rd3<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end												
						end										
					//end
				end
			end
			else if(read_time-ip_addr_time_in_q3[127:64]<64'd3) begin
				ip_addr_time_in_q_r3<=ip_addr_time_in_q3;
				current_state<=write_data_s0_r;
			end
			else begin			
				ip_addr_value_out_wr3<=1'b1;
				ip_addr_value_out3<={ip_addr_time_in_q3[63:0],32'd0};
				if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
					ip_addr_time_in_rd3<=1'b1;
					current_state<=write_data_s0;
				end
				else begin
					current_state<=idle_s;
				end
			end		
		end
		
		write_data_s0_r:begin
			if(read_time-ip_addr_time_in_q_r3[127:64]==64'd3) begin
				if((ip_addr_rdvalue3==96'b0)||(ip_addr_rdvalue3[95:64]==ip_addr_time_in_q_r3[63:32])) begin
					ip_addr_wren3<=1'b1;
					ip_addr_wraddr3<=ip_addr_time_in_q_r3[55:44];
					ip_addr_wrvalue3<={ip_addr_time_in_q_r3[63:0],32'd0};				
					ip_addr_wrvalue_reg<={ip_addr_time_in_q_r3[63:0],32'd0};
						ip_addr_wrvalue3[95:64]<=ip_addr_time_in_q_r3[63:32];
						ip_addr_wrvalue3[63:32] <=ip_addr_rdvalue3[63:32]+ip_addr_time_in_q_r3[31:0];
						ip_addr_wrvalue3[31:0]<=ip_addr_rdvalue3[31:0];					
					if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
						ip_addr_time_in_rd3<=1'b1;
						current_state<=write_data_s0;
					end
					else begin
						current_state<=idle_s;
					end
				end
				else begin
//					if(ip_addr_rdvalue3[95:64]==ip_addr_time_in_q_r3[63:32]) begin						
//						ip_addr_wren3<=1'b1;
//						ip_addr_wraddr3<=ip_addr_time_in_q_r3[55:44];	
//						ip_addr_wrvalue3[95:64]<=ip_addr_rdvalue3[95:64];
//						ip_addr_wrvalue3[63:32] <=ip_addr_rdvalue3[63:32]+ip_addr_time_in_q_r3[31:0];
//						ip_addr_wrvalue3[31:0]<=ip_addr_rdvalue3[31:0];
//						
//						ip_addr_wrvalue_reg[95:64] <=ip_addr_rdvalue3[95:64];
//						ip_addr_wrvalue_reg[63:31] <=ip_addr_rdvalue3[63:32]+ip_addr_time_in_q_r3[31:0];	
//						ip_addr_wrvalue_reg[31:0] <=ip_addr_rdvalue3[31:0];	
////						if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
////							ip_addr_time_in_rd3<=1'b1;
////							current_state<=write_data_s0;
////						end
////						else begin
//							current_state<=idle_s;
////						end
//					end
//					else begin
						if(ip_addr_rdvalue3[31:0]+ip_addr_time_in_q_r3[31:0]>=ip_addr_rdvalue3[63:32]<<3) begin
							ip_addr_wren3<=1'b1;
							ip_addr_wraddr3<=ip_addr_time_in_q_r3[55:44];							
							ip_addr_wrvalue3<={ip_addr_time_in_q_r3[63:0],32'd0};
							ip_addr_wrvalue_reg<={ip_addr_time_in_q_r3[63:0],32'd0};
							
							ip_addr_value_out_wr3<=1'b1;
							ip_addr_value_out3<={ip_addr_rdvalue3[95:32],32'd0};
							if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
								ip_addr_time_in_rd3<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end					
						end
						else begin
							ip_addr_wren3<=1'b1;
							ip_addr_wraddr3<=ip_addr_time_in_q_r3[55:44];							
							ip_addr_wrvalue3<={ip_addr_rdvalue3[95:32],ip_addr_rdvalue3[31:0]+ip_addr_time_in_q_r3[31:0]};
							ip_addr_wrvalue_reg<={ip_addr_rdvalue3[95:32],ip_addr_rdvalue3[31:0]+ip_addr_time_in_q_r3[31:0]};
							
							ip_addr_value_out_wr3<=1'b1;
							ip_addr_value_out3<={ip_addr_time_in_q_r3[63:0],32'd0};
							if((ip_addr_time_in_empty3==1'b0)&&(ip_addr_value_out_alf3==1'b0))begin
								ip_addr_time_in_rd3<=1'b1;
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

 fifo_128_512 heavy_part3_fifo(
	.aclr(!reset),
	.clock(clk),
	.data(ip_addr_time_in3),
	.rdreq(ip_addr_time_in_rd3),
	.wrreq(ip_addr_time_in_wr3),
	.empty(ip_addr_time_in_empty3),
	.q(ip_addr_time_in_q3),
	.usedw(ip_addr_time_in_usedw3) 
 );
 endmodule



			