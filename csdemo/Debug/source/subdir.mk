################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../source/lscript.ld 

C_SRCS += \
../source/app_to_trace.c \
../source/platform.c \
../source/primes.c 

OBJS += \
./source/app_to_trace.o \
./source/platform.o \
./source/primes.o 

C_DEPS += \
./source/app_to_trace.d \
./source/platform.d \
./source/primes.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM A53 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -I"H:\workspace_xilinx\tracerSetupExample\csdemo\include" -I"H:\workspace_xilinx\tracerSetupExample\csdemo\source" -c -fmessage-length=0 -MT"$@" -I../../csdemo_bsp/psu_cortexa53_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


