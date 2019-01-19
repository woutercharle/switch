#############################################################################
#
# Makefile for libSwitch_Router on Raspberry Pi
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
LIBNAME=libSwitch_Router
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
all: libSwitch_Router install

# make debug version
debug: CCFLAGS += -D_DEBUG -g -O0 -fbuiltin
debug: libSwitch_Router install

# Make the library
libSwitch_Router: Switch_RouterNodeModel.o Switch_RouterNetworkModel.o Switch_Router.o
	${AR} rcs ${LIBNAME}.a Switch_RouterNodeModel.o Switch_RouterNetworkModel.o Switch_Router.o

# Library parts
Switch_Router.o: ${SRCDIR}Switch_Router.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} ${ADDITIONAL_INC_DIRS} -c ${SRCDIR}Switch_Router.cpp 

Switch_RouterNetworkModel.o: ${SRCDIR}Switch_RouterNetworkModel.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} ${ADDITIONAL_INC_DIRS} -c ${SRCDIR}Switch_RouterNetworkModel.cpp 

Switch_RouterNodeModel.o: ${SRCDIR}Switch_RouterNodeModel.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} ${ADDITIONAL_INC_DIRS} -c ${SRCDIR}Switch_RouterNodeModel.cpp 

# clear build files
clean:
	rm -rf *.o ${LIBNAME}.a

# Install the library to LIBPATH
install: 
	cp ${LIBNAME}.a ${LIBDIR}${LIBNAME}.a

