/*?*************************************************************************
*                           Switch_NetworkMessage.h
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

#ifndef _SWITCH_NETWORKMESSAGE
#define _SWITCH_NETWORKMESSAGE

#include "Switch_NetworkAddress.h"

#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"

#pragma pack (push)
#pragma pack (1)

namespace Switch
{
  /*!
    \brief Network message

    Data structure representing a message that can be transmitted over the network.
    Consists of a header and a payload.
   */
  class NetworkMessage
  {
  public:

    /*
      Maximum size in bytes of the payload of one network message
      Must be smaller or equal to 32 - sizeof (Header)
     */
#   define NM_MAX_PAYLOAD_SIZE 27

    /*
      Enumeration of all possible types of messages
     */
    typedef uint8_t message_type_type;
#   define MT_BROADCAST           0
#   define MT_ADDRESS_ASSIGNMENT  1
#   define MT_NODE_EXCLUSION      2
#   define MT_PING                3
#   define MT_PONG                4
#   define MT_DATA                5

    /*!
      \brief Information container about message partitioning

      A larger network message can be partitioned in multiple network
      messages. E.g., when the payload does not fit in one network message.
     */
    struct PartitionInfo
    {
      unsigned nrPartitions : 4;  ///< The total number of partitions
      unsigned partitionNr  : 4;  ///< The partition number of this message
    };

    /*!
      \brief Container of all relevant message information other than the payload
     */
    class Header
    {
    public:

      /*!
        \brief Constructor
       */
      Header ();
      /*!
        \brief Destructor
       */
      ~Header ();

      /*!
        \brief Copy constructor

        \param[in] i_other Object to copy
       */
      Header (const Header& i_other);
      /*!
        \brief Assignment operator

        \param[in] i_other Object to assign to this object
       */
      Header& operator= (const Header& i_other);

      // members
      Switch::NetworkAddress  toNetworkAddress;   ///< The network address of the receiver node
      Switch::NetworkAddress  fromNetworkAddress; ///< The network address of the sender node
      message_type_type       messageType;        ///< The type of this message
    };

    /*!
      \brief Constructor
     */
    NetworkMessage ();
    /*!
      \brief Destructor
     */
    ~NetworkMessage ();

    /*!
      \brief Copy constructor

      \param[in] i_other Object to copy
     */
    NetworkMessage (const NetworkMessage& i_other);
    /*!
      \brief Assignment operator

      \param[in] i_other Object to assign to this object
     */
    NetworkMessage& operator= (const NetworkMessage& i_other);

    // members
    Header	header;   						///< The message's header 
    uint8_t payload [NM_MAX_PAYLOAD_SIZE];  ///< The actual payload
  };
}

#pragma pack (pop)

#endif // _SWITCH_NETWORKMESSAGE
