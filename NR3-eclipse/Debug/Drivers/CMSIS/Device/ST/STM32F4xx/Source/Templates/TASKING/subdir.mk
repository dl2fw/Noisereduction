################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/TASKING/cstart_thumb2.asm 

OBJS += \
./Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/TASKING/cstart_thumb2.o 

ASM_DEPS += \
./Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/TASKING/cstart_thumb2.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/TASKING/%.o: ../Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/TASKING/%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -x assembler-with-cpp -DDEBUG -DSTM32F40_41xxx -D__FPU_PRESENT=1 -D__FPU_USED=1 -DUSE_FULL_ASSERT -DSTM32F407xx -DHSE_VALUE=8000000 -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -I"../include" -I../Drivers/STM32F4xx_StdPeriph_Driver/inc -I../Drivers/Project/STM32F4xx_StdPeriph_Templates -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


