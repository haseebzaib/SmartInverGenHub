################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Application/System/System_Rtos.cpp \
../Application/System/System_rtc.cpp \
../Application/System/System_serial.cpp \
../Application/System/System_sys.cpp 

OBJS += \
./Application/System/System_Rtos.o \
./Application/System/System_rtc.o \
./Application/System/System_serial.o \
./Application/System/System_sys.o 

CPP_DEPS += \
./Application/System/System_Rtos.d \
./Application/System/System_rtc.d \
./Application/System/System_serial.d \
./Application/System/System_sys.d 


# Each subdirectory must supply rules for building sources it contributes
Application/System/%.o Application/System/%.su Application/System/%.cyclo: ../Application/System/%.cpp Application/System/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/Modem" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/Sensor" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/System" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/u8g2/csrc" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/u8g2" -I"F:/haseeb/liquidMeasuringProject/SmartInverGenHub/Application/User_Interface" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-System

clean-Application-2f-System:
	-$(RM) ./Application/System/System_Rtos.cyclo ./Application/System/System_Rtos.d ./Application/System/System_Rtos.o ./Application/System/System_Rtos.su ./Application/System/System_rtc.cyclo ./Application/System/System_rtc.d ./Application/System/System_rtc.o ./Application/System/System_rtc.su ./Application/System/System_serial.cyclo ./Application/System/System_serial.d ./Application/System/System_serial.o ./Application/System/System_serial.su ./Application/System/System_sys.cyclo ./Application/System/System_sys.d ./Application/System/System_sys.o ./Application/System/System_sys.su

.PHONY: clean-Application-2f-System

