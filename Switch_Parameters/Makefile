#############################################################################
#
# Makefile for libSwitch_Parameters on Raspberry Pi
#
# License: Proprietary
# Author:  Wouter Charle <wouter.charle@gmail.com>
# Date:    2013/10/19 (version 1.0)
#
# Description:
# ------------
# use make all and mak install to install the library 
# You can change the install directory by editing the LIBDIR line
#
SRCDIR=./
#LIBDIR=../../../Libraries/
LIBDIR=/usr/lib/switch/
LIBNAME=libSwitch_Parameters
#CC_PREFIX=arm-unknown-linux-gnueabi-
CC_PREFIX=
CC=${CC_PREFIX}gcc
CXX=${CC_PREFIX}g++
AR=${CC_PREFIX}ar

# The recommended compiler flags for the Raspberry Pi
#CCFLAGS = -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -std=c++11
CCFLAGS = -std=c++11

# make all
all: CCFLAGS += -Ofast
all: libSwitch_Parameters install

# make debug version
debug: CCFLAGS += -D_DEBUG -g -O0 -fbuiltin
debug: libSwitch_Parameters install

# Make the library
libSwitch_Parameters: Switch_Parameter.o Switch_ParameterContainer.o Switch_ParameterizedObject.o
	${AR} rcs ${LIBNAME}.a Switch_Parameter.o Switch_ParameterContainer.o Switch_ParameterizedObject.o

# Library parts
Switch_Parameter.o: ${SRCDIR}Switch_Parameter.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_Parameter.cpp 
Switch_ParameterContainer.o: ${SRCDIR}Switch_ParameterContainer.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_ParameterContainer.cpp 
Switch_ParameterizedObject.o: ${SRCDIR}Switch_ParameterizedObject.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_ParameterizedObject.cpp 

# clear build files
clean:
	rm -rf *.o ${LIBNAME}.a

# Install the library to LIBPATH
install: 
	cp ${LIBNAME}.a ${LIBDIR}${LIBNAME}.a

