################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Main.c \
../RWObj.c \
../Rotate.c \
../pointSVD.c \
../svdcmp.c \
../testPSVD.c 

OBJS += \
./Main.o \
./RWObj.o \
./Rotate.o \
./pointSVD.o \
./svdcmp.o \
./testPSVD.o 

C_DEPS += \
./Main.d \
./RWObj.d \
./Rotate.d \
./pointSVD.d \
./svdcmp.d \
./testPSVD.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


