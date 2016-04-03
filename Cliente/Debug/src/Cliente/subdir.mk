################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Cliente/AlanTuring.cpp \
../src/Cliente/Menu.cpp \
../src/Cliente/cliente.cpp 

OBJS += \
./src/Cliente/AlanTuring.o \
./src/Cliente/Menu.o \
./src/Cliente/cliente.o 

CPP_DEPS += \
./src/Cliente/AlanTuring.d \
./src/Cliente/Menu.d \
./src/Cliente/cliente.d 


# Each subdirectory must supply rules for building sources it contributes
src/Cliente/%.o: ../src/Cliente/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -lpthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


