################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../utils/Crc32.cpp \
../utils/RegExp.cpp \
../utils/StringUtils.cpp \
../utils/Variant.cpp 

C_SRCS += \
../utils/fstrcmp.c 

OBJS += \
./utils/Crc32.o \
./utils/RegExp.o \
./utils/StringUtils.o \
./utils/Variant.o \
./utils/fstrcmp.o 

C_DEPS += \
./utils/fstrcmp.d 

CPP_DEPS += \
./utils/Crc32.d \
./utils/RegExp.d \
./utils/StringUtils.d \
./utils/Variant.d 


# Each subdirectory must supply rules for building sources it contributes
utils/%.o: ../utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/lib/x86_64-linux-gnu -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

utils/%.o: ../utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/lib/x86_64-linux-gnu -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


