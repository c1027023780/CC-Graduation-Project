################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../05_SoftComponent/common/common.c 

OBJS += \
./05_SoftComponent/common/common.o 

C_DEPS += \
./05_SoftComponent/common/common.d 


# Each subdirectory must supply rules for building sources it contributes
05_SoftComponent/common/%.o: ../05_SoftComponent/common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\01_Doc" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\02_Core" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\04_UserBoard" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\05_SoftComponent" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\06_NosPrg" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\linker_file" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\MCU_drivers" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\startup" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\MCU_drivers\adc" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\MCU_drivers\flash" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\MCU_drivers\gpio" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\MCU_drivers\lptmr" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\MCU_drivers\spi" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\MCU_drivers\tsi" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\MCU_drivers\uart" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\MCU_drivers\wdog" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\04_UserBoard\lcd" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\04_UserBoard\light" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\04_UserBoard\uecom" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\05_SoftComponent\common" -I"E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\05_SoftComponent\timeStamp" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


