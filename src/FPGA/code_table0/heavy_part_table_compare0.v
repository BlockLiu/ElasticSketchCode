///2018.01.20 BY DU Haiying
module heavy_part_table_compare0 (
//--------clk&reset----------------
input	wire	clk,  //clock
input	wire 	reset,	//reset
//-------input from module heavy_part_table_read0----
input	wire	ip_addr_time_in_wr0,
input	wire	[107:0] ip_addr_time_in0, //[95:32] time,[31:0] ip address ;the hash vlaue is the ip_addr_time_in0[11:0]
output 	wire	ip_addr_time_in_alf0,

//-------output  time&addr&data to ram0----
input	wire	[95:0] ip_addr_rdvalue0,//[95:64] ip addr,[63:32] value,[31:0] counter
output	reg 	ip_addr_wren0,
output 	reg 	[11:0] ip_addr_wraddr0,
output 	reg 	[95:0] ip_addr_wrvalue0,//[95:64] ip addr,[63:32] value,[31:0] counter
//-------output ip_address&value to table1----
output	reg 	ip_addr_value_out_wr0,
output 	reg 	[95:0]ip_addr_value_out0,//[95:64] ip addr,[63:32] value,[31:0] counter
input 	wire	ip_addr_value_out_alf0

);

//---------regsiters of fifo----------------
reg 	ip_addr_time_in_rd0;
wire	[107:0] ip_addr_time_in_q0; //[95:32] time,[31:0] ip address ;the hash vlaue is the ip_addr_time_in0[11:0]
wire 	ip_addr_time_in_empty0;
wire 	[8:0] ip_addr_time_in_usedw0;
reg 	[107:0] ip_addr_time_in_q_r0;	 //[95:32] time,[31:0] ip address ;the hash vlaue is the ip_addr_time_in0[11:0]
reg 	[95:0] ip_addr_time_in_q_r0_reg;
assign ip_addr_time_in_alf0=ip_addr_time_in_usedw0[8];

//---------regsiters ----------------
reg		[95:0]ip_addr_wrvalue_reg;
reg 	[63:0] read_time;
reg 	[1:0] time_cnt;

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
		ip_addr_time_in_rd0<=1'b0;
		ip_addr_time_in_q_r0<=108'b0;
		ip_addr_time_in_q_r0_reg<=96'b0;
		
		ip_addr_wren0<=1'b0;
		ip_addr_wraddr0<=12'b0;
		ip_addr_wrvalue0<=96'b0;
		time_cnt<=2'b0;
		ip_addr_value_out_wr0<=1'b0;
		ip_addr_value_out0<=96'b0;
		
		current_state<=idle_s;
	end
	else begin
	case(current_state)	
		idle_s:begin 
			ip_addr_time_in_rd0<=1'b0;
			ip_addr_time_in_q_r0<=108'b0;
			ip_addr_time_in_q_r0_reg<=96'b0;
			time_cnt<=2'b0;
			ip_addr_wren0<=1'b0;
			ip_addr_wraddr0<=12'b0;
			ip_addr_wrvalue0<=96'b0;
		
			ip_addr_value_out_wr0<=1'b0;
			ip_addr_value_out0<=96'b0;
			if((ip_addr_time_in_empty0==1'b0)&&(ip_addr_value_out_alf0==1'b0))begin
				ip_addr_time_in_rd0<=1'b1;	
				current_state<=write_data_s0;
			end
			else begin
				current_state<=idle_s;
			end
		end

				write_data_s0:begin
			ip_addr_time_in_rd0<=1'b0;	
			ip_addr_wren0<=1'b0;
			ip_addr_value_out_wr0<=1'b0;
			if(read_time-ip_addr_time_in_q0[107:44]==64'd3) begin
				if((ip_addr_rdvalue0==96'b0)||(ip_addr_rdvalue0[95:64]==ip_addr_time_in_q0[43:12])) begin
					ip_addr_wren0<=1'b1;
					ip_addr_wraddr0<=ip_addr_time_in_q0[11:0];
					ip_addr_wrvalue0[95:64] <=ip_addr_time_in_q0[43:12];
					ip_addr_wrvalue0[63:32] <=ip_addr_rdvalue0[63:32]+32'd1;
					ip_addr_wrvalue0[31:0] <=ip_addr_rdvalue0[31:0];					
					if((ip_addr_time_in_empty0==1'b0)&&(ip_addr_value_out_alf0==1'b0))begin
						ip_addr_time_in_rd0<=1'b1;
						current_state<=write_data_s0;
					end
					else begin
						current_state<=idle_s;
					end
				end
				else begin
	
						if(ip_addr_rdvalue0[31:0]+32'd1>=ip_addr_rdvalue0[63:32]<<3) begin
							ip_addr_wren0<=1'b1;
							ip_addr_wraddr0<=ip_addr_time_in_q0[11:0];							
							ip_addr_wrvalue0<={ip_addr_time_in_q0[43:12],32'd1,32'd0};
//							ip_addr_wrvalue_reg<={ip_addr_time_in_q0[31:0],32'd1,32'd0};
							
							ip_addr_value_out_wr0<=1'b1;
							ip_addr_value_out0<={ip_addr_rdvalue0[95:32],32'd0};
							if((ip_addr_time_in_empty0==1'b0)&&(ip_addr_value_out_alf0==1'b0))begin
								ip_addr_time_in_rd0<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end					
						end
						else begin
							ip_addr_wren0<=1'b1;
							ip_addr_wraddr0<=ip_addr_time_in_q0[11:0];							
							ip_addr_wrvalue0<={ip_addr_rdvalue0[95:32],ip_addr_rdvalue0[31:0]+32'd1};
//							ip_addr_wrvalue_reg<={ip_addr_rdvalue0[95:32],ip_addr_rdvalue0[31:0]+32'd1};
							
							ip_addr_value_out_wr0<=1'b1;
							ip_addr_value_out0<={ip_addr_time_in_q0[43:12],32'd1,32'd0};
							if((ip_addr_time_in_empty0==1'b0)&&(ip_addr_value_out_alf0==1'b0))begin
								ip_addr_time_in_rd0<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
								current_state<=idle_s;
							end												
						end										
					//end
				end
			end
			else if(read_time-ip_addr_time_in_q0[107:44]<64'd3) begin
				ip_addr_time_in_q_r0<=ip_addr_time_in_q0;
				time_cnt<=read_time-ip_addr_time_in_q0[107:44]+64'd1;
				current_state<=write_data_s0_r;
			end
			else begin			
				ip_addr_value_out_wr0<=1'b1;
				ip_addr_value_out0<={ip_addr_time_in_q0[43:12],32'd1,32'd0};
				if((ip_addr_time_in_empty0==1'b0)&&(ip_addr_value_out_alf0==1'b0))begin
					ip_addr_time_in_rd0<=1'b1;
					current_state<=write_data_s0;
				end
				else begin
					current_state<=idle_s;
				end
			end		
		end
		
		write_data_s0_r:begin
			case(time_cnt)
			2'd0:begin
				time_cnt<=time_cnt+2'd1;
				current_state<=write_data_s0_r;
			end
			2'd1:begin
				time_cnt<=time_cnt+2'd1;
				current_state<=write_data_s0_r;
			end
			2'd2:begin
				time_cnt<=time_cnt+2'd1;
				current_state<=write_data_s0_r;
			end
			2'd3:begin			
				if((ip_addr_rdvalue0==96'b0)||(ip_addr_rdvalue0[95:64]==ip_addr_time_in_q_r0[43:12])) begin				
					ip_addr_wren0<=1'b1;
					ip_addr_wraddr0<=ip_addr_time_in_q_r0[11:0];
					ip_addr_wrvalue0[95:64] <=ip_addr_time_in_q_r0[43:12];						
					ip_addr_wrvalue0[63:32] <=ip_addr_rdvalue0[63:32]+32'd1;
					ip_addr_wrvalue0[31:0] <=ip_addr_rdvalue0[31:0];
//					ip_addr_wrvalue_reg[95:64] <=ip_addr_time_in_q_r0[31:0];
//					ip_addr_wrvalue_reg[63:31] <=ip_addr_rdvalue0[63:32]+32'd1;	
//					ip_addr_wrvalue_reg[31:0] <=ip_addr_rdvalue0[31:0];
					time_cnt<=2'b0;
					if((ip_addr_time_in_empty0==1'b0)&&(ip_addr_value_out_alf0==1'b0))begin
						ip_addr_time_in_rd0<=1'b1;
						current_state<=write_data_s0;
					end
					else begin
						
						current_state<=idle_s;
					end
				end
				else begin
					if(ip_addr_rdvalue0[31:0]+32'd1>=ip_addr_rdvalue0[63:32]<<3) begin
						ip_addr_wren0<=1'b1;
						ip_addr_wraddr0<=ip_addr_time_in_q_r0[11:0];							
						ip_addr_wrvalue0<={ip_addr_time_in_q_r0[43:12],32'd1,32'd0};
//						ip_addr_wrvalue_reg<={ip_addr_time_in_q_r0[31:0],32'd1,32'd0};							
						ip_addr_value_out_wr0<=1'b1;
						ip_addr_value_out0<={ip_addr_rdvalue0[95:32],32'd0};
						time_cnt<=2'b0;
							if((ip_addr_time_in_empty0==1'b0)&&(ip_addr_value_out_alf0==1'b0))begin
								ip_addr_time_in_rd0<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
							current_state<=idle_s;
							end					
					end
					else begin
						ip_addr_wren0<=1'b1;
						ip_addr_wraddr0<=ip_addr_time_in_q_r0[11:0];							
						ip_addr_wrvalue0<={ip_addr_rdvalue0[95:32],ip_addr_rdvalue0[31:0]+32'd1};
						ip_addr_wrvalue_reg<={ip_addr_rdvalue0[95:32],ip_addr_rdvalue0[31:0]+32'd1};
						
						ip_addr_value_out_wr0<=1'b1;
						ip_addr_value_out0<={ip_addr_time_in_q_r0[43:12],32'd1,32'd0};
						time_cnt<=2'b0;
							if((ip_addr_time_in_empty0==1'b0)&&(ip_addr_value_out_alf0==1'b0))begin
								ip_addr_time_in_rd0<=1'b1;
								current_state<=write_data_s0;
							end
							else begin
							current_state<=idle_s;
						end												
					end										
				
				end						
			end
			
			endcase
		end
		

		
		
		
	default:current_state<=idle_s;
	endcase
	end
end
 fifo_108_512 heavy_part0_fifo(
	.aclr(!reset),
	.clock(clk),
	.data(ip_addr_time_in0),
	.rdreq(ip_addr_time_in_rd0),
	.wrreq(ip_addr_time_in_wr0),
	.empty(ip_addr_time_in_empty0),
	.q(ip_addr_time_in_q0),
	.usedw(ip_addr_time_in_usedw0) 
 );
 endmodule




			