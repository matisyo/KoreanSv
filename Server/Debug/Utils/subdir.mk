################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Utils/GameObjectFactory.cpp \
../Utils/Logger.cpp \
../Utils/TextureHelper.cpp \
../Utils/Timer.cpp 

OBJS += \
./Utils/GameObjectFactory.o \
./Utils/Logger.o \
./Utils/TextureHelper.o \
./Utils/Timer.o 

CPP_DEPS += \
./Utils/GameObjectFactory.d \
./Utils/Logger.d \
./Utils/TextureHelper.d \
./Utils/Timer.d 


# Each subdirectory must supply rules for building sources it contributes
Utils/%.o: ../Utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0  -lSDL2 -lSDL2_image -lSDL_ttf  -std=c++11  -lpthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


