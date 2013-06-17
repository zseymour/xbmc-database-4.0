################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../dbwrappers/Database.cpp \
../dbwrappers/dataset.cpp \
../dbwrappers/mysqldataset.cpp \
../dbwrappers/qry_dat.cpp \
../dbwrappers/sqlitedataset.cpp 

OBJS += \
./dbwrappers/Database.o \
./dbwrappers/dataset.o \
./dbwrappers/mysqldataset.o \
./dbwrappers/qry_dat.o \
./dbwrappers/sqlitedataset.o 

CPP_DEPS += \
./dbwrappers/Database.d \
./dbwrappers/dataset.d \
./dbwrappers/mysqldataset.d \
./dbwrappers/qry_dat.d \
./dbwrappers/sqlitedataset.d 


# Each subdirectory must supply rules for building sources it contributes
dbwrappers/%.o: ../dbwrappers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/lib/x86_64-linux-gnu -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


