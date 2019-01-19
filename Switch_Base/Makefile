#############################################################################
#
# Makefile for libSwitch_Base on Raspberry Pi
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
LIBNAME=libSwitch_Base
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
all: libSwitch_Base install

# make debug version
debug: CCFLAGS += -D_DEBUG -g -O0 -fbuiltin
debug: libSwitch_Base install

# Make the library
libSwitch_Base: Switch_Utilities.o
	${AR} rcs ${LIBNAME}.a Switch_Utilities.o

# Library parts
Switch_Utilities.o: ${SRCDIR}Switch_Utilities.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_Utilities.cpp

# clear build files
clean:
	rm -rf *.o ${LIBNAME}.a

# Install the library to LIBPATH
install:
	mkdir ${LIBDIR}
	cp ${LIBNAME}.a ${LIBDIR}${LIBNAME}.a

