module half32_16bit(
input	wire			clk,
input	wire			reset,
//-------input ip address----
input	wire	ip_address_in_wr,
input	wire	[31:0] ip_address_in,
//-----------output hash value----//
output	reg			ip_addr_checksum_wr,
output	reg			[43:0]	ip_addr_checksum

);
    
//----------------------ram--------------------------//


/************************************code**********************************/ 
always @ (posedge clk or negedge reset)
begin
    if (~reset)
    begin
        ip_addr_checksum	<={32'b0,12'hfff};
        ip_addr_checksum_wr	<= 1'b0;
    end
    else if(ip_address_in_wr)
    begin//1 cycle, get the value of data
		ip_addr_checksum	<=	{ip_address_in,ip_address_in[11:0]^ip_address_in[23:12]};
		ip_addr_checksum_wr	<=	1'b1;
    end
    else//data transmit is over
    begin
        ip_addr_checksum	<={32'b0,12'hfff};
        ip_addr_checksum_wr	<= 1'b0;
    end
end

endmodule