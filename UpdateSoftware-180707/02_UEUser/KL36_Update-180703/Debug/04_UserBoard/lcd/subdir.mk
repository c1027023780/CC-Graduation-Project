################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../04_UserBoard/lcd/lcd.c 

OBJS += \
./04_UserBoard/lcd/lcd.o 

C_DEPS += \
./04_UserBoard/lcd/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
04_UserBoard/lcd/%.o: ../04_UserBoard/lcd/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\01_Doc" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\02_Core" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\04_UserBoard" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\05_SoftComponent" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\06_NosPrg" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\linker_file" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\MCU_drivers" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\startup" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\MCU_drivers\adc" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\MCU_drivers\flash" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\MCU_drivers\gpio" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\MCU_drivers\lptmr" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\MCU_drivers\spi" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\MCU_drivers\tsi" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\MCU_drivers\uart" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\03_MCU\MCU_drivers\wdog" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\04_UserBoard\lcd" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\04_UserBoard\light" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\04_UserBoard\uecom" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\05_SoftComponent\common" -I"E:\2017-2018\03_Graduation Project\04_Software\05_Wangshi\03_UpdateSoftware-180702\02_UEUser\KL36_Update-180703\05_SoftComponent\valueType" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


