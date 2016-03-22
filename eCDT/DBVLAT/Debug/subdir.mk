################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CPCA.c \
../GenerateRandomPoints.c \
../Main.c \
../RWObj.c \
../Rotate.c \
../depthImage.c \
../eispack.c \
../pointSVD.c \
../svd.c \
../svdcmp.c \
../testPSVD.c 

OBJS += \
./CPCA.o \
./GenerateRandomPoints.o \
./Main.o \
./RWObj.o \
./Rotate.o \
./depthImage.o \
./eispack.o \
./pointSVD.o \
./svd.o \
./svdcmp.o \
./testPSVD.o 

C_DEPS += \
./CPCA.d \
./GenerateRandomPoints.d \
./Main.d \
./RWObj.d \
./Rotate.d \
./depthImage.d \
./eispack.d \
./pointSVD.d \
./svd.d \
./svdcmp.d \
./testPSVD.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/opt/X11/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


