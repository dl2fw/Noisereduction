################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/DSP_Lib/Examples/arm_fft_bin_example/arm_fft_bin_data.c \
../Drivers/CMSIS/DSP_Lib/Examples/arm_fft_bin_example/arm_fft_bin_example_f32.c 

OBJS += \
./Drivers/CMSIS/DSP_Lib/Examples/arm_fft_bin_example/arm_fft_bin_data.o \
./Drivers/CMSIS/DSP_Lib/Examples/arm_fft_bin_example/arm_fft_bin_example_f32.o 

C_DEPS += \
./Drivers/CMSIS/DSP_Lib/Examples/arm_fft_bin_example/arm_fft_bin_data.d \
./Drivers/CMSIS/DSP_Lib/Examples/arm_fft_bin_example/arm_fft_bin_example_f32.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/DSP_Lib/Examples/arm_fft_bin_example/%.o: ../Drivers/CMSIS/DSP_Lib/Examples/arm_fft_bin_example/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DSTM32F40_41xxx -D__FPU_PRESENT=1 -D__FPU_USED=1 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F407xx -DHSE_VALUE=8000000 -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -I"../include" -I../Drivers/STM32F4xx_StdPeriph_Driver/inc -I../Drivers/Project/STM32F4xx_StdPeriph_Templates -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


