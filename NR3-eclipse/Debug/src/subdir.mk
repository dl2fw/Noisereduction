################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/debugblinky.c \
../src/fifo.c \
../src/init.c \
../src/main.c \
../src/sm1000_leds_switches.c \
../src/stm32f4_adc.c \
../src/stm32f4_dac.c \
../src/system_stm32f4xx.c 

S_UPPER_SRCS += \
../src/startup_stm32f4xx.S 

OBJS += \
./src/debugblinky.o \
./src/fifo.o \
./src/init.o \
./src/main.o \
./src/sm1000_leds_switches.o \
./src/startup_stm32f4xx.o \
./src/stm32f4_adc.o \
./src/stm32f4_dac.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/debugblinky.d \
./src/fifo.d \
./src/init.d \
./src/main.d \
./src/sm1000_leds_switches.d \
./src/stm32f4_adc.d \
./src/stm32f4_dac.d \
./src/system_stm32f4xx.d 

S_UPPER_DEPS += \
./src/startup_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fsingle-precision-constant -Wall -Wno-unused-function  -g -ggdb -DSTM32F40_41xxx -DCORTEX_M4 -D__FPU_PRESENT=1 -D__FPU_USED=1 -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -D__EMBEDDED__ -I"../include" -I../Drivers/STM32F4xx_StdPeriph_Driver/inc -I../Drivers/Project/STM32F4xx_StdPeriph_Templates -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fsingle-precision-constant -Wall -Wno-unused-function  -g -ggdb -x assembler-with-cpp -DSTM32F40_41xxx -D__FPU_PRESENT=1 -D__FPU_USED=1 -DCORTEX_M4 -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -D__EMBEDDED__ -I"../include" -I../Drivers/STM32F4xx_StdPeriph_Driver/inc -I../Drivers/Project/STM32F4xx_StdPeriph_Templates -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


