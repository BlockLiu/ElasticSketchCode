///2018.01.20 BY DU Haiying
module heavy_part_table_compare2 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input ip address----
input	wire	ip_addr_time_in_wr2,
input	wire	[127:0] ip_addr_time_in2,
output 	wire	ip_addr_time_in_alf2,

//-------output  time&addr&data to ram1----
input	wire	[95:0] ip_addr_rdvalue2,
output	reg 	ip_addr_wren2,
output 	reg 	[11:0] ip_addr_wraddr2,
output 	reg 	[95:0] ip_addr_wrvalue2,
//-------output ip_address&value to table1----
output	reg 	ip_addr_value_out_wr2,
output 	reg 	[95:0]ip_addr_value_out2,
input 	wire	ip_addr_value_out_alf2

);

//---------regsiters of fifo----------------
reg 	ip_addr_time_in_rd2;
wire	[127:0] ip_addr_time_in_q2;
wire 	ip_addr_time_in_empty2;
wire 	[8:0] ip_addr_time_in_usedw2;
reg 	[127:0] ip_addr_time_in_q_r2;
assign ip_addr_time_in_alf2=ip_addr_time_in_usedw2[8];

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
		ip_addr_time_in_rd2<=1'b0;
		ip_addr_time_in_q_r2<=128'b0;
		
		ip_addr_wren2<=1'b0;
		ip_addr_wraddr2<=12'b0;
		ip_addr_wrvalue2<=96'b0;
		
		ip_addr_value_out_wr2<=1'b0;
		ip_addr_value_out2<=96'b0;
		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_time_in_rd2<=1'b0;
			ip_addr_time_in_q_r2<=128'b0;
			
			ip_addr_wren2<=1'b0;
			ip_addr_wraddr2<=12'b0;
			ip_addr_wrvalue2<=96'b0;
		
			ip_addr_value_out_wr2<=1'b0;
			ip_addr_value_out2<=96'b0;
			if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
				ip_addr_time_in_rd2<=1'b1;		
		current_state<=write_data_s0;		
			end
			else begin
				current_state<=idle_s;
			end
		end
		
		write_data_s0:begin
			ip_addr_time_in_rd2<=1'b0;	
			if(read_time-ip_addr_time_in_q2[127:64]==64'd3) begin
				if((ip_addr_rdvalue2==96'b0)||(ip_addr_rdvalue2[95:64]==ip_addr_time_in_q2[63:32])) begin
					ip_addr_wren2<=1'b1;
					ip_addr_wraddr2<=ip_addr_time_in_q2[51:40];
					//ip_addr_wrvalue2<={ip_addr_time_in_q2[63:0],32'd0};				
					//ip_addr_wrvalue_reg<={ip_addr_time_in_q2[63:0],32'd0};
						ip_addr_wrvalue2[95:64]<=ip_addr_time_in_q2[63:32];
						ip_addr_wrvalue2[63:32] <=ip_addr_rdvalue2[63:32]+ip_addr_time_in_q2[31:0];
						ip_addr_wrvalue2[31:0]<=ip_addr_rdvalue2[31:0];					
					if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
						ip_addr_time_in_rd2<=1'b1;
						current_state<=write_data_s0;
					end
					else begin
						current_state<=idle_s;
					end
				end
				else begin
//					if(ip_addr_rdvalue2[95:64]==ip_addr_time_in_q2[63:32]) begin						
//						ip_addr_wren2<=1'b1;
//						ip_addr_wraddr2<=ip_addr_time_in_q2[51:40];	
//						ip_addr_wrvalue2[95:64]<=ip_addr_rdvalue2[95:64];
//						ip_addr_wrvalue2[63:32] <=ip_addr_rdvalue2[63:32]+ip_addr_time_in_q2[31:0];
//						ip_addr_wrvalue2[31:0]<=ip_addr_rdvalue2[31:0];
//						
//						ip_addr_wrvalue_reg[95:64] <=ip_addr_rdvalue2[95:64];
//						ip_addr_wrvalue_reg[63:31] <=ip_addr_rdvalue2[63:32]+ip_addr_time_in_q2[31:0];	
//						ip_addr_wrvalue_reg[31:0] <=ip_addr_rdvalue2[31:0];	
////						if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
////							ip_addr_time_in_rd2<=1'b1;
////							current_state<=write_data_s0;
////						end
////						else begin
//							current_state<=idle_s;
////						end
//					end
//					else begin
						if(ip_addr_rdvalue2[31:0]+ip_addr_time_in_q2[31:0]>=ip_addr_rdvalue2[63:32]<<3) begin
							ip_addr_wren2<=1'b1;
							ip_addr_wraddr2<=ip_addr_time_in_q2[51:40];							
							ip_addr_wrvalue2<={ip_addr_time_in_q2[63:0],32'd0};
							ip_addr_wrvalue_reg<={ip_addr_time_in_q2[63:0],32'd0};
							
							ip_addr_value_out_wr2<=1'b1;
							ip_addr_value_out2<={ip_addr_rdvalue2[95:32],32'd0};
							if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
								ip_addr_time_in_rd2<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end					
						end
						else begin
							ip_addr_wren2<=1'b1;
							ip_addr_wraddr2<=ip_addr_time_in_q2[51:40];							
							ip_addr_wrvalue2<={ip_addr_rdvalue2[95:32],ip_addr_rdvalue2[31:0]+ip_addr_time_in_q2[31:0]};
							ip_addr_wrvalue_reg<={ip_addr_rdvalue2[95:32],ip_addr_rdvalue2[31:0]+ip_addr_time_in_q2[31:0]};
							
							ip_addr_value_out_wr2<=1'b1;
							ip_addr_value_out2<={ip_addr_time_in_q2[63:0],32'd0};
							if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
								ip_addr_time_in_rd2<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end												
						end										
				//	end
				end
			end
			else if(read_time-ip_addr_time_in_q2[127:64]<64'd3) begin
				ip_addr_time_in_q_r2<=ip_addr_time_in_q2;
				current_state<=write_data_s0_r;
			end
			else begin			
				ip_addr_value_out_wr2<=1'b1;
				ip_addr_value_out2<={ip_addr_time_in_q2[63:0],32'd0};
				if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
					ip_addr_time_in_rd2<=1'b1;
					current_state<=write_data_s0;
				end
				else begin
					current_state<=idle_s;
				end
			end		
		end
		
		write_data_s0_r:begin
			if(read_time-ip_addr_time_in_q_r2[127:64]==64'd3) begin
				if((ip_addr_rdvalue2==96'b0)||(ip_addr_rdvalue2[95:64]==ip_addr_time_in_q_r2[63:32])) begin
					ip_addr_wren2<=1'b1;
					ip_addr_wraddr2<=ip_addr_time_in_q_r2[51:40];
					//ip_addr_wrvalue2<={ip_addr_time_in_q_r2[63:0],32'd0};				
					//ip_addr_wrvalue_reg<={ip_addr_time_in_q_r2[63:0],32'd0};
						ip_addr_wrvalue2[95:64]<=ip_addr_time_in_q_r2[63:32];
						ip_addr_wrvalue2[63:32] <=ip_addr_rdvalue2[63:32]+ip_addr_time_in_q_r2[31:0];
						ip_addr_wrvalue2[31:0]<=ip_addr_rdvalue2[31:0];
					if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
						ip_addr_time_in_rd2<=1'b1;
						current_state<=write_data_s0;
					end
					else begin
						current_state<=idle_s;
					end
				end
				else begin
//					if(ip_addr_rdvalue2[95:64]==ip_addr_time_in_q_r2[63:32]) begin						
//						ip_addr_wren2<=1'b1;
//						ip_addr_wraddr2<=ip_addr_time_in_q_r2[51:40];	
//						ip_addr_wrvalue2[95:64]<=ip_addr_rdvalue2[95:64];
//						ip_addr_wrvalue2[63:32] <=ip_addr_rdvalue2[63:32]+ip_addr_time_in_q_r2[31:0];
//						ip_addr_wrvalue2[31:0]<=ip_addr_rdvalue2[31:0];
//						
//						ip_addr_wrvalue_reg[95:64] <=ip_addr_rdvalue2[95:64];
//						ip_addr_wrvalue_reg[63:31] <=ip_addr_rdvalue2[63:32]+ip_addr_time_in_q_r2[31:0];	
//						ip_addr_wrvalue_reg[31:0] <=ip_addr_rdvalue2[31:0];	
////						if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
////							ip_addr_time_in_rd2<=1'b1;
////							current_state<=write_data_s0;
////						end
////						else begin
//							current_state<=idle_s;
////						end
//					end
//					else begin
						if(ip_addr_rdvalue2[31:0]+ip_addr_time_in_q_r2[31:0]>=ip_addr_rdvalue2[63:32]<<3) begin
							ip_addr_wren2<=1'b1;
							ip_addr_wraddr2<=ip_addr_time_in_q_r2[51:40];							
							ip_addr_wrvalue2<={ip_addr_time_in_q_r2[63:0],32'd0};
							ip_addr_wrvalue_reg<={ip_addr_time_in_q_r2[63:0],32'd0};
							
							ip_addr_value_out_wr2<=1'b1;
							ip_addr_value_out2<={ip_addr_rdvalue2[95:32],32'd0};
							if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
								ip_addr_time_in_rd2<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end					
						end
						else begin
							ip_addr_wren2<=1'b1;
							ip_addr_wraddr2<=ip_addr_time_in_q_r2[51:40];							
							ip_addr_wrvalue2<={ip_addr_rdvalue2[95:32],ip_addr_rdvalue2[31:0]+ip_addr_time_in_q_r2[31:0]};
							ip_addr_wrvalue_reg<={ip_addr_rdvalue2[95:32],ip_addr_rdvalue2[31:0]+ip_addr_time_in_q_r2[31:0]};
							
							ip_addr_value_out_wr2<=1'b1;
							ip_addr_value_out2<={ip_addr_time_in_q_r2[63:0],32'd0};
							if((ip_addr_time_in_empty2==1'b0)&&(ip_addr_value_out_alf2==1'b0))begin
								ip_addr_time_in_rd2<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end												
						end										
					//end
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

 fifo_128_512 heavy_part2_fifo(
	.aclr(!reset),
	.clock(clk),
	.data(ip_addr_time_in2),
	.rdreq(ip_addr_time_in_rd2),
	.wrreq(ip_addr_time_in_wr2),
	.empty(ip_addr_time_in_empty2),
	.q(ip_addr_time_in_q2),
	.usedw(ip_addr_time_in_usedw2) 
 );
 endmodule



			