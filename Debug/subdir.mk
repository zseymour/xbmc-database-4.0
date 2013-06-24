################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../DatabaseManager.cpp \
../ObjectDatabase.cpp \
../URL.cpp \
../Util.cpp \
../test.cpp 

OBJS += \
./DatabaseManager.o \
./ObjectDatabase.o \
./URL.o \
./Util.o \
./test.o 

CPP_DEPS += \
./DatabaseManager.d \
./ObjectDatabase.d \
./URL.d \
./Util.d \
./test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/lib/x86_64-linux-gnu -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


