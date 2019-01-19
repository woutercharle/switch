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

#include "Switch_Utilities.h"

#ifndef ARDUINO
#include <sys/time.h>
#endif

/*!
  \brief Gets the current instant in milliseconds

  \return The current instant in milliseconds
 */
uint32_t Switch::NowInMilliseconds ()
{
#ifdef ARDUINO
  return millis ();
#else
  timeval timeValue;
  gettimeofday (&timeValue, 0x0);
  return static_cast <uint32_t> (1000*timeValue.tv_sec) + static_cast <uint32_t> (0.001*timeValue.tv_usec + 0.5);
#endif
}

/*!
  \brief Gets the time difference between then and now in milliseconds.

  \param [in] i_then The time to get the difference between now.

  \return The time difference between then and now in milliseconds.
 */
uint32_t Switch::MillisecondsSince (const uint32_t& i_timeMs)
{
  uint32_t now = NowInMilliseconds ();
  return TimeBetween (i_timeMs, now);
}

/*!
  \brief Gets the time difference between two times.

  \param [in] i_oldestTime The oldest time. This time plus the time difference is the youngest time.
  \param [in] i_youngestTime The youngest time. This time minus the time difference is the oldest time.

  \return The time difference between the two times.

  \note Takes into account overflow if i_oldestTime < i_youngestTime.
 */
uint32_t Switch::TimeBetween (const uint32_t& i_oldestTime, const uint32_t& i_youngestTime)
{
  if (i_youngestTime < i_oldestTime)
  {
    return 0xFFFFFFFF - (i_oldestTime - i_youngestTime);
  }
  else
  {
    return i_youngestTime - i_oldestTime;
  }
}

