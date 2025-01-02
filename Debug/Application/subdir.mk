################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Application/ControlTask.cpp \
../Application/DisplayTask.cpp \
../Application/ModemTask.cpp \
../Application/app_main.cpp 

OBJS += \
./Application/ControlTask.o \
./Application/DisplayTask.o \
./Application/ModemTask.o \
./Application/app_main.o 

CPP_DEPS += \
./Application/ControlTask.d \
./Application/DisplayTask.d \
./Application/ModemTask.d \
./Application/app_main.d 


# Each subdirectory must supply rules for building sources it contributes
Application/%.o Application/%.su Application/%.cyclo: ../Application/%.cpp Application/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/Modem" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/Sensor" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/System" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/u8g2/csrc" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/u8g2" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/User_Interface" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application

clean-Application:
	-$(RM) ./Application/ControlTask.cyclo ./Application/ControlTask.d ./Application/ControlTask.o ./Application/ControlTask.su ./Application/DisplayTask.cyclo ./Application/DisplayTask.d ./Application/DisplayTask.o ./Application/DisplayTask.su ./Application/ModemTask.cyclo ./Application/ModemTask.d ./Application/ModemTask.o ./Application/ModemTask.su ./Application/app_main.cyclo ./Application/app_main.d ./Application/app_main.o ./Application/app_main.su

.PHONY: clean-Application

