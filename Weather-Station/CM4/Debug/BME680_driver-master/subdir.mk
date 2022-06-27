################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BME680_driver-master/bme680.c 

OBJS += \
./BME680_driver-master/bme680.o 

C_DEPS += \
./BME680_driver-master/bme680.d 


# Each subdirectory must supply rules for building sources it contributes
BME680_driver-master/%.o BME680_driver-master/%.su: ../BME680_driver-master/%.c BME680_driver-master/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../Core/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../Drivers/CMSIS/Include -I../BME680_driver-master -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-BME680_driver-2d-master

clean-BME680_driver-2d-master:
	-$(RM) ./BME680_driver-master/bme680.d ./BME680_driver-master/bme680.o ./BME680_driver-master/bme680.su

.PHONY: clean-BME680_driver-2d-master

