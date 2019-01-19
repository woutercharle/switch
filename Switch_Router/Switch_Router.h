/*?*************************************************************************
*                           Switch_Router.h
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

#ifndef _SWITCH_ROUTER
#define _SWITCH_ROUTER


// switch includes
#include "Switch_RouterConfiguration.h"
#include "Switch_RouterNetworkModel.h"

#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"
#include "../Switch_Application/Switch_ApplicationModule.h"
#include "../Switch_Network/Switch_NetworkAddress.h"
#include "../Switch_Network/Switch_NetworkMessage.h"
#include "../Switch_Network/Switch_DataPayload.h"

// third party includes
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

// forward declarations
class RF24;

namespace Switch
{
  class BroadcastPayload;
}


namespace Switch
{
  class Router : public Switch::ApplicationModule
  {
  public:

    /*!
      \brief Parameters container class

      Contains all router-specific parameters needed to configure a router.
      These parameters are different for every node and are for example stored
      on disk or determined by the program's control loop.
     */
    class Parameters : public Switch::ApplicationModule::Parameters
    {
    public:

      /*!
        \brief Constructor
       */
      Parameters ();

      /*!
        \brief Destructor
       */
      ~Parameters ();

      Parameters (const Parameters& i_other) = default;
      Parameters& operator= (const Parameters& i_other) = default;

      // members
      switch_device_address_type m_deviceAddress;     ///< The router's device address.
      uint8_t     m_minNodeHearingCountBeforeRouting; ///< The minimum number of times a node must be heared before it is routed.
      uint8_t     m_maxNrNodesRoutedSimultaneously;   ///< The maximum number of nodes that may be routed in one update.
      uint8_t     m_maxNrTxMessagesHandledInOneCycle; ///< The maximum number of tx data messages transmitted to the nodes during one cycle.
      uint32_t    m_updateCycleTimeMicros;            ///< The time in microseconds between two update cycles.
      std::string m_spiDevice;                        ///< SPI device identifier on the system.
      uint32_t    m_spiSpeed;                         ///< Speed of the SPI interface.
      uint8_t     m_cePin;                            ///< GPIO pin to use for the "Chip Enable" signal.
    };

    /*!
      \brief Abstract base class for event handlers
     */
    class EventHandler
    {
    public:

      typedef std::function <void (const switch_device_address_type&, const Switch::DeviceInfo&)>   NodeDiscoveredCallback;
      typedef std::function <void (const switch_device_address_type&, const bool&)>                 NodeConnectionUpdateCallback;
      typedef std::function <bool (const switch_device_address_type&, const Switch::DataPayload&)>  NodeDataReceivedCallback;
      typedef std::function <void (const switch_device_address_type&, const bool&)>                 NodeDataTransmittedCallback;

      /*!
        \brief Constructor
       */
      EventHandler (const NodeDiscoveredCallback& i_newNodeDiscoveredCallback=nullptr,
                    const NodeConnectionUpdateCallback& i_nodeConnectionUpdateCallback=nullptr,
                    const NodeDataReceivedCallback& i_nodeDataReceivedCallback=nullptr,
                    const NodeDataTransmittedCallback& i_nodeDataTransmittedCallback=nullptr);
      /*!
        \brief Destructor
       */
      virtual ~EventHandler ();

      // using default copy-constructor and assignment-operator
      EventHandler (const EventHandler& i_other) = default;
      EventHandler& operator= (const EventHandler& i_other) = default;

      /*!
        \brief Signals that a new node was disconvered in the network with specified device address.

        \param [in] i_deviceAddress         The device address of the new node.
        \param [in] i_deviceBrandId         The id of the brand of the device
        \param [in] i_deviceProductId       The id of the product the devide is
        \param [in] i_deviceProductVersion  The version of the product
       */
      void NewNodeDiscovered (const switch_device_address_type& i_deviceAddress, const Switch::DeviceInfo& i_deviceInfo);
      /*!
        \brief Signals that a node's connection status has changed

        \param [in] i_deviceAddress The device address of the node.
        \param [in] i_connected The new connection status. True if connected, false otherwise.
       */
      void NodeConnectionUpdate (const switch_device_address_type& i_deviceAddress, const bool& i_connected);
      /*!
        \brief Signals that data was received from a node

        \param [in] i_deviceAddress The device address of the node.
        \param [in] i_dataPayload Const reference to the received data payload

        \return True if the data may be released, false otherwise.
       */
      bool NodeDataReceived (const switch_device_address_type& i_deviceAddress, const Switch::DataPayload& i_dataPayload);
      /*!
        \brief Signals that data was transmitted to a node

        \param [in] i_deviceAddress The device address of the node.
        \param [in] i_result Flags if the transmission was successfull (true) or not (false).
       */
      void NodeDataTransmitted (const switch_device_address_type& i_deviceAddress, const bool& i_result);

    private:

      // callback functions
      NodeDiscoveredCallback        m_newNodeDiscoveredCallback;
      NodeConnectionUpdateCallback  m_nodeConnectionUpdateCallback;
      NodeDataReceivedCallback      m_nodeDataReceivedCallback;
      NodeDataTransmittedCallback   m_nodeDataTransmittedCallback;

    };

    /*!
      \brief Default constructor.
     */
    Router ();
    /*!
      \brief Constructor.
     */
    explicit Router (const Parameters& i_parameters);
    /*!
      \brief Destructor
     */
    virtual ~Router ();

    // copy constructor and assignment operator disabled
    Router (const Router& i_other) = delete;
    Router& operator= (const Router& i_other) = delete;

    /*!
      \brief Sets the event handler to which events will be posted.

      \param [in] i_pEventHandler Pointer to the event handler or NULL.
     */
    void SetEventHandler (const EventHandler& i_eventHandler);

    /*!
      \brief Updates the router

      Gives processing time to the router to process the rx queue and maintain the network connection
     */
    void Update ();

    /*!
      \brief Checks if the router is connected to a node in the network

      \param[in] i_deviceAddress Address of the node to check the connection with

      \return True if the router is connected to the device in the network, false otherwise
     */
    bool IsConnected (const switch_device_address_type& i_deviceAddress) const;

    /*!
     \brief Enables routing of the node with specified device address

     \param[in] i_deviceAddress Address of the node for which to enable routing
     */
    void EnableNodeRouting (const switch_device_address_type& i_deviceAddress);

    /*!
      \brief Gets the current size of the rx message queue

      \return Const reference to the queue count
     */
    const uint8_t& GetNrRxMessagesQueued () const;

    /*!
      \brief Gets the next available rx message

      \param[out] o_deviceAddress Address of the device that sent the payload.

      \return Cosnt reference to the payload of the oldest available rx message in the queue
     */
    const Switch::DataPayload& GetRxMessagePayload (switch_device_address_type& o_deviceAddress) const;

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

      \param[in] i_nodeDeviceAddress
      \param[in] i_dataPayload The data payload to send to the root of the network

      \note Requires that the node is connected to the root
     */
    void TransmitData (const switch_device_address_type& i_nodeDeviceAddress, const Switch::DataPayload& i_dataPayload);

  protected:

    void _SetupParameterContainer ();
    virtual void _SetParameters (const Switch::ParameterizedObject::Parameters& i_parameters);
    virtual void _GetParameters (Switch::ParameterizedObject::Parameters& o_parameters) const;

    virtual void _Prepare ();
    virtual void _Start   ();
    virtual void _Pause   ();
    virtual void _Stop    ();

  private:

    /*!
      \brief Communication information container class
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
      uint64_t                 lastCommunicationAttempt;  ///< Time on which it was last tried to send a message
      uint64_t                 lastCommunication;         ///< Keeps track of the last time messages were sent or received
      uint8_t                  nrUnsuccessfulTxAttempts;  ///< Counts the nr of consequtive unsuccesfull attempts to send a message
    };

    // helper methods
    void _Run ();
	  void _ListenAndDispatch ();
    void _RouteUnassignedNodes ();
    void _CheckConnections ();
    void _HandleEnableNodeRoutingData ();
    void _HandleTransmitData ();

    void _ReleaseRxMessage ();

    bool _SendMessageTo (const uint8_t& i_receiverIndex, const Switch::NetworkMessage& i_txMessage);
    switch_pipe_address_type _RxAddress (const uint8_t& i_pipeIndex);
    Switch::NetworkMessage* _GetFreeRxMessagePointer ();

    // variables
    CommunicationInfo           m_txCommunicationPipes [ROUTER_MAX_NR_CHILD_NODES];
    Switch::NetworkMessage      m_bufferMessage;
    Switch::NetworkMessage      m_rxMessageQueue [ROUTER_RX_MESSAGE_QUEUE_SIZE];
    uint8_t                     m_rxMessageQueueEnd;
    uint8_t                     m_rxMessageQueueBegin;
    uint8_t                     m_rxMessageQueueCount;
    EventHandler                m_eventHandler;

    // members
    RF24*                       m_pRadio;
    Switch::RouterNetworkModel* m_pNetworkModel;

    // threading variables
    std::atomic <eObjectState>              m_routerState;
    std::thread                             m_routerThread;
    mutable std::mutex                      m_routerMutex;
    std::condition_variable                 m_updateCondition;
    mutable std::mutex                      m_rxMessageQueueMutex;
    mutable std::mutex                      m_networkModelMutex;

    mutable std::mutex                      m_dataEnableNodeRoutingMutex;
    mutable std::mutex                      m_dataTransmitDataMutex;

    // threading data
    std::list <switch_device_address_type>                                  m_dataEnableNodeRouting;
    std::list <std::pair <switch_device_address_type, Switch::DataPayload>> m_dataTransmitData;

    // parameters
    switch_device_address_type m_deviceAddress;     ///< The router's device address.
    uint8_t     m_minNodeHearingCountBeforeRouting; ///< The minimum number of times a node must be heared before it is routed.
    uint8_t     m_maxNrNodesRoutedSimultaneously;   ///< The maximum number of nodes that may be routed in one update.
    uint8_t     m_maxNrTxMessagesHandledInOneCycle; ///< The maximum number of tx data messages transmitted to the nodes during one cycle.
    uint32_t    m_updateCycleTimeMicros;            ///< The time in microseconds between two update cycles.
    std::string m_spiDevice;                        ///< SPI device identifier on the system.
    uint32_t    m_spiSpeed;                         ///< Speed of the SPI interface.
    uint8_t     m_cePin;                            ///< GPIO pin to use for the "Chip Enable" signal.
  };
}

#endif // _SWITCH_NODE
