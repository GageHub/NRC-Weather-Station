################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/dumbt/STM32CubeIDE/workspace_1.9.0/Weather-Station/Common/System/system_stm32wlxx.c 

OBJS += \
./Common/System/system_stm32wlxx.o 

C_DEPS += \
./Common/System/system_stm32wlxx.d 


# Each subdirectory must supply rules for building sources it contributes
Common/System/system_stm32wlxx.o: C:/Users/dumbt/STM32CubeIDE/workspace_1.9.0/Weather-Station/Common/System/system_stm32wlxx.c Common/System/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../Core/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Common-2f-System

clean-Common-2f-System:
	-$(RM) ./Common/System/system_stm32wlxx.d ./Common/System/system_stm32wlxx.o ./Common/System/system_stm32wlxx.su

.PHONY: clean-Common-2f-System

