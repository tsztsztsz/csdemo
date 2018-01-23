connect -url tcp:127.0.0.1:3121
source C:/Xilinx/SDK/2017.2/scripts/sdk/util/zynqmp_utils.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A1C4B3"} -index 1
reset_apu
targets -set -nocase -filter {name =~"RPU*" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A1C4B3"} -index 1
clear_rpu_reset
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A1C4B3"} -index 1
loadhw -hw H:/workspace_xilinx/tracerSetupExample/TracerSetupUS_wrapper_hw_platform_0/system.hdf -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A1C4B3"} -index 1
source H:/workspace_xilinx/tracerSetupExample/TracerSetupUS_wrapper_hw_platform_0/psu_init.tcl
psu_init
catch {psu_protection}
targets -set -nocase -filter {name =~"*A53*0" && jtag_cable_name =~ "Digilent JTAG-SMT2NC 210308A1C4B3"} -index 1
rst -processor
dow H:/workspace_xilinx/tracerSetupExample/csdemo/Debug/csdemo.elf
configparams force-mem-access 0
bpadd -addr &main
