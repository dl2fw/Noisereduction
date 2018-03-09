################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/debugblinky.c \
../src/eeprom_access.c \
../src/fifo.c \
../src/main.c \
../src/sm1000_leds_switches.c \
../src/stm32f4_adc.c \
../src/stm32f4_dac.c \
../src/stm32f4_vrom.c \
../src/system_stm32f4xx.c 

S_UPPER_SRCS += \
../src/startup_stm32f4xx.S 

OBJS += \
./src/debugblinky.o \
./src/eeprom_access.o \
./src/fifo.o \
./src/main.o \
./src/sm1000_leds_switches.o \
./src/startup_stm32f4xx.o \
./src/stm32f4_adc.o \
./src/stm32f4_dac.o \
./src/stm32f4_vrom.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/debugblinky.d \
./src/eeprom_access.d \
./src/fifo.d \
./src/main.d \
./src/sm1000_leds_switches.d \
./src/stm32f4_adc.d \
./src/stm32f4_dac.d \
./src/stm32f4_vrom.d \
./src/system_stm32f4xx.d 

S_UPPER_DEPS += \
./src/startup_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall -Wdouble-promotion  -g -ggdb -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DCORTEX_M4 -DARM_MATH_CM4 -D__FPU_PRESENT=1 -D__FPU_USED=1 -D__EMBEDDED__ -I"../include" -I../Drivers/STM32F4xx_StdPeriph_Driver/inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Audio_DSP/inc -I../LCD/inc -I../ENCODER/inc -std=gnu99 -O3 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -fsingle-precision-constant -Wall -Wdouble-promotion  -g -ggdb -x assembler-with-cpp -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DCORTEX_M4 -DARM_MATH_CM4 -D__FPU_PRESENT=1 -D__FPU_USED=1 -D__EMBEDDED__ -I"../include" -I../Drivers/STM32F4xx_StdPeriph_Driver/inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Audio_DSP/inc -I../LCD/inc -I../ENCODER/inc -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


