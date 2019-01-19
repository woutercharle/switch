/*?*************************************************************************
*                           Switch_Debug.h
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

#ifndef _SWITCH_DEBUG
#define _SWITCH_DEBUG

#include "Switch_CompilerConfiguration.h"

#ifdef _DEBUG

#  include <printf.h>

#  define SWITCH_DEBUG_BEGIN() ( { printf_begin (); } )
#  define SWITCH_DEBUG_MSG_0(expression) ( { printf (expression); } )
#  define SWITCH_DEBUG_MSG_1(expression, arg0) ( { printf (expression, arg0); } )
#  define SWITCH_DEBUG_MSG_2(expression, arg0, arg1) ( { printf (expression, arg0, arg1); } )
#  define SWITCH_DEBUG_MSG_3(expression, arg0, arg1, arg2) ( { printf (expression, arg0, arg1, arg2); } )
#  define SWITCH_DEBUG_MSG_4(expression, arg0, arg1, arg2, arg3) ( { printf (expression, arg0, arg1, arg2, arg3); } )
#  define SWITCH_DEBUG_MSG_5(expression, arg0, arg1, arg2, arg3, arg4) ( { printf (expression, arg0, arg1, arg2, arg3, arg4); } )
#  define SWITCH_DEBUG(expression) ( { expression; } )
#  define SWITCH_DEBUG_IF(condition, expression) ( { if (condition) { expression; } } );
#  define SWITCH_DEBUG_BYTES(value, nrBytes) ( { uint8_t* pData = reinterpret_cast <uint8_t*> (value); uint8_t length = static_cast <uint8_t> (nrBytes); for (uint8_t b=0; b<length; ++b) { printf ("%x ", pData [b]); } } );
#  define SWITCH_ASSERT(expression) ( { if (!(expression)) { SWITCH_DEBUG_MSG_3 ("%s [%u]: debug assertion failed: %s\n\r", __FILE__, __LINE__, #expression); } } )
#  define SWITCH_ASSERT_THROW(expression, exception) ( { if (!(expression)) { SWITCH_DEBUG_MSG_3 ("%s [%u]: debug assertion failed: %s\n\r", __FILE__, __LINE__, #expression); throw exception; } } )
#  define SWITCH_ASSERT_RETURN_0(expression) ( { if (!(expression)) { SWITCH_DEBUG_MSG_3 ("%s [%u]: debug assertion failed: %s\n\r", __FILE__, __LINE__, #expression); return; } } )
#  define SWITCH_ASSERT_RETURN_1(expression, value) ( { if (!(expression)) { SWITCH_DEBUG_MSG_3 ("%s [%u]: debug assertion failed: %s\n\r", __FILE__, __LINE__, #expression); return value; } } )
#  define SWITCH_DEBUG_PING(count, expression) ( { static uint32_t counter = 0; ++counter; if (0 == counter%(count)) { printf ("%05u - %s", counter/(count), expression); } } )

#else

#  define SWITCH_DEBUG_BEGIN()
#  define SWITCH_DEBUG_MSG_0(expression)
#  define SWITCH_DEBUG_MSG_1(expression, arg0)
#  define SWITCH_DEBUG_MSG_2(expression, arg0, arg1)
#  define SWITCH_DEBUG_MSG_3(expression, arg0, arg1, arg2)
#  define SWITCH_DEBUG_MSG_4(expression, arg0, arg1, arg2, arg3)
#  define SWITCH_DEBUG_MSG_5(expression, arg0, arg1, arg2, arg3, arg4)
#  define SWITCH_DEBUG(expression)
#  define SWITCH_DEBUG_IF(condition, expression)
#  define SWITCH_DEBUG_BYTES(value, nrBytes)
#  define SWITCH_ASSERT(expression)
#  define SWITCH_ASSERT_THROW(expression, exception)
#  define SWITCH_ASSERT_RETURN_0(expression)
#  define SWITCH_ASSERT_RETURN_1(expression, value)
#  define SWITCH_DEBUG_PING(count, expression)

#endif

#endif // _SWITCH_TYPES
