webtalk_init -webtalk_dir H:\\workspace_xilinx\\tracerSetupExample\\webtalk
webtalk_register_client -client project
webtalk_add_data -client project -key date_generated -value "2018-01-12 11:01:29" -context "software_version_and_target_device"
webtalk_add_data -client project -key product_version -value "SDK v2017.2" -context "software_version_and_target_device"
webtalk_add_data -client project -key build_version -value "2017.2" -context "software_version_and_target_device"
webtalk_add_data -client project -key os_platform -value "amd64" -context "software_version_and_target_device"
webtalk_add_data -client project -key registration_id -value "210806966_1777511497_210657828_120" -context "software_version_and_target_device"
webtalk_add_data -client project -key tool_flow -value "SDK" -context "software_version_and_target_device"
webtalk_add_data -client project -key beta -value "false" -context "software_version_and_target_device"
webtalk_add_data -client project -key route_design -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key target_family -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key target_device -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key target_package -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key target_speed -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key random_id -value "4ghahs79n0ftnlm3sfimep8m6e" -context "software_version_and_target_device"
webtalk_add_data -client project -key project_id -value "2017.2_2" -context "software_version_and_target_device"
webtalk_add_data -client project -key project_iteration -value "2" -context "software_version_and_target_device"
webtalk_add_data -client project -key os_name -value "Microsoft Windows 7 , 64-bit" -context "user_environment"
webtalk_add_data -client project -key os_release -value "Service Pack 1  (build 7601)" -context "user_environment"
webtalk_add_data -client project -key cpu_name -value "Intel(R) Core(TM) i5-4570 CPU @ 3.20GHz" -context "user_environment"
webtalk_add_data -client project -key cpu_speed -value "3192 MHz" -context "user_environment"
webtalk_add_data -client project -key total_processors -value "1" -context "user_environment"
webtalk_add_data -client project -key system_ram -value "8.000 GB" -context "user_environment"
webtalk_register_client -client sdk
webtalk_add_data -client sdk -key uid -value "1515673087121" -context "sdk\\\\bsp/1515673087121"
webtalk_add_data -client sdk -key hwid -value "1513613948000" -context "sdk\\\\bsp/1515673087121"
webtalk_add_data -client sdk -key os -value "standalone" -context "sdk\\\\bsp/1515673087121"
webtalk_add_data -client sdk -key apptemplate -value "empty_application" -context "sdk\\\\bsp/1515673087121"
webtalk_add_data -client sdk -key RecordType -value "BSPCreation" -context "sdk\\\\bsp/1515673087121"
webtalk_add_data -client sdk -key uid -value "1515673102169" -context "sdk\\\\application/1515673102169"
webtalk_add_data -client sdk -key hwid -value "1513613948000" -context "sdk\\\\application/1515673102169"
webtalk_add_data -client sdk -key bspid -value "1515673087121" -context "sdk\\\\application/1515673102169"
webtalk_add_data -client sdk -key newbsp -value "true" -context "sdk\\\\application/1515673102169"
webtalk_add_data -client sdk -key os -value "standalone" -context "sdk\\\\application/1515673102169"
webtalk_add_data -client sdk -key apptemplate -value "empty_application" -context "sdk\\\\application/1515673102169"
webtalk_add_data -client sdk -key RecordType -value "APPCreation" -context "sdk\\\\application/1515673102169"
webtalk_add_data -client sdk -key LangUsed -value "C" -context "sdk\\\\application/1515673102169"
webtalk_add_data -client sdk -key Procused -value "psu_cortexa53" -context "sdk\\\\application/1515673102169"
webtalk_add_data -client sdk -key uid -value "NA" -context "sdk\\\\bsp/1515751289195"
webtalk_add_data -client sdk -key RecordType -value "ToolUsage" -context "sdk\\\\bsp/1515751289195"
webtalk_add_data -client sdk -key BootgenCount -value "0" -context "sdk\\\\bsp/1515751289195"
webtalk_add_data -client sdk -key DebugCount -value "10" -context "sdk\\\\bsp/1515751289195"
webtalk_add_data -client sdk -key PerfCount -value "0" -context "sdk\\\\bsp/1515751289195"
webtalk_add_data -client sdk -key FlashCount -value "0" -context "sdk\\\\bsp/1515751289195"
webtalk_add_data -client sdk -key CrossTriggCount -value "0" -context "sdk\\\\bsp/1515751289195"
webtalk_add_data -client sdk -key QemuDebugCount -value "0" -context "sdk\\\\bsp/1515751289195"
webtalk_transmit -clientid 3656784588 -regid "210806966_1777511497_210657828_120" -xml H:\\workspace_xilinx\\tracerSetupExample\\webtalk\\usage_statistics_ext_sdk.xml -html H:\\workspace_xilinx\\tracerSetupExample\\webtalk\\usage_statistics_ext_sdk.html -wdm H:\\workspace_xilinx\\tracerSetupExample\\webtalk\\sdk_webtalk.wdm -intro "<H3>SDK Usage Report</H3><BR>"
webtalk_terminate
