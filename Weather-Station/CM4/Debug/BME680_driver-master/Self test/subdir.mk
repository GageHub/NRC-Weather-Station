################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BME680_driver-master/Self\ test/bme680_selftest.c 

OBJS += \
./BME680_driver-master/Self\ test/bme680_selftest.o 

C_DEPS += \
./BME680_driver-master/Self\ test/bme680_selftest.d 


# Each subdirectory must supply rules for building sources it contributes
BME680_driver-master/Self\ test/bme680_selftest.o: ../BME680_driver-master/Self\ test/bme680_selftest.c BME680_driver-master/Self\ test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../Core/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../Drivers/CMSIS/Include -I../BME680_driver-master -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"BME680_driver-master/Self test/bme680_selftest.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-BME680_driver-2d-master-2f-Self-20-test

clean-BME680_driver-2d-master-2f-Self-20-test:
	-$(RM) ./BME680_driver-master/Self\ test/bme680_selftest.d ./BME680_driver-master/Self\ test/bme680_selftest.o ./BME680_driver-master/Self\ test/bme680_selftest.su

.PHONY: clean-BME680_driver-2d-master-2f-Self-20-test

