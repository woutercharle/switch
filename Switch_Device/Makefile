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
LIBDIR=../../../Libraries/
SYS_ROOT=../../../Tools/crosstools-ng/bin/arm-unknown-linux-gnueabi/sysroot/
LIBNAME=libSwitch_Device
CC_PREFIX=arm-unknown-linux-gnueabi-
CC=${CC_PREFIX}gcc
CXX=${CC_PREFIX}g++
AR=${CC_PREFIX}ar
ADDITIONAL_INC_DIRS=-I../../Thirdparty/sqlite/include \
                    -I../../Thirdparty/jsoncpp/include \
                    -I../../Thirdparty/ssvutils/include \
                    -I${SYS_ROOT}usr/include

# The recommended compiler flags for the Raspberry Pi
CCFLAGS = -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -std=c++11

# make all
all: CCFLAGS += -Ofast
all: libSwitch_Device install

# make debug version
debug: CCFLAGS += -D_DEBUG -g -O0 -fbuiltin
debug: libSwitch_Device install

# Make the library
libSwitch_Device: Switch_DataAdapter.o Switch_DataContainer.o Switch_Device.o Switch_DeviceStore.o
	${AR} rcs ${LIBNAME}.a Switch_DataAdapter.o Switch_DataContainer.o Switch_Device.o Switch_DeviceStore.o

# Library parts
Switch_DataAdapter.o: ${SRCDIR}Switch_DataAdapter.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} ${ADDITIONAL_INC_DIRS} -c ${SRCDIR}Switch_DataAdapter.cpp 
Switch_DataContainer.o: ${SRCDIR}Switch_DataContainer.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} ${ADDITIONAL_INC_DIRS} -c ${SRCDIR}Switch_DataContainer.cpp 
Switch_Device.o: ${SRCDIR}Switch_Device.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} ${ADDITIONAL_INC_DIRS} -c ${SRCDIR}Switch_Device.cpp
Switch_DeviceStore.o: ${SRCDIR}Switch_DeviceStore.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} ${ADDITIONAL_INC_DIRS} -c ${SRCDIR}Switch_DeviceStore.cpp

# clear build files
clean:
	rm -rf *.o ${LIBNAME}.a

# Install the library to LIBPATH
install: 
	cp ${LIBNAME}.a ${LIBDIR}${LIBNAME}.a

