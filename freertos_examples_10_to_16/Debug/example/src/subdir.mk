################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc43xx.c \
../example/src/freertos_examples_10_to_16.c \
../example/src/sysinit.c 

OBJS += \
./example/src/cr_startup_lpc43xx.o \
./example/src/freertos_examples_10_to_16.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/cr_startup_lpc43xx.d \
./example/src/freertos_examples_10_to_16.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -D__REDLIB__ -DCORE_M4 -D__MULTICORE_NONE -I"C:\Users\JMCruz\Documents\LPCXpresso_8.2.0_647\workspace-EDU_CIAA_NXP\lpc_chip_43xx\inc" -I"C:\Users\JMCruz\Documents\LPCXpresso_8.2.0_647\workspace-EDU_CIAA_NXP\lpc_board_nxp_lpcxpresso_4337\inc" -I"C:\Users\JMCruz\Documents\LPCXpresso_8.2.0_647\workspace-EDU_CIAA_NXP\freertos_examples_10_to_16\example\inc" -I"C:\Users\JMCruz\Documents\LPCXpresso_8.2.0_647\workspace-EDU_CIAA_NXP\freertos_examples_10_to_16\freertos\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


