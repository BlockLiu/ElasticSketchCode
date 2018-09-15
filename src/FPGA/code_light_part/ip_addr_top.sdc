

#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3

#**************************************************************
# Create Clock
#**************************************************************
create_clock -name {sys_clk} 	-period 6.000 -waveform { 0.000 3.0000 } [get_ports {sys_clk}]

derive_pll_clocks 
derive_clock_uncertainty



