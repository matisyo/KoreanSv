################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Weapons/BlueBullet.cpp \
../Weapons/Bullet.cpp \
../Weapons/BulletsHandler.cpp 

OBJS += \
./Weapons/BlueBullet.o \
./Weapons/Bullet.o \
./Weapons/BulletsHandler.o 

CPP_DEPS += \
./Weapons/BlueBullet.d \
./Weapons/Bullet.d \
./Weapons/BulletsHandler.d 


# Each subdirectory must supply rules for building sources it contributes
Weapons/%.o: ../Weapons/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0  -lSDL2 -lSDL2_image -lSDL_ttf  -std=c++11  -lpthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


