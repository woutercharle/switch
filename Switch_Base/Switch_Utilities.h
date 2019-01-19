/*?*************************************************************************
*                           Switch_Utilities.h
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

#ifndef _SWITCH_UTILITIES
#define _SWITCH_UTILITIES

#include "Switch_CompilerConfiguration.h"

namespace Switch
{
  /*!
    \brief Gets the current instant in milliseconds

    \return The current instant in milliseconds
   */
  uint32_t NowInMilliseconds ();

  /*!
    \brief Gets the time difference between then and now in milliseconds.

    \param [in] i_then The time to get the difference between now.

    \return The time difference between then and now in milliseconds.
   */
  uint32_t MillisecondsSince (const uint32_t& i_timeMs);

  /*!
    \brief Gets the time difference between two times.

    \param [in] i_oldestTime The oldest time. This time plus the time difference is the youngest time.
    \param [in] i_youngestTime The youngest time. This time minus the time difference is the oldest time.

    \return The time difference between the two times.

    \note Takes into account overflow if i_oldestTime < i_youngestTime.
   */
  uint32_t TimeBetween (const uint32_t& i_oldestTime, const uint32_t& i_youngestTime);
}

#endif // _SWITCH_UTILITIES
