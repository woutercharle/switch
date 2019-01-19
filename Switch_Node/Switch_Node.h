/*?*************************************************************************
*                           Switch_Node.h
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

#ifndef _SWITCH_NODE
#define _SWITCH_NODE

// project includes
#include "Switch_NodeConfiguration.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"
#include "../Switch_Network/Switch_NetworkMessage.h"
#include "../Switch_Network/Switch_DataPayload.h"

// forward declarations
class RF24;


namespace Switch
{
  class Node
  {
  public:

	/*!
	  \brief Configuration parameters container class

	  Contains all node-specific parameters needed to configure a node.
	  These parameters are different for every node and are for example stored
	  in EEPROM or determined by the program's control loop.
	 */
    class Configuration
    {
    public:

      /*!
        \brief Constructor
       */
      Configuration ();

      /*!
        \brief Destructor
       */
      ~Configuration ();

      /*!
        \brief Copy constructor

        \param[in] i_other Object to copy
       */
      Configuration (const Configuration& i_other);

      /*!
        \brief Assignment operator

        \param[in] i_other Object to assign to this object
       */
      Configuration& operator= (const Configuration& i_other);

      // members
      switch_device_address_type  deviceAddress;        ///< The unique device address
      switch_brand_id_type        deviceBrandId;        ///< The id of the brand of this device
      switch_product_id_type      deviceProductId;      ///< The id of the product of this device
      switch_product_version_type deviceProductVersion; ///< The version of the product of this device
    };

    /*!
      \brief Constructor

	  \param[in] i_radio The node's radio to communicate with the rest of the network
     */
    Node (RF24& i_radio);

    /*!
      \brief Destructor
     */
    ~Node ();

    /*!
      \brief Initializes the node

      \param[in] i_configuration The node's configuration
     */
    void Begin (const Configuration& i_configuration);

    /*!
      \brief Updates the node

      Gives processing time to the node to process the rx queue and maintain the network connection
     */
    void Update ();

    /*!
      \brief Checks if the node is connected to the root of the network

      \return True if the node is connected to the root of the network, false otherwise
     */
    bool IsConnected () const;

    /*!
      \brief Gets the current size of the rx message queue

      \return Const reference to the queue count
     */
    const uint8_t& GetNrRxMessagesQueued () const;

    /*!
      \brief Gets the next available rx message

      \return Cosnt reference to the payload of the oldest available rx message in the queue
     */
    const Switch::DataPayload& GetRxMessagePayload () const;

    /*!
      \brief Releases the oldest message in the rx queue
     */
    void ReleaseRxMessage ();

    /*!
      \brief Flushes all rx messsages from the queue

      Flushes the rx message queue and resets the indices of the queue.
      All previously received messages are lost.
     */
    void FlushRxMessageQueue ();

	/*!
	  \brief Sends data to the root of the network

	  \param[in] i_dataPayload The data payload to send to the root of the network

	  \return True if succeeded, false otherwise

	  \note Requires that the node is connected to the root
	 */
	bool TransmitData (const Switch::DataPayload& i_dataPayload);

  private:

    /*!
     */
    class CommunicationInfo
    {
    public:
      /*!
        \brief Constructor
       */
      CommunicationInfo ();

      /*!
        \brief Destructor
       */
      ~CommunicationInfo ();

      /*!
        \brief Copy constructor

        \param[in] i_other Object to copy
       */

      CommunicationInfo (const CommunicationInfo& i_other);

      /*!
        \brief Assignment operator

        \param[in] i_other Object to assign to this object
       */
      CommunicationInfo& operator= (const CommunicationInfo& i_other);

      /*!
        \brief Sets the tx address for communication

        \param[in] i_txAddress The tx address for communication
       */
      void SetTxAddress (const switch_pipe_address_type& i_txAddress);

      // members
      switch_pipe_address_type txAddress;                 ///< The address to which to send a message
      uint32_t                 lastCommunicationAttempt;  ///< Time on which it was last tried to send a message
      uint32_t                 lastCommunicationTx;       ///< Keeps track of the last time messages were sent
      uint32_t                 lastCommunicationRx;       ///< Keeps track of the last time messages were received
      uint8_t                  nrUnsuccessfulTxAttempts;  ///< Counts the nr of consequtive unsuccesfull attempts to send a message
      uint8_t                  nrPingsSinceLastPong;      ///< Counts the nr of pings sent since the last pong was received
    };

    // copy constructor and assignment operator are never to be used
    Node (const Node& i_other);
    Node& operator= (const Node& i_other);

    // helper methods
    void _ResetVariables ();
    void _CheckConnectionToParent ();
    void _ListenAndDispatch ();
    bool _Broadcast ();
    bool _SendMessageTo (const uint8_t& i_receiverIndex, const Switch::NetworkMessage& i_txMessage);
    switch_pipe_address_type _RxAddress (const uint8_t& i_pipeIndex);
    Switch::NetworkMessage* _GetFreeRxMessagePointer ();

    // variables
    Switch::NetworkAddress  m_networkAddress;                                   ///< Network address of the node
    CommunicationInfo       m_txCommunicationPipes [1+NODE_MAX_NR_CHILD_NODES]; ///< Array of communication information for each pipe
    uint64_t                m_lastBroadcastTimeMs;                              ///< The last time a broadcast message was sent by this node
    Switch::NetworkMessage  m_bufferMessage;                                    ///< Pre-allocated buffer message used to buffer reads and writes to the radio
    Switch::NetworkMessage  m_rxMessageQueue [NODE_RX_MESSAGE_QUEUE_SIZE];      ///< Pre-allocated queue of incoming data messages
    uint8_t                 m_rxMessageQueueEnd;                                ///< Pointer to the end index of the incoming data message queue
    uint8_t                 m_rxMessageQueueBegin;                              ///< Pointer to the begin index of the incoming data message queue
    uint8_t                 m_rxMessageQueueCount;                              ///< The nr of data messages currently in the queue
    bool                    m_virgin;                                           ///< Flags whether this node has never before been assigned (true) or not (false)

    // members
    Configuration m_configuration;  ///< The node's configuration
    RF24&         m_rRadio;         ///< Reference to the radio used for communication
  };
}

#endif // _SWITCH_NODE
