/*?*************************************************************************
*                           Switch_BroadcastPayload.h
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

#ifndef _SWITCH_BROADCASTPAYLOAD
#define _SWITCH_BROADCASTPAYLOAD

#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"

#pragma pack (push)
#pragma pack (1)

namespace Switch
{
  /*!
    \brief Broadcast network message payload
   */
  class BroadcastPayload
  {
  public:

    typedef uint8_t Reason;
    #define BROADCAST_REASON_BEGIN        0
    #define BROADCAST_REASON_PARENT_LOST  1

    /*!
      \brief Constructor
     */
    BroadcastPayload ();

    /*!
      \brief Destructor
     */
    ~BroadcastPayload ();

    /*!
      \brief Copy constructor

      \param[in] i_other Object to copy
     */
    BroadcastPayload (const BroadcastPayload& i_other);

    /*!
      \brief Assignment operator

      \param[in] i_other Object to assign to this object

      \return Reference to this object
     */
    BroadcastPayload& operator= (const BroadcastPayload& i_other);

    // members
    switch_device_address_type  broadcastNodeDeviceAddress; ///< Device address of the broadcasting node.
    switch_pipe_address_type    broadcastNodeRxPipeAddress; ///< Address of the rx pipe of the broadcasting node. Can be used to reply to the node.
    Reason                      broadcastReason;            ///< Reason why the broadcast message is sent
    Switch::DeviceInfo          deviceInfo;                 ///< Information about the device type.
  };
}

#pragma pack (pop)

#endif // _SWITCH_NETWORKBROADCAST
