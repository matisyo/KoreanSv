################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Server/AlanTuring.cpp \
../src/Server/MessageFactory.cpp \
../src/Server/server.cpp 

OBJS += \
./src/Server/AlanTuring.o \
./src/Server/MessageFactory.o \
./src/Server/server.o 

CPP_DEPS += \
./src/Server/AlanTuring.d \
./src/Server/MessageFactory.d \
./src/Server/server.d 


# Each subdirectory must supply rules for building sources it contributes
src/Server/%.o: ../src/Server/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -lpthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

