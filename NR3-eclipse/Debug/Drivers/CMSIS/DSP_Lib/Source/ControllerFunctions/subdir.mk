################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_f32.o \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_q15.o \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_q31.o \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_f32.o \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_q15.o \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_q31.o \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_sin_cos_f32.o \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_sin_cos_q31.o 

C_SRCS += \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_f32.c \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_q15.c \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_q31.c \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_f32.c \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_q15.c \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_q31.c \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_sin_cos_f32.c \
../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_sin_cos_q31.c 

OBJS += \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_f32.o \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_q15.o \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_q31.o \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_f32.o \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_q15.o \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_q31.o \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_sin_cos_f32.o \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_sin_cos_q31.o 

C_DEPS += \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_f32.d \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_q15.d \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_init_q31.d \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_f32.d \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_q15.d \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_pid_reset_q31.d \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_sin_cos_f32.d \
./Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/arm_sin_cos_q31.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/%.o: ../Drivers/CMSIS/DSP_Lib/Source/ControllerFunctions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fsingle-precision-constant -Wall -Wno-unused-function  -g -ggdb -DSTM32F40_41xxx -DCORTEX_M4 -D__FPU_PRESENT=1 -D__FPU_USED=1 -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -D__EMBEDDED__ -I"../include" -I../Drivers/STM32F4xx_StdPeriph_Driver/inc -I../Drivers/Project/STM32F4xx_StdPeriph_Templates -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


