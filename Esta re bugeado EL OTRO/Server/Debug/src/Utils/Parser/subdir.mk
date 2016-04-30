################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Utils/Parser/ParserCliente.cpp \
../src/Utils/Parser/ParserServidor.cpp 

OBJS += \
./src/Utils/Parser/ParserCliente.o \
./src/Utils/Parser/ParserServidor.o 

CPP_DEPS += \
./src/Utils/Parser/ParserCliente.d \
./src/Utils/Parser/ParserServidor.d 


# Each subdirectory must supply rules for building sources it contributes
src/Utils/Parser/%.o: ../src/Utils/Parser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -lpthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


