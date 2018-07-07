################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include 06_NosPrg/subdir.mk
-include 05_SoftComponent/timeStamp/subdir.mk
-include 05_SoftComponent/common/subdir.mk
-include 04_UserBoard/light/subdir.mk
-include 04_UserBoard/lcd/subdir.mk
-include 03_MCU/startup/subdir.mk
-include 03_MCU/MCU_drivers/wdog/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
KL36_Update.hex \

SECONDARY_LIST += \
KL36_Update.lst \


# All Target
all: KL36_Update.elf secondary-outputs

# Tool invocations
KL36_Update.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m0plus -mthumb -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "E:\2017-2018\03_Graduation Project\04_Software\04_wangshi\04_UpdateSoftware-180628\02_UEUser\KL36_Update-180627\03_MCU\linker_file\MKL36Z64xxx4_flash.ld" -Xlinker --gc-sections -Wl,-Map,"KL36_Update.map" -specs=nano.specs -specs=nosys.specs -o "KL36_Update.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

KL36_Update.hex: KL36_Update.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "KL36_Update.elf"  "KL36_Update.hex"
	@echo 'Finished building: $@'
	@echo ' '

KL36_Update.lst: KL36_Update.elf
	@echo 'Invoking: Cross ARM GNU Create Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --line-numbers --wide "KL36_Update.elf" > "KL36_Update.lst"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(CC_DEPS)$(SECONDARY_FLASH)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS)$(SECONDARY_LIST)$(S_UPPER_DEPS) KL36_Update.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
