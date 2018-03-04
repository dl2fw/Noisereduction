################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../Drivers/CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.o 

C_SRCS += \
../Drivers/CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.c 

OBJS += \
./Drivers/CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.o 

C_DEPS += \
./Drivers/CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/DSP_Lib/Source/CommonTables/%.o: ../Drivers/CMSIS/DSP_Lib/Source/CommonTables/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall -Wdouble-promotion  -g -ggdb -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DCORTEX_M4 -DARM_MATH_CM4 -D__FPU_PRESENT=1 -D__FPU_USED=1 -D__EMBEDDED__ -I"../include" -I../Drivers/STM32F4xx_StdPeriph_Driver/inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Audio_DSP -std=gnu99 -O0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


