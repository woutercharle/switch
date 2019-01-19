#############################################################################
#
# Makefile for libSwitch_Node on Raspberry Pi
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
LIBNAME=libSwitch_Node
#CC_PREFIX=arm-unknown-linux-gnueabi-
CC_PREFIX=
CC=${CC_PREFIX}gcc
CXX=${CC_PREFIX}g++
AR=${CC_PREFIX}ar
ADDITIONAL_INC_DIRS=-I../../Thirdparty/RF24_HardwareRadio/inc

# The recommended compiler flags for the Raspberry Pi
#CCFLAGS = -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -std=c++11
CCFLAGS = -std=c++11

# make all
all: CCFLAGS += -Ofast
all: libSwitch_Node install

# make debug version
debug: CCFLAGS += -D_DEBUG -g -O0 -fbuiltin
debug: libSwitch_Node install

# Make the library
libSwitch_Node: Switch_Node.o
	${AR} rcs ${LIBNAME}.a Switch_Node.o

# Library parts
Switch_Node.o: ${SRCDIR}Switch_Node.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} ${ADDITIONAL_INC_DIRS} -c ${SRCDIR}Switch_Node.cpp 

# clear build files
clean:
	rm -rf *.o ${LIBNAME}.a

# Install the library to LIBPATH
install: 
	cp ${LIBNAME}.a ${LIBDIR}${LIBNAME}.a

