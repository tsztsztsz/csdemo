################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../source/lscript.ld 

C_SRCS += \
../source/app_to_trace.c \
../source/cs_access_cmnfns.c \
../source/cs_cti_ect.c \
../source/cs_debug_halt.c \
../source/cs_debug_sample.c \
../source/cs_demo_known_boards.c \
../source/cs_etm.c \
../source/cs_etm_v4.c \
../source/cs_init_manage.c \
../source/cs_pmu.c \
../source/cs_reg_access.c \
../source/cs_sw_stim.c \
../source/cs_topology.c \
../source/cs_trace_metadata.c \
../source/cs_trace_sink.c \
../source/cs_trace_source.c \
../source/cs_ts_gen.c \
../source/cs_util_create_snapshot.c \
../source/csregistration.c \
../source/getline.c \
../source/platform.c \
../source/primes.c 

OBJS += \
./source/app_to_trace.o \
./source/cs_access_cmnfns.o \
./source/cs_cti_ect.o \
./source/cs_debug_halt.o \
./source/cs_debug_sample.o \
./source/cs_demo_known_boards.o \
./source/cs_etm.o \
./source/cs_etm_v4.o \
./source/cs_init_manage.o \
./source/cs_pmu.o \
./source/cs_reg_access.o \
./source/cs_sw_stim.o \
./source/cs_topology.o \
./source/cs_trace_metadata.o \
./source/cs_trace_sink.o \
./source/cs_trace_source.o \
./source/cs_ts_gen.o \
./source/cs_util_create_snapshot.o \
./source/csregistration.o \
./source/getline.o \
./source/platform.o \
./source/primes.o 

C_DEPS += \
./source/app_to_trace.d \
./source/cs_access_cmnfns.d \
./source/cs_cti_ect.d \
./source/cs_debug_halt.d \
./source/cs_debug_sample.d \
./source/cs_demo_known_boards.d \
./source/cs_etm.d \
./source/cs_etm_v4.d \
./source/cs_init_manage.d \
./source/cs_pmu.d \
./source/cs_reg_access.d \
./source/cs_sw_stim.d \
./source/cs_topology.d \
./source/cs_trace_metadata.d \
./source/cs_trace_sink.d \
./source/cs_trace_source.d \
./source/cs_ts_gen.d \
./source/cs_util_create_snapshot.d \
./source/csregistration.d \
./source/getline.d \
./source/platform.d \
./source/primes.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM A53 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -I"H:\workspace_xilinx\tracerSetupExample\csdemo\include" -I"H:\workspace_xilinx\tracerSetupExample\csdemo\source" -c -fmessage-length=0 -MT"$@" -I../../csdemo_bsp/psu_cortexa53_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


