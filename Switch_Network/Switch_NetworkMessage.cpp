/*?*************************************************************************
*                           Switch_NetworkMessage.cpp
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

#include "Switch_NetworkMessage.h"

#include "../Switch_Base/Switch_Debug.h"

/*!
  \brief Constructor
 */
Switch::NetworkMessage::Header::Header ()
: toNetworkAddress    (0x0),
  fromNetworkAddress  (0x0),
  messageType         (MT_DATA)
{
}

/*!
  \brief Destructor
 */
Switch::NetworkMessage::Header::~Header ()
{
}

/*!
  \brief Copy constructor

  \param[in] i_other Object to copy
 */
Switch::NetworkMessage::Header::Header (const Switch::NetworkMessage::Header& i_other)
: toNetworkAddress    (i_other.toNetworkAddress),
  fromNetworkAddress  (i_other.fromNetworkAddress),
  messageType         (i_other.messageType)
{
}

/*!
  \brief Assignment operator

  \param[in] i_other Object to assign to this object
 */
Switch::NetworkMessage::Header& Switch::NetworkMessage::Header::operator= (const Switch::NetworkMessage::Header& i_other)
{
  if (this != &i_other)
  {
    toNetworkAddress    = i_other.toNetworkAddress;
    fromNetworkAddress  = i_other.fromNetworkAddress;
    messageType         = i_other.messageType;
  }
  
  return *this;
}

/*!
  \brief Constructor
 */
Switch::NetworkMessage::NetworkMessage ()
{
//  SWITCH_DEBUG_MSG_1 ("%u\n\r", sizeof (Switch::NetworkMessage::Header));
  SWITCH_ASSERT (NM_MAX_PAYLOAD_SIZE <= (32 - sizeof (Switch::NetworkMessage::Header)));

  memset (payload, 0, NM_MAX_PAYLOAD_SIZE);
}

/*!
  \brief Destructor
 */
Switch::NetworkMessage::~NetworkMessage ()
{
}

/*!
  \brief Copy constructor

  \param[in] i_other Object to copy
 */
Switch::NetworkMessage::NetworkMessage (const Switch::NetworkMessage& i_other)
{
  SWITCH_ASSERT (NM_MAX_PAYLOAD_SIZE <= (32 - sizeof (Switch::NetworkMessage::Header)));
  
  header = i_other.header;
  memcpy (payload, i_other.payload, NM_MAX_PAYLOAD_SIZE);
}

/*!
  \brief Assignment operator

  \param[in] i_other Object to assign to this object
 */
Switch::NetworkMessage& Switch::NetworkMessage::operator= (const Switch::NetworkMessage& i_other)
{
  if (this != &i_other)
  {
    header = i_other.header;
    memcpy (payload, i_other.payload, NM_MAX_PAYLOAD_SIZE);
  }

  return *this;
}
