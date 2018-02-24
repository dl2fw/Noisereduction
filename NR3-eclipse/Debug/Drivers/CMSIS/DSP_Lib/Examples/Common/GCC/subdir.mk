################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/startup_ARMCM0.S \
../Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/startup_ARMCM3.S \
../Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/startup_ARMCM4.S 

OBJS += \
./Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/startup_ARMCM0.o \
./Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/startup_ARMCM3.o \
./Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/startup_ARMCM4.o 

S_UPPER_DEPS += \
./Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/startup_ARMCM0.d \
./Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/startup_ARMCM3.d \
./Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/startup_ARMCM4.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/%.o: ../Drivers/CMSIS/DSP_Lib/Examples/Common/GCC/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -x assembler-with-cpp -DDEBUG -DSTM32F40_41xxx -D__FPU_PRESENT=1 -D__FPU_USED=1 -DUSE_FULL_ASSERT -DSTM32F407xx -DHSE_VALUE=8000000 -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -I"../include" -I../Drivers/STM32F4xx_StdPeriph_Driver/inc -I../Drivers/Project/STM32F4xx_StdPeriph_Templates -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


