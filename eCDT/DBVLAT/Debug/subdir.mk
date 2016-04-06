################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../GenerateRandomPoints.c \
../Main.c \
../RWOFF.c \
../RWObj.c \
../Rotate.c \
../TranslateScale.c \
../eispack.c \
../normalPCA.c \
../pointPCA.c \
../pointSVD.c \
../svd.c \
../svdcmp.c \
../testPSVD.c 

OBJS += \
./GenerateRandomPoints.o \
./Main.o \
./RWOFF.o \
./RWObj.o \
./Rotate.o \
./TranslateScale.o \
./eispack.o \
./normalPCA.o \
./pointPCA.o \
./pointSVD.o \
./svd.o \
./svdcmp.o \
./testPSVD.o 

C_DEPS += \
./GenerateRandomPoints.d \
./Main.d \
./RWOFF.d \
./RWObj.d \
./Rotate.d \
./TranslateScale.d \
./eispack.d \
./normalPCA.d \
./pointPCA.d \
./pointSVD.d \
./svd.d \
./svdcmp.d \
./testPSVD.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/opt/X11/include -I/Users/sfma/Downloads/vlfeat-0.9.20_bin/vl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


