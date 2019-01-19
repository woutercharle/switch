/*?*************************************************************************
*                           Switch_CompilerConfiguration.h
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
*    email                : wouter.charle@gmail.com
*
*    DISCLAIMER OF DAMAGES
*    ---------------------
*    Wouter Charle has made every effort possible to ensure that the software
*    is free of any bugs or errors, however in no way is the software to
*    be considered error or bug free. You assume all responsibility for
*    any damages or lost data that may result from any errors or bugs in
*    the software.
*
*    IN NO EVENT WILL VISION++ BE LIABLE TO YOU FOR ANY GENERAL, SPECIAL,
*    INDIRECT, CONSEQUENTIAL, INCIDENTAL OR OTHER DAMAGES ARISING OUT OF
*    THIS LICENSE.
*
*    In no case shall Wouter Charle's liability exceed the purchase price for
*    the software or services.
*
***************************************************************************/

#ifndef _SWITCH_COMPILERCONFIGURATION
#define _SWITCH_COMPILERCONFIGURATION

//#define SWITCH_SERIALIZE_WITH_COMMENTS

//#include <stddef.h>

// Stuff that is normally provided by Arduino
#ifdef ARDUINO
#  if ARDUINO < 100
#    include <WProgram.h>
#  else
#    include <Arduino.h>
#  endif
//#  include <SPI.h>
#else
#  include <stdint.h>
#  include <stdio.h>
#  include <string.h>
//extern HardwareSPI SPI;
#endif

// Avoid spurious warnings
/*#if 1
#if ! defined( NATIVE ) && defined( ARDUINO )
#undef PROGMEM
#define PROGMEM __attribute__(( section(".progmem.data") ))
#undef PSTR
#define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))
#endif
#endif

// Progmem is Arduino-specific
#ifdef ARDUINO
#include <avr/pgmspace.h>
#define PRIPSTR "%S"
#else
typedef char const char;
typedef uint16_t prog_uint16_t;
#define PSTR(x) (x)
#define printf_P printf
#define strlen_P strlen
#define PROGMEM
#define pgm_read_word(p) (*(p))
#define PRIPSTR "%s"
#endif*/

#endif // _SWITCH_COMPILERCONFIGURATION
