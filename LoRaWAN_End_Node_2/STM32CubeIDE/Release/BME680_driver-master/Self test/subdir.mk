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
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DCORE_CM4 -DSTM32WL55xx -DUSE_HAL_DRIVER -c -I../../Core/Inc -I../../LoRaWAN/App -I../../LoRaWAN/Target -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../../../../../Utilities/trace/adv_trace -I../../../../../../../Utilities/misc -I../../../../../../../Utilities/sequencer -I../../../../../../../Utilities/timer -I../../../../../../../Utilities/lpm/tiny_lpm -I../../../../../../../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../../../../../../../Middlewares/Third_Party/SubGHz_Phy -I../../../../../../../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../../../../../Middlewares/Third_Party/LoRaWAN/Crypto -I../../../../../../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../../../../../../Middlewares/Third_Party/LoRaWAN/Mac -I../../../../../../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../../../../../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Drivers/STM32WLxx_HAL_Driver/Inc -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Utilities/trace/adv_trace -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Utilities/misc -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Utilities/sequencer -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Utilities/timer -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Utilities/lpm/tiny_lpm -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Middlewares/Third_Party/SubGHz_Phy -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Drivers/CMSIS/Device/ST/STM32WLxx/Include -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Middlewares/Third_Party/LoRaWAN/Crypto -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Middlewares/Third_Party/LoRaWAN/Mac/Region -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Middlewares/Third_Party/LoRaWAN/Mac -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Middlewares/Third_Party/LoRaWAN/LmHandler -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Middlewares/Third_Party/LoRaWAN/Utilities -IC:/Users/dumbt/STM32Cube/Repository/STM32Cube_FW_WL_V1.2.0/Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"BME680_driver-master/Self test/bme680_selftest.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-BME680_driver-2d-master-2f-Self-20-test

clean-BME680_driver-2d-master-2f-Self-20-test:
	-$(RM) ./BME680_driver-master/Self\ test/bme680_selftest.d ./BME680_driver-master/Self\ test/bme680_selftest.o ./BME680_driver-master/Self\ test/bme680_selftest.su

.PHONY: clean-BME680_driver-2d-master-2f-Self-20-test

