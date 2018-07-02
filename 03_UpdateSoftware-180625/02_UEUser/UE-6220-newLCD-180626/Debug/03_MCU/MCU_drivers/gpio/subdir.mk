################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../03_MCU/MCU_drivers/gpio/gpio.c 

OBJS += \
./03_MCU/MCU_drivers/gpio/gpio.o 

C_DEPS += \
./03_MCU/MCU_drivers/gpio/gpio.d 


# Each subdirectory must supply rules for building sources it contributes
03_MCU/MCU_drivers/gpio/%.o: ../03_MCU/MCU_drivers/gpio/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\01_Doc" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\02_Core" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\04_UserBoard" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\05_SoftComponent" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\06_NosPrg" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\linker_file" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\startup" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\adc" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\flash" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\gpio" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\lptmr" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\power_mode" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\spi" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\systick" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\tsi" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\uart" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\03_MCU\MCU_drivers\wdog" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\04_UserBoard\lcd" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\04_UserBoard\light" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\04_UserBoard\uecom" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\05_SoftComponent\common" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\05_SoftComponent\printf" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\05_SoftComponent\timeStamp" -I"C:\Users\wang\OneDrive\00-working\AHL-beta-version-180622\UE-6220-newLCD-180626\05_SoftComponent\valueType" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


