/*?*************************************************************************
*                           Switch_Node.cpp
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

#include "Switch_Node.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"
#include "../Switch_Base/Switch_StdLibExtras.h"
#include "../Switch_Base/Switch_Utilities.h"
#include "../Switch_Network/Switch_NetworkConfiguration.h"
#include "../Switch_Network/Switch_BroadcastPayload.h"
#include "../Switch_Network/Switch_NodeAssignmentPayload.h"
#include "../Switch_Network/Switch_NodeExclusionPayload.h"
#include "../Switch_Network/Switch_PingPongPayload.h"

// thirdparty includes
#include <RF24.h>


/*!
  \brief Constructor
 */
Switch::Node::Configuration::Configuration ()
: deviceAddress         (0x0),
  deviceBrandId         (0),
  deviceProductId       (0),
  deviceProductVersion  (0)
{
}

/*!
  \brief Destructor
 */
Switch::Node::Configuration::~Configuration ()
{
}

/*!
  \brief Copy constructor

  \param[in] i_other Object to copy
 */
Switch::Node::Configuration::Configuration (const Switch::Node::Configuration& i_other)
: deviceAddress         (i_other.deviceAddress),
  deviceBrandId         (i_other.deviceBrandId),
  deviceProductId       (i_other.deviceProductId),
  deviceProductVersion  (i_other.deviceProductVersion)
{
}

/*!
  \brief Assignment operator

  \param[in] i_other Object to assign to this object
 */
Switch::Node::Configuration& Switch::Node::Configuration::operator= (const Switch::Node::Configuration& i_other)
{
  if (this != &i_other)
  {
    deviceAddress         = i_other.deviceAddress;
    deviceBrandId         = i_other.deviceBrandId;
    deviceProductId       = i_other.deviceProductId;
    deviceProductVersion  = i_other.deviceProductVersion;
  }

  return *this;
}

/*!
  \brief Constructor
 */
Switch::Node::CommunicationInfo::CommunicationInfo ()
: txAddress                 (0x0),
  lastCommunicationAttempt  (0),
  lastCommunicationTx       (0),
  lastCommunicationRx       (0),
  nrUnsuccessfulTxAttempts  (0),
  nrPingsSinceLastPong      (0)
{
}

/*!
  \brief Destructor
 */
Switch::Node::CommunicationInfo::~CommunicationInfo ()
{
}

/*!
  \brief Copy constructor

  \param[in] i_other Object to copy
 */
Switch::Node::CommunicationInfo::CommunicationInfo (const Switch::Node::CommunicationInfo& i_other)
: txAddress                 (i_other.txAddress),
  lastCommunicationAttempt  (i_other.lastCommunicationAttempt),
  lastCommunicationTx       (i_other.lastCommunicationTx),
  lastCommunicationRx       (i_other.lastCommunicationRx),
  nrUnsuccessfulTxAttempts  (i_other.nrUnsuccessfulTxAttempts),
  nrPingsSinceLastPong      (i_other.nrPingsSinceLastPong)
{
}

/*!
  \brief Assignment operator

  \param[in] i_other Object to assign to this object
 */
Switch::Node::CommunicationInfo& Switch::Node::CommunicationInfo::operator= (const Switch::Node::CommunicationInfo& i_other)
{
  if (this != &i_other)
  {
    txAddress                 = i_other.txAddress;
    lastCommunicationAttempt  = i_other.lastCommunicationAttempt;
    lastCommunicationTx       = i_other.lastCommunicationTx;
    lastCommunicationRx       = i_other.lastCommunicationRx;
    nrUnsuccessfulTxAttempts  = i_other.nrUnsuccessfulTxAttempts;
    nrPingsSinceLastPong      = i_other.nrPingsSinceLastPong;
  }

  return *this;
}

void Switch::Node::CommunicationInfo::SetTxAddress (const switch_pipe_address_type& i_txAddress)
{
  txAddress                 = i_txAddress;
  lastCommunicationAttempt  = 0;
  lastCommunicationTx       = 0;
  lastCommunicationRx       = 0;
  nrUnsuccessfulTxAttempts  = 0;
  nrPingsSinceLastPong      = 0;
}

/*!
  \brief Default constructor
 */
Switch::Node::Node (RF24& i_radio)
: m_networkAddress (0x0),
  m_rRadio (i_radio)
{
}

/*!
  \brief Destructor
 */
Switch::Node::~Node ()
{
}

/*!
  \brief Begins the node with the given configuration

  Resets all state variables and initializes the radio
 */
void Switch::Node::Begin (const Switch::Node::Configuration& i_configuration)
{
  // store the configuration
  m_configuration = i_configuration;
  m_virgin        = true;

  // clear all vairables
  _ResetVariables ();

  // initialize the radio
  m_rRadio.begin ();

  // configure the radio
  m_rRadio.setChannel (NC_NETWORK_CHANNEL);
  m_rRadio.setRetries (NC_RETRY_DELAY_MS, NC_NR_RETRIES);
  m_rRadio.setPayloadSize (NC_PAYLOAD_SIZE);
  m_rRadio.setPALevel (NC_POWER_AMPLIER_LEVEL);
  m_rRadio.setDataRate (NC_DATA_RATE);
  m_rRadio.setCRCLength (NC_CRC_LENGTH);
  m_rRadio.enableDynamicAcknowledgement (true);

  // open the reading pipes
  m_rRadio.openReadingPipe (0, _RxAddress (0));
  m_rRadio.openReadingPipe (1, _RxAddress (1));
  m_rRadio.openReadingPipe (2, _RxAddress (2));
  m_rRadio.openReadingPipe (3, _RxAddress (3));
  m_rRadio.openReadingPipe (4, _RxAddress (4));
  m_rRadio.openReadingPipe (5, _RxAddress (5));

  // start listening
  m_rRadio.startListening ();

  SWITCH_DEBUG (m_rRadio.printDetails ());
}

/*!
  \brief Updates the node

  Gives processing time to the node to process the rx queue and maintain the network connection.
  Must be called on a regular basis, especially when the node is not connected to the network.
 */
void Switch::Node::Update ()
{
  // listen for incoming messages
  _ListenAndDispatch ();

  // check if there is something else to do
  if (m_networkAddress == 0x0)
  {
    // check if it is time to broadcast
    if (Switch::NowInMilliseconds () - m_lastBroadcastTimeMs >= NODE_BROADCAST_INTERVAL_MS)
    {
      _Broadcast ();
    }
  }
  else
  {
    // check connection to parent
    _CheckConnectionToParent ();
  }
}

/*!
  \brief Checks if the node is connected to the root of the network

  \return True if the node is connected to the root of the network, false otherwise
 */
bool Switch::Node::IsConnected () const
{
  return (m_networkAddress.value != 0x0);
}

/*!
  \brief Gets the current size of the rx message queue
  \return Const reference to the queue count
 */
const uint8_t& Switch::Node::GetNrRxMessagesQueued () const
{
  return m_rxMessageQueueCount;
}

/*!
  \brief Gets the next available rx message payload

  \return Cosnt reference to the payload of the oldest available rx message in the queue
 */
const Switch::DataPayload& Switch::Node::GetRxMessagePayload () const
{
  SWITCH_ASSERT (0 != m_rxMessageQueueCount);

  return *reinterpret_cast_ptr <const Switch::DataPayload*> (m_rxMessageQueue [m_rxMessageQueueBegin].payload);
}

/*!
  \brief Releases the oldest message in the rx queue
 */
void Switch::Node::ReleaseRxMessage ()
{
  SWITCH_ASSERT (0 != m_rxMessageQueueCount);

  if (0 != m_rxMessageQueueCount)
  {
    m_rxMessageQueueBegin = (m_rxMessageQueueBegin + 1) % NODE_RX_MESSAGE_QUEUE_SIZE;
    --m_rxMessageQueueCount;
  }
}

/*!
  \brief Flushes all rx messsages from the queue

  Flushes the rx message queue and resets the indices of the queue.
  All previously received messages are lost.
 */
void Switch::Node::FlushRxMessageQueue ()
{
  m_rxMessageQueueBegin = 0;
  m_rxMessageQueueEnd   = 0;
  m_rxMessageQueueCount = 0;
}

/*!
  \brief Transmits data to the root of the network

  \param[in] i_dataPayload The data payload to send to the root of the network
 */
bool Switch::Node::TransmitData (const Switch::DataPayload& i_dataPayload)
{
  SWITCH_ASSERT (m_networkAddress != 0x0);

  // create the message to transmit
  m_bufferMessage.header.toNetworkAddress   = 0x0;
  m_bufferMessage.header.fromNetworkAddress = m_networkAddress;
  m_bufferMessage.header.messageType   	    = MT_DATA;

  // copy the payload
  SWITCH_ASSERT (sizeof (Switch::DataPayload) <= NM_MAX_PAYLOAD_SIZE);
  memcpy (m_bufferMessage.payload, &i_dataPayload, NM_MAX_PAYLOAD_SIZE);

  return _SendMessageTo (0, m_bufferMessage);
}

/*!
  \brief Gets a pointer to the next free message in the rx message queue

  \returns A pointer to the next free message in the rx message queue or 0x0 if the queue is full
 */
Switch::NetworkMessage* Switch::Node::_GetFreeRxMessagePointer ()
{
  if (NODE_RX_MESSAGE_QUEUE_SIZE == m_rxMessageQueueCount)
  {
    SWITCH_DEBUG_MSG_0 ("Rx message queue full\n");
    return 0x0;
  }

  Switch::NetworkMessage* pFreeRxMessage = &m_rxMessageQueue [m_rxMessageQueueEnd];
  m_rxMessageQueueEnd = (m_rxMessageQueueEnd + 1) % NODE_RX_MESSAGE_QUEUE_SIZE;
  ++m_rxMessageQueueCount;

  return pFreeRxMessage;
}

/*!
  \brief Resets all variables
 */
void Switch::Node::_ResetVariables ()
{
  SWITCH_DEBUG_MSG_0 ("resetting variables\n");

  m_networkAddress = 0x0;
  for (uint8_t i=0; i<(1+NODE_MAX_NR_CHILD_NODES); ++i)
  {
    m_txCommunicationPipes [i].SetTxAddress (0x0);
  }
  m_lastBroadcastTimeMs = 0;
  FlushRxMessageQueue ();

  // note: don't mark the node as virgin as this is only the case when begin () is called
}

/*!
  \brief Checks the connection with the parent

  Resets the node if too many communication attempts have failed
 */
void Switch::Node::_CheckConnectionToParent ()
{
  uint32_t now = NowInMilliseconds ();

  // check communication with parent
  if ((NODE_MAX_NR_COMMUNICATION_FAULTS <= m_txCommunicationPipes [0].nrUnsuccessfulTxAttempts) ||
      (NODE_MAX_NR_COMMUNICATION_FAULTS <= m_txCommunicationPipes [0].nrPingsSinceLastPong))
  // exclude all children
  {
    // prepare the node exclusion message
    m_bufferMessage.header.fromNetworkAddress = m_networkAddress;
    m_bufferMessage.header.toNetworkAddress   = m_networkAddress;
    m_bufferMessage.header.messageType        = MT_NODE_EXCLUSION;
    Switch::NodeExclusionPayload* pPayload = reinterpret_cast <Switch::NodeExclusionPayload*> (m_bufferMessage.payload);
    pPayload->nodeToExclude = NE_ALL_CHILDREN;

    uint8_t branchIndex = m_networkAddress.GetBranchIndex ();

    for (uint8_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
    {
      if (0x0 != m_txCommunicationPipes [i + 1].txAddress)
      {
        // set the network address of the child node
        m_bufferMessage.header.toNetworkAddress.SetBranchIndex (branchIndex + 1);
        m_bufferMessage.header.toNetworkAddress.SetChildIndex  (branchIndex, i);

        // forward message to child
        _SendMessageTo (i + 1, m_bufferMessage);
      }
    }

    // forget all we know
    _ResetVariables ();
  }
  else
  {
    if (((NODE_MAX_PARENT_COMMUNICATION_SILENCE_MS <= (now - m_txCommunicationPipes [0].lastCommunicationRx)) ||
         (NODE_MAX_PARENT_COMMUNICATION_SILENCE_MS <= (now - m_txCommunicationPipes [0].lastCommunicationTx))) &&
        (NODE_MAX_TXRX_COMMUNICATION_SILENCE_MS <= (now - m_txCommunicationPipes [0].lastCommunicationAttempt)))
    {
      // create a ping message
      m_bufferMessage.header.fromNetworkAddress = m_networkAddress;
      m_bufferMessage.header.toNetworkAddress   = m_networkAddress;
      uint8_t parentBranchIndex = m_networkAddress.GetBranchIndex () - 1;
      m_bufferMessage.header.toNetworkAddress.SetBranchIndex (parentBranchIndex);
      m_bufferMessage.header.toNetworkAddress.SetChildIndex (parentBranchIndex, 0);
      m_bufferMessage.header.messageType        = MT_PING;

      // configure the payload
      Switch::PingPongPayload* pPayload = reinterpret_cast <Switch::PingPongPayload*> (m_bufferMessage.payload);
      pPayload->transmissionStartTimeMs = now;

      // send
      bool result = _SendMessageTo (0, m_bufferMessage);
      if (result)
      {
        ++m_txCommunicationPipes [0].nrPingsSinceLastPong;
      }
    }
  }
}

/*!
  \brief Reads all messages from the radio's rx buffer and dispatches the messages to handler functions
 */
void Switch::Node::_ListenAndDispatch ()
{
  // helper vairables
  uint8_t pipeNr;

  // loop until no more messages are available on the rx fifo
  // and at most read NODE_MAX_NR_CONSECUTIVE_RX_READS messages
  for (uint8_t i=0;  m_rRadio.available (&pipeNr) && (i<NODE_MAX_NR_CONSECUTIVE_RX_READS); ++i)
  {
    SWITCH_ASSERT_RETURN_0 ((pipeNr >=0) && (pipeNr < 6));

    // read a buffer message
    m_rRadio.read (&m_bufferMessage, sizeof (m_bufferMessage));
    SWITCH_DEBUG_MSG_4 ("incoming message %u on pipe %u from 0x%04x to 0x%04x\n", i, pipeNr, m_bufferMessage.header.fromNetworkAddress.value, m_bufferMessage.header.toNetworkAddress.value);

    if (MT_ADDRESS_ASSIGNMENT == m_bufferMessage.header.messageType)
    // handle address assignment payload
    {
      // this message may only come from the first pipe
      SWITCH_ASSERT (1 == pipeNr);

      // cast the payload to the correct type
      Switch::NodeAssignmentPayload* pPayload = reinterpret_cast <Switch::NodeAssignmentPayload*> (m_bufferMessage.payload);

      if (pPayload->nodeDeviceAddress == m_configuration.deviceAddress)
      // assignment for this node
      {
        // get the network address and tx address to the parent node
        m_networkAddress = pPayload->nodeNetworkAddress;
        m_txCommunicationPipes [0].SetTxAddress (pPayload->communicationPipeAddress);
        m_virgin = false;
      }
      else
      {
        // this node's network address can not be 0x0 if messages are directed to it for other nodes
        SWITCH_ASSERT (0x0 != m_networkAddress.value);

        // get this node's branch index
        uint8_t branchIndex = m_networkAddress.GetBranchIndex ();
        uint8_t childIndex  = 255;

        if (m_bufferMessage.header.toNetworkAddress == m_networkAddress)
        // assignment of a new child to this node
        {
          // get the new child index
          childIndex = pPayload->nodeNetworkAddress.GetChildIndex (branchIndex);

          // set the new child tx address
          // note: possibly txAddress of child at childIndex is not null, but it is decided by the router that no messages
          //       will pass through that route
          m_txCommunicationPipes [childIndex + 1].SetTxAddress (pPayload->communicationPipeAddress);

          // fill in the rx address in the payload, the child node's network address and sender's network address in the header
          pPayload->communicationPipeAddress = _RxAddress (childIndex + 2);
          m_bufferMessage.header.toNetworkAddress   = pPayload->nodeNetworkAddress;
          m_bufferMessage.header.fromNetworkAddress = m_networkAddress;
        }
        else
        // the message is not for us
        {
          // get the child index
          childIndex  = m_bufferMessage.header.toNetworkAddress.GetChildIndex (branchIndex);
        }

        // forward to child
        SWITCH_ASSERT_RETURN_0 (255 != childIndex);
        _SendMessageTo (childIndex + 1, m_bufferMessage);
      }
    }
    else if (0x0 != m_networkAddress.value)
    {
      // get the current time
      uint64_t timeNow = NowInMilliseconds ();

      if (0 == pipeNr)
      // broadcast messages
      {
        SWITCH_ASSERT_RETURN_0 (MT_BROADCAST == m_bufferMessage.header.messageType);

        // fill in the address
        m_bufferMessage.header.fromNetworkAddress = m_networkAddress;

        // forward the broadcast message to the parent node
        _SendMessageTo (0, m_bufferMessage);
      }
      else
      // messages from other nodes
      {
        // mark the communication in the stats
        m_txCommunicationPipes [pipeNr - 1].lastCommunicationRx = timeNow;

        if (m_bufferMessage.header.toNetworkAddress != m_networkAddress)
        // the message is not for us
        {
          // check if the message must be sent downstream or upstream
          // => upstream only if branchlevel > than ours and the subnodes coincide
          uint8_t branchIndex = m_networkAddress.GetBranchIndex ();
          uint16_t mask = !(0xFFFF << 2*branchIndex);

          if ((m_bufferMessage.header.toNetworkAddress.GetBranchIndex () > branchIndex) &&
              (m_bufferMessage.header.toNetworkAddress.value & mask) == (m_networkAddress.value & mask))
          // send in direction of children
          {
            // get the child index
            uint8_t childIndex = m_bufferMessage.header.toNetworkAddress.GetChildIndex (branchIndex);

            // forward the message to the corresponding node
            _SendMessageTo (childIndex + 1, m_bufferMessage);
          }
          else
          // send in direction of router
          {
            _SendMessageTo (0, m_bufferMessage);
          }
        }
        else // m_bufferMessage.header.toNetworkAddress == m_networkAddress
        // the message is for us
        {
          if (MT_NODE_EXCLUSION == m_bufferMessage.header.messageType)
          {
            // cast the payload to the correct type
            Switch::NodeExclusionPayload* pPayload = reinterpret_cast <Switch::NodeExclusionPayload*> (m_bufferMessage.payload);

            if (NE_ALL_CHILDREN == pPayload->nodeToExclude)
            {
              // exclude all children
              for (uint8_t j=0; j<NODE_MAX_NR_CHILD_NODES; ++j)
              {
                if (0x0 != m_txCommunicationPipes [j + 1].txAddress)
                {
                  // forward message to child
                  m_bufferMessage.header.fromNetworkAddress = m_networkAddress;
                  uint8_t branchIndex = m_networkAddress.GetBranchIndex ();
                  m_bufferMessage.header.toNetworkAddress = m_networkAddress;
                  m_bufferMessage.header.toNetworkAddress.SetBranchIndex (branchIndex + 1);
                  m_bufferMessage.header.toNetworkAddress.SetChildIndex (branchIndex, i);
                  _SendMessageTo (j + 1, m_bufferMessage);
                }
              }

              // forget all we know
              _ResetVariables ();
            }
            else if (NE_NONE != pPayload->nodeToExclude)
            {
              // alter the payload such that the child excludes all its own children
              pPayload->nodeToExclude = NE_ALL_CHILDREN;

              uint8_t childIndex = (pPayload->nodeToExclude - NE_CHILD_0);
              SWITCH_ASSERT (childIndex < NODE_MAX_NR_CHILD_NODES);

              // send to the right child
#ifndef ARDUINO
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
              if (0x0 != m_txCommunicationPipes [childIndex + 1].txAddress)
#pragma GCC diagnostic pop
#else
              if (0x0 != m_txCommunicationPipes [childIndex + 1].txAddress)
#endif
              {
                // forward message to child
                m_bufferMessage.header.fromNetworkAddress = m_networkAddress;
                uint8_t branchIndex = m_networkAddress.GetBranchIndex ();
                m_bufferMessage.header.toNetworkAddress = m_networkAddress;
                m_bufferMessage.header.toNetworkAddress.SetBranchIndex (branchIndex + 1);
                m_bufferMessage.header.toNetworkAddress.SetChildIndex (branchIndex, childIndex);
                _SendMessageTo (childIndex + 1, m_bufferMessage);
              }

              // forget about the child
              m_txCommunicationPipes [childIndex + 1].SetTxAddress (0x0);
            }
            SWITCH_DEBUG_IF (NE_NONE == pPayload->nodeToExclude, SWITCH_DEBUG_MSG_0 ("node exclusion with content NE_NONE received\n"));
          }
          else if (MT_PING == m_bufferMessage.header.messageType)
          {
            // translate into pong
            m_bufferMessage.header.messageType = MT_PONG;
            m_bufferMessage.header.toNetworkAddress = m_bufferMessage.header.fromNetworkAddress;
            m_bufferMessage.header.fromNetworkAddress = m_networkAddress;

            // return message
            _SendMessageTo (pipeNr - 1, m_bufferMessage);
          }
          else if (MT_PONG == m_bufferMessage.header.messageType)
          {
            // what was the time difference to when the ping was sent
            Switch::PingPongPayload* pPayload = reinterpret_cast <Switch::PingPongPayload*> (m_bufferMessage.payload);

            m_txCommunicationPipes [pipeNr - 1].nrPingsSinceLastPong = 0;

            SWITCH_DEBUG (uint64_t pingPongTimeMs = Switch::NowInMilliseconds () - pPayload->transmissionStartTimeMs;
            SWITCH_DEBUG_MSG_1 ("ping pong transmission time %lu\n", pingPongTimeMs););
          }
          else if (MT_DATA == m_bufferMessage.header.messageType)
          // put the message in the rx queue
          {
            // get a vacant slot in the rx message queue
            Switch::NetworkMessage* pRxMessageQueueSlot = _GetFreeRxMessagePointer ();

            // make sure the slot is empty
            SWITCH_ASSERT (0x0 != pRxMessageQueueSlot);
            if (0x0 != pRxMessageQueueSlot)
            {
              // copy the message into the slot
              memcpy (pRxMessageQueueSlot, &m_bufferMessage, sizeof (Switch::NetworkMessage));
            }
          }
          else
          {
            SWITCH_DEBUG_MSG_1 ("unknown message type received: 0x%02x\n", m_bufferMessage.header.messageType);
          }
        }
      }
    }
    else
    {
      SWITCH_DEBUG_MSG_0 ("not assigned\n");
    }
  }
}

/*!
  \brief Broadcasts the tokens of this node over the broadcast channel
 */
bool Switch::Node::_Broadcast ()
{
  SWITCH_ASSERT_RETURN_1 (m_networkAddress == 0x0, false);

  // create the message to transmit
  m_bufferMessage.header.toNetworkAddress   = 0x0;
  m_bufferMessage.header.fromNetworkAddress = 0x0;
  m_bufferMessage.header.messageType        = MT_BROADCAST;

  // compose the broadcast message
  SWITCH_ASSERT (sizeof (Switch::BroadcastPayload) <= NM_MAX_PAYLOAD_SIZE);
  Switch::BroadcastPayload* pBroadcast = reinterpret_cast <Switch::BroadcastPayload*> (m_bufferMessage.payload);
  pBroadcast->broadcastNodeDeviceAddress  = m_configuration.deviceAddress;
  pBroadcast->broadcastNodeRxPipeAddress  = _RxAddress (1);
  pBroadcast->deviceInfo.brandId          = m_configuration.deviceBrandId;
  pBroadcast->deviceInfo.productId        = m_configuration.deviceProductId;
  pBroadcast->deviceInfo.productVersion   = m_configuration.deviceProductVersion;
  if (m_virgin)
  {
    pBroadcast->broadcastReason = BROADCAST_REASON_BEGIN;
  }
  else
  {
    pBroadcast->broadcastReason = BROADCAST_REASON_PARENT_LOST;
  }

  // prepare to write
  m_rRadio.stopListening ();
  m_rRadio.openWritingPipe (NC_BROADCAST_PIPE);

  // write without requiring an acknowledgement
  bool result = m_rRadio.write (&m_bufferMessage, sizeof (m_bufferMessage), false);

  // keep track of the time
  m_lastBroadcastTimeMs = Switch::NowInMilliseconds ();

  // resume listening
  m_rRadio.startListening ();

  SWITCH_DEBUG_IF (result, SWITCH_DEBUG_MSG_0 ("broadcast transmitted\n"));
  SWITCH_DEBUG_IF (!result, SWITCH_DEBUG_MSG_0 ("broadcast failed\n"));

  return result;
}

/*!
  \brief Sends a message to a known receiver

  \param[in] i_receiverIndex Index of the known receiver. The receiver at index 0 is the parent node
  and must always be known when this node has a network address.
  \param[in] i_rxMessage The message to send.
 */
bool Switch::Node::_SendMessageTo (const uint8_t& i_receiverIndex, const Switch::NetworkMessage& i_txMessage)
{
  SWITCH_DEBUG_MSG_1 ("send message to %u ...", i_receiverIndex);
  SWITCH_ASSERT_RETURN_1 (m_networkAddress != 0x0, false);
  SWITCH_ASSERT_RETURN_1 (0x0 != m_txCommunicationPipes [i_receiverIndex].txAddress, false);

  // get the current time
  uint64_t timeNow = NowInMilliseconds ();

  // get a reference to the receiver
  CommunicationInfo& receiver = m_txCommunicationPipes [i_receiverIndex];

  // prepare to write
  m_rRadio.stopListening ();
  m_rRadio.openWritingPipe (receiver.txAddress);

  // write with auto acknowledgement enabled
  bool result = m_rRadio.write (&i_txMessage, sizeof (i_txMessage), true);

  // resume listening
  m_rRadio.startListening ();

  // update the node communication stats
  receiver.lastCommunicationAttempt = timeNow;
  if (result)
  {
    receiver.nrUnsuccessfulTxAttempts = 0;
    receiver.lastCommunicationTx = timeNow;
    SWITCH_DEBUG_MSG_0 (" transmitted\n");
  }
  else
  {
    ++receiver.nrUnsuccessfulTxAttempts;
    SWITCH_DEBUG_MSG_1 (" %u attempts in a row failed\n", receiver.nrUnsuccessfulTxAttempts);
  }

  return result;
}

/*!
  \brief Computes the rx address given the pipe index

  On pipe 0, the rx address always equals the broadcast channel
  On the other pipes, the rx address equals the node address with one byte added depending on the pipe number

  \param [in] i_pipeIndex The index of the rx pipe to get the address for

  \return The rx address of the pipe at given index
 */
switch_pipe_address_type Switch::Node::_RxAddress (const uint8_t& i_pipeIndex)
{
  static uint8_t pipeRxBytes [] = { 0x5a, 0x69, 0x96, 0xa5, 0xc3 };

  uint64_t resultAddress;
  if (0 == i_pipeIndex)
  {
    resultAddress = NC_BROADCAST_PIPE;
  }
  else
  {
    resultAddress = m_configuration.deviceAddress;
    resultAddress  <<= 8;
    resultAddress |= pipeRxBytes [i_pipeIndex - 1];
  }
  return resultAddress;
}

