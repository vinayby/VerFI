set search_path /opt/NangateOpenCellLibrary_PDKv1_3_v2010_12/Liberty/CCS
set target_library "NangateOpenCellLibrary.db"
set link_library "$target_library"

#Define design lib
define_design_lib WORK -path "./work"

#Define top level and arch
set TOPCELL_ENT present_encryptor_top_cm

#Analyze target design
analyze -library WORK -autoread {./rtl/verilog/} > ./logfiles/analyze.log

#Elaborate design
elaborate $TOPCELL_ENT  -library DEFAULT > ./logfiles/elaborate.log

#Link design with the library
link

set_dont_touch [get_nets {*}]
set_dont_touch [get_nets {*/*}]
#Avoid optimization of registers signals
set_dont_touch [get_nets {data_o_valid}]
set_dont_touch [get_ports {rst}]
set_dont_touch [get_nets {round_counter* state* key* key_reg* z5_reg* z6_reg* z7_reg* z8_reg* z9_reg* }]
set_dont_touch [get_nets {*/round_counter* */state* */key* */key_reg* */z5_reg* */z6_reg* */z7_reg* */z8_reg* */z9_reg* }]
set_dont_touch [get_nets {*/*/round_counter* */*/state* */*/key* */*/key_reg* */*/z5_reg* */*/z6_reg* */*/z7_reg* */*/z8_reg* */*/z9_reg* }]
set_dont_touch [get_nets {*/*/*/round_counter* */*/*/state* */*/*/key* */*/*/key_reg* */*/*/z5_reg* */*/*/z6_reg* */*/*/z7_reg* */*/*/z8_reg* */*/*/z9_reg* }]
set_dont_touch [get_nets {*/*/*/*/round_counter* */*/*/*/state* */*/*/*/key* */*/*/*/key_reg* */*/*/*/z5_reg* */*/*/*/z6_reg* */*/*/*/z7_reg* */*/*/*/z8_reg* */*/*/*/z9_reg* }]

set ungroup_keep_original_design true
#Compile
#compile_ultra -Gateclock -no_autoungroup
compile -exact_map > ./logfiles/compile.log

#Verilog netlist
set verilogout_show_unconnected_pins true
set verilogout_single_bit true
write -format verilog -hierarchy -output ./netlist/present_encryptor_top_hierarchy.v

#Compile again to get un grouped netlist
compile -exact_map -ungroup_all > ./logfiles/compile.log

#Verilog netlist
set verilogout_single_bit true
write -format verilog -hierarchy -output ./netlist/present_encryptor_top.v

#Reports
report_timing -transition_time -nets -attributes -nosplit > ./reports/timing.log
report_area -nosplit -hierarchy > ./reports/area.log
report_cell -nosplit > ./reports/cells.rpt
report_power -nosplit -hier > ./reports/power.log
report_reference -nosplit -hierarchy > ./reports/ref.log
report_resources -nosplit -hierarchy > ./reports/resources.log

#Exit
exit

