################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Application/User_Interface/UI.cpp \
../Application/User_Interface/img.cpp 

OBJS += \
./Application/User_Interface/UI.o \
./Application/User_Interface/img.o 

CPP_DEPS += \
./Application/User_Interface/UI.d \
./Application/User_Interface/img.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User_Interface/%.o Application/User_Interface/%.su Application/User_Interface/%.cyclo: ../Application/User_Interface/%.cpp Application/User_Interface/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/Inverter" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/Modem" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/Sensor" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/System" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/u8g2/csrc" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/u8g2" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/User_Interface" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User_Interface

clean-Application-2f-User_Interface:
	-$(RM) ./Application/User_Interface/UI.cyclo ./Application/User_Interface/UI.d ./Application/User_Interface/UI.o ./Application/User_Interface/UI.su ./Application/User_Interface/img.cyclo ./Application/User_Interface/img.d ./Application/User_Interface/img.o ./Application/User_Interface/img.su

.PHONY: clean-Application-2f-User_Interface

