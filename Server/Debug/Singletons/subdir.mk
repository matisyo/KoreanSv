################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Singletons/CollisionHandler.cpp \
../Singletons/GameTimeHelper.cpp \
../Singletons/InputHandler.cpp \
../Singletons/ObjectIdGenerator.cpp \
../Singletons/TextureManager.cpp 

OBJS += \
./Singletons/CollisionHandler.o \
./Singletons/GameTimeHelper.o \
./Singletons/InputHandler.o \
./Singletons/ObjectIdGenerator.o \
./Singletons/TextureManager.o 

CPP_DEPS += \
./Singletons/CollisionHandler.d \
./Singletons/GameTimeHelper.d \
./Singletons/InputHandler.d \
./Singletons/ObjectIdGenerator.d \
./Singletons/TextureManager.d 


# Each subdirectory must supply rules for building sources it contributes
Singletons/%.o: ../Singletons/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0  -lSDL2 -lSDL2_image -lSDL_ttf  -std=c++11  -lpthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


