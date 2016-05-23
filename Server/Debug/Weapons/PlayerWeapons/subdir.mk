################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Weapons/PlayerWeapons/BasicWeapon.cpp \
../Weapons/PlayerWeapons/FiveBulletsWeapon.cpp 

OBJS += \
./Weapons/PlayerWeapons/BasicWeapon.o \
./Weapons/PlayerWeapons/FiveBulletsWeapon.o 

CPP_DEPS += \
./Weapons/PlayerWeapons/BasicWeapon.d \
./Weapons/PlayerWeapons/FiveBulletsWeapon.d 


# Each subdirectory must supply rules for building sources it contributes
Weapons/PlayerWeapons/%.o: ../Weapons/PlayerWeapons/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__cplusplus=201103L -O0 -g3 -Wall -c -fmessage-length=0  -lSDL2 -lSDL2_image -lSDL_ttf  -std=c++11  -lpthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


