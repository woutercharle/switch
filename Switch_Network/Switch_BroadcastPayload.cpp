/*?*************************************************************************
*                           Switch_BroadcastPayload.cpp
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

#include "Switch_BroadcastPayload.h"

/*!
  \brief Constructor
 */
Switch::BroadcastPayload::BroadcastPayload ()
{
  broadcastNodeDeviceAddress  = 0x0;
  broadcastNodeRxPipeAddress  = 0x0;
  broadcastReason             = BROADCAST_REASON_BEGIN;
}

/*!
  \brief Destructor
 */
Switch::BroadcastPayload::~BroadcastPayload ()
{
}

/*!
  \brief Copy constructor

  \param[in] i_other Object to copy
 */
Switch::BroadcastPayload::BroadcastPayload (const Switch::BroadcastPayload& i_other)
{
  broadcastNodeDeviceAddress  = i_other.broadcastNodeDeviceAddress;
  broadcastNodeRxPipeAddress  = i_other.broadcastNodeRxPipeAddress;
  broadcastReason             = i_other.broadcastReason;
  deviceInfo                  = i_other.deviceInfo;
}

/*!
  \brief Assignment operator

  \param[in] i_other Object to assign to this object

  \return Reference to this object
 */
Switch::BroadcastPayload& Switch::BroadcastPayload::operator= (const Switch::BroadcastPayload& i_other)
{
  // avoid self-assignment
  if (this != &i_other)
  {
    broadcastNodeDeviceAddress  = i_other.broadcastNodeDeviceAddress;
    broadcastNodeRxPipeAddress  = i_other.broadcastNodeRxPipeAddress;
    broadcastReason             = i_other.broadcastReason;
    deviceInfo                  = i_other.deviceInfo;
  }

  // return reference to this object
  return *this;
}

