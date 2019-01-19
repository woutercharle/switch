#############################################################################
#
# Makefile for libSwitch_Network on Raspberry Pi
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
LIBNAME=libSwitch_Network
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
all: libSwitch_Network install

# make debug version
debug: CCFLAGS += -D_DEBUG -g -O0 -fbuiltin
debug: libSwitch_Network install

# Make the library
libSwitch_Network: Switch_NetworkAddress.o Switch_BroadcastPayload.o Switch_DataPayload.o Switch_NetworkMessage.o Switch_NodeAssignmentPayload.o Switch_NodeExclusionPayload.o Switch_PingPongPayload.o
	${AR} rcs ${LIBNAME}.a Switch_NetworkAddress.o Switch_BroadcastPayload.o Switch_DataPayload.o  Switch_NetworkMessage.o Switch_NodeAssignmentPayload.o Switch_NodeExclusionPayload.o Switch_PingPongPayload.o

# Library parts
Switch_NetworkAddress.o: ${SRCDIR}Switch_NetworkAddress.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_NetworkAddress.cpp

Switch_BroadcastPayload.o: ${SRCDIR}Switch_BroadcastPayload.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_BroadcastPayload.cpp

Switch_DataPayload.o: ${SRCDIR}Switch_DataPayload.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_DataPayload.cpp

Switch_NetworkMessage.o: ${SRCDIR}Switch_NetworkMessage.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_NetworkMessage.cpp

Switch_NodeAssignmentPayload.o: ${SRCDIR}Switch_NodeAssignmentPayload.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_NodeAssignmentPayload.cpp

Switch_NodeExclusionPayload.o: ${SRCDIR}Switch_NodeExclusionPayload.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_NodeExclusionPayload.cpp

Switch_PingPongPayload.o: ${SRCDIR}Switch_PingPongPayload.cpp
	${CXX} -Wall -fPIC ${CCFLAGS} -c ${SRCDIR}Switch_PingPongPayload.cpp

# clear build files
clean:
	rm -rf *.o ${LIBNAME}.a

# Install the library to LIBPATH
install: 
	cp ${LIBNAME}.a ${LIBDIR}${LIBNAME}.a

