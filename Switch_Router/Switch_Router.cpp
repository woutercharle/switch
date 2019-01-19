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

#include "Switch_Router.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"
#include "../Switch_Base/Switch_Utilities.h"
#include "../Switch_Network/Switch_NetworkConfiguration.h"
#include "../Switch_Network/Switch_BroadcastPayload.h"
#include "../Switch_Network/Switch_NodeAssignmentPayload.h"
#include "../Switch_Network/Switch_NodeExclusionPayload.h"
#include "../Switch_Network/Switch_PingPongPayload.h"

// thirdparty includes
#ifdef ARDUINO
#  include "../RF24/RF24.h"
#else
#  include <RF24.h>
#endif

// std includes
#include <limits>


/*!
  \brief Constructor
 */
Switch::Router::Parameters::Parameters ()
: Switch::ApplicationModule::Parameters ()
{
  m_deviceAddress                     = 0x0;
  m_minNodeHearingCountBeforeRouting  = 1;
  m_maxNrNodesRoutedSimultaneously    = 1;
  m_maxNrTxMessagesHandledInOneCycle  = 3;
  m_updateCycleTimeMicros				      = 200000;
  m_spiDevice                         = "/dev/spidev0.0";
  m_spiSpeed                          = 8000000;
  m_cePin                             = 25;
}

/*!
  \brief Destructor
 */
Switch::Router::Parameters::~Parameters ()
{
}

void Switch::Router::_SetupParameterContainer ()
{
  // set container properties
  _SetContainerName ("Router");
  _SetContainerDescription ("Parameters of the router.");

  // add parameters
  Parameters myParameters;
  _AddParameter (myParameters, myParameters.m_deviceAddress,                    "Device address", "The router's device address.", "General");
  _AddParameter (myParameters, myParameters.m_minNodeHearingCountBeforeRouting, "Min. hearing count", "The minimum number of times a node must be heared before it is routed.", "Routing");
  _AddParameter (myParameters, myParameters.m_maxNrNodesRoutedSimultaneously,   "Max. nr. unknown devices", "The maximum number of nodes that may be routed in one update.", "Routing");
  _AddParameter (myParameters, myParameters.m_maxNrTxMessagesHandledInOneCycle, "Max. nr. tx messages per cycle", "The maximum number of tx data messages transmitted to the nodes during one cycle.", "Routing");
  _AddParameter (myParameters, myParameters.m_updateCycleTimeMicros,            "Update cycle time (us)", "The time in microseconds between two update cycles.", "General");
  _AddParameter (myParameters, myParameters.m_spiDevice,                        "Device identifier", "SPI device identifier on the system.", "Radio");
  _AddParameter (myParameters, myParameters.m_spiSpeed,                         "Speed", "Speed of the SPI interface.", "Radio");
  _AddParameter (myParameters, myParameters.m_cePin,                            "CE pin", "GPIO pin to use for the \"Chip Enable\" signal.", "Radio");
  // note: add validation criterium to parameter

  // add sub-module parameters

  // override parameters

}

void Switch::Router::_SetParameters (const Switch::ParameterizedObject::Parameters& i_parameters)
{
  SWITCH_DEBUG_MSG_0 ("Switch::Router::SetParameters ... ");

  // push down to super
  // => no super to push to

  // cast to own parameters object
  const Parameters* pInParameters = dynamic_cast <const Parameters*> (&i_parameters);
  if (0x0 == pInParameters)
  {
    return;
  }

  // validate parameters
//   if (0 == pInParameters->m_maxNrUnidentifiedDevices)
//   {
//     throw std::runtime_error ("max. nr. unknown devices must be striclty positive");
//   }

  // store parameters
  m_deviceAddress                     = pInParameters->m_deviceAddress;
  m_minNodeHearingCountBeforeRouting  = pInParameters->m_minNodeHearingCountBeforeRouting;
  m_maxNrNodesRoutedSimultaneously    = pInParameters->m_maxNrNodesRoutedSimultaneously;
  m_maxNrTxMessagesHandledInOneCycle  = pInParameters->m_maxNrTxMessagesHandledInOneCycle;
  m_updateCycleTimeMicros             = pInParameters->m_updateCycleTimeMicros;
  m_spiDevice                         = pInParameters->m_spiDevice;
  m_spiSpeed                          = pInParameters->m_spiSpeed;
  m_cePin                             = pInParameters->m_cePin;

  SWITCH_DEBUG_MSG_0 ("success\n\r");
}

void Switch::Router::_GetParameters (Switch::ParameterizedObject::Parameters& o_parameters) const
{
  // push down to super
  // => no super to push to

  // cast to own parameters object
  Parameters* pOutParameters = dynamic_cast <Parameters*> (&o_parameters);
  if (0x0 == pOutParameters)
  {
    return;
  }

  // read parameters
  pOutParameters->m_deviceAddress                     = m_deviceAddress;
  pOutParameters->m_minNodeHearingCountBeforeRouting  = m_minNodeHearingCountBeforeRouting;
  pOutParameters->m_maxNrNodesRoutedSimultaneously    = m_maxNrNodesRoutedSimultaneously;
  pOutParameters->m_maxNrTxMessagesHandledInOneCycle  = m_maxNrTxMessagesHandledInOneCycle;
  pOutParameters->m_updateCycleTimeMicros             = m_updateCycleTimeMicros;
  pOutParameters->m_spiDevice                         = m_spiDevice;
  pOutParameters->m_spiSpeed                          = m_spiSpeed;
  pOutParameters->m_cePin                             = m_cePin;
}

/*!
  \brief Constructor
 */
Switch::Router::CommunicationInfo::CommunicationInfo ()
: txAddress                 (0x0),
  lastCommunicationAttempt  (0),
  lastCommunication         (0),
  nrUnsuccessfulTxAttempts  (0)
{
}

/*!
  \brief Destructor
 */
Switch::Router::CommunicationInfo::~CommunicationInfo ()
{
}

/*!
  \brief Copy constructor

  \param[in] i_other Object to copy
 */
Switch::Router::CommunicationInfo::CommunicationInfo (const Switch::Router::CommunicationInfo& i_other)
: txAddress                 (i_other.txAddress),
  lastCommunicationAttempt  (i_other.lastCommunicationAttempt),
  lastCommunication         (i_other.lastCommunication),
  nrUnsuccessfulTxAttempts  (i_other.nrUnsuccessfulTxAttempts)
{
}

/*!
  \brief Assignment operator

  \param[in] i_other Object to assign to this object
 */
Switch::Router::CommunicationInfo& Switch::Router::CommunicationInfo::operator= (const Switch::Router::CommunicationInfo& i_other)
{
  if (this != &i_other)
  {
    txAddress                 = i_other.txAddress;
    lastCommunicationAttempt  = i_other.lastCommunicationAttempt;
    lastCommunication         = i_other.lastCommunication;
    nrUnsuccessfulTxAttempts  = i_other.nrUnsuccessfulTxAttempts;
  }

  return *this;
}

void Switch::Router::CommunicationInfo::SetTxAddress (const switch_pipe_address_type& i_txAddress)
{
  txAddress                 = i_txAddress;
  lastCommunicationAttempt  = 0;
  lastCommunication         = 0;
  nrUnsuccessfulTxAttempts  = 0;
}

/*!
  \brief Constructor
 */
Switch::Router::EventHandler::EventHandler (const NodeDiscoveredCallback& i_newNodeDiscoveredCallback,
                                            const NodeConnectionUpdateCallback& i_nodeConnectionUpdateCallback,
                                            const NodeDataReceivedCallback& i_nodeDataReceivedCallback,
                                            const NodeDataTransmittedCallback& i_nodeDataTransmittedCallback)
: m_newNodeDiscoveredCallback     (i_newNodeDiscoveredCallback),
  m_nodeConnectionUpdateCallback  (i_nodeConnectionUpdateCallback),
  m_nodeDataReceivedCallback      (i_nodeDataReceivedCallback),
  m_nodeDataTransmittedCallback   (i_nodeDataTransmittedCallback)
{
}

/*!
  \brief Destructor
 */
Switch::Router::EventHandler::~EventHandler ()
{
}

/*!
  \brief Signals that a new node was discovered in the network with specified device address.

  \param [in] i_deviceAddress The device address of the new node.
  \param [in] i_deviceInfo    Information about the device. E.g., brand, product and version.
 */
void Switch::Router::EventHandler::NewNodeDiscovered (const switch_device_address_type& i_deviceAddress, const Switch::DeviceInfo& i_deviceInfo)
{
  if (!m_newNodeDiscoveredCallback)
  {
    SWITCH_DEBUG_MSG_0 ("new node discovered callback not set\n");
    return;
  }

  m_newNodeDiscoveredCallback (i_deviceAddress, i_deviceInfo);
}

/*!
  \brief Signals that a node's connection status has changed.

  \param [in] i_deviceAddress The device address of the node.
  \param [in] i_connected     The new connection status. True if connected, false otherwise.
 */
void Switch::Router::EventHandler::NodeConnectionUpdate (const switch_device_address_type& i_deviceAddress, const bool& i_connected)
{
  if (!m_nodeConnectionUpdateCallback)
  {
    SWITCH_DEBUG_MSG_0 ("node connection update callback not set\n");
    return;
  }

  m_nodeConnectionUpdateCallback (i_deviceAddress, i_connected);
}

/*!
  \brief Signals that data was received from a node.

  \param [in] i_deviceAddress The device address of the node.
  \param [in] i_dataPayload   Const reference to the received data payload.

  \return True if the data may be released, false otherwise.
 */
bool Switch::Router::EventHandler::NodeDataReceived (const switch_device_address_type& i_deviceAddress, const Switch::DataPayload& i_dataPayload)
{
  if (!m_nodeDataReceivedCallback)
  {
    SWITCH_DEBUG_MSG_0 ("node data received callback not set\n");
    return false;
  }

  return m_nodeDataReceivedCallback (i_deviceAddress, i_dataPayload);
}

/*!
  \brief Signals that data was transmitted to a node

  \param [in] i_deviceAddress The device address of the node.
  \param [in] i_result Flags if the transmission was successfull (true) or not (false).
 */
void Switch::Router::EventHandler::NodeDataTransmitted (const switch_device_address_type& i_deviceAddress, const bool& i_result)
{
  if (!m_nodeDataTransmittedCallback)
  {
    SWITCH_DEBUG_MSG_0 ("node data transmitted callback not set\n");
    return;
  }

  return m_nodeDataTransmittedCallback (i_deviceAddress, i_result);
}

/*!
  \brief Default constructor.
 */
Switch::Router::Router ()
: m_pRadio (0x0),
  m_pNetworkModel (0x0)
{
  m_routerState.store (OS_STOPPED);

  _SetupParameterContainer ();

  // set the default parameters
  Parameters parameters;
  _SetParameters (parameters);
}

/*!
  \brief Constructor.

  \param[in] i_parameters Parameters to initialize the object with.
 */
Switch::Router::Router (const Parameters& i_parameters)
: m_pRadio (0x0),
  m_pNetworkModel (0x0)
{
  m_routerState.store (OS_STOPPED);

  _SetupParameterContainer ();

  // set the provided parameters
  _SetParameters (i_parameters);
}

/*!
  \brief Destructor
 */
Switch::Router::~Router ()
{
  // make sure the object is stopped and all resources are deallocated
  Stop ();

  // deallocate

}

void Switch::Router::SetEventHandler (const EventHandler& i_eventHandler)
{
  // obtain lock on router mutex
  std::unique_lock <std::mutex> lock (m_routerMutex);

  m_eventHandler = i_eventHandler;
}

void Switch::Router::_Prepare ()
{
  // obtain lock on router mutex
  std::unique_lock <std::mutex> lock (m_routerMutex);

  try
  {
    // create a new network model
    SWITCH_ASSERT_THROW (0x0 == m_pNetworkModel, std::runtime_error ("router network model already exists"));
    Switch::RouterNodeModel routerNode (m_deviceAddress, _RxAddress (0), true);
    m_pNetworkModel = new Switch::RouterNetworkModel (routerNode);

    // initialize the radio
    SWITCH_ASSERT_THROW (0x0 == m_pRadio, std::runtime_error ("radio already exists"));
    m_pRadio = new RF24 (m_spiDevice.c_str (), m_spiSpeed, m_cePin);
    m_pRadio->begin ();

    // configure the radio
    m_pRadio->setChannel      (NC_NETWORK_CHANNEL);
    m_pRadio->setRetries      (NC_RETRY_DELAY_MS, NC_NR_RETRIES);
    m_pRadio->setPayloadSize  (NC_PAYLOAD_SIZE);
    m_pRadio->setPALevel      (NC_POWER_AMPLIER_LEVEL);
    m_pRadio->setDataRate     (NC_DATA_RATE);
    m_pRadio->setCRCLength    (NC_CRC_LENGTH);
    m_pRadio->enableDynamicAcknowledgement (true);

    // open the reading pipes
    m_pRadio->openReadingPipe (0, _RxAddress (0));
    m_pRadio->openReadingPipe (1, _RxAddress (1));
    m_pRadio->openReadingPipe (2, _RxAddress (2));
    m_pRadio->openReadingPipe (3, _RxAddress (3));
    m_pRadio->openReadingPipe (4, _RxAddress (4));
    m_pRadio->openReadingPipe (5, _RxAddress (5));
    // note: pipe 1 is actually not used, but is opened to allow using the other 4 pipes and to
    //       keep conformity between router and nodes

    SWITCH_DEBUG (m_pRadio->printDetails ());

    // start the thread
    SWITCH_ASSERT_THROW (!m_routerThread.joinable (), std::runtime_error ("router thread already running"));
    m_routerThread = std::thread (std::bind (&Switch::Router::_Run, this));

    // switch the router state
    m_routerState.store (OS_READY);
  }
  catch (...)
  {
    // cleanup
    delete m_pRadio;
    m_pRadio = 0x0;

    delete m_pNetworkModel;
    m_pNetworkModel = 0x0;

    // forward
    throw;
  }
}

void Switch::Router::_Start ()
{
  // obtain lock on router mutex
  std::unique_lock <std::mutex> lock (m_routerMutex);

  // start listening
  SWITCH_ASSERT_THROW (0x0 != m_pRadio, std::runtime_error ("router radio does not exist"));
  m_pRadio->startListening ();

  // switch the router state
  SWITCH_ASSERT_THROW (m_routerThread.joinable (), std::runtime_error ("router thread not running"));
  m_routerState.store (OS_STARTED);
  m_updateCondition.notify_all ();
}

void Switch::Router::_Pause ()
{
  SWITCH_ASSERT (m_routerThread.joinable ());

  // switch the router's state to ready
  m_routerState.store (OS_READY);

  // obtain lock on router mutex
  std::unique_lock <std::mutex> lock (m_routerMutex);

  // stop listening to the network
  m_pRadio->stopListening ();
}

void Switch::Router::_Stop ()
{
  SWITCH_ASSERT (m_routerThread.joinable ());

  // obtain lock on router mutex
  std::unique_lock <std::mutex> lock (m_routerMutex);

  // switch the router's state to stopped
  m_routerState.store (OS_STOPPED);

  // wakeup the thread
  lock.unlock ();
  m_updateCondition.notify_all ();

  // join the router thread
  m_routerThread.join ();
  lock.lock ();

  // reset all variables
  for (uint8_t i=0; i<ROUTER_MAX_NR_CHILD_NODES; ++i)
  {
    m_txCommunicationPipes [i].SetTxAddress (0x0);
  }
  FlushRxMessageQueue ();

  // deallocate
  delete m_pRadio;
  m_pRadio = 0x0;

  delete m_pNetworkModel;
  m_pNetworkModel = 0x0;
};

/*!
  \brief Updates the router.

  Triggers the router to process the rx queue and maintain the network connection.
 */
void Switch::Router::Update ()
{
  // notify the waiting thread
  m_updateCondition.notify_all ();
}

/*!
  \brief Router thread run method.

  Contains the logic executed by the router thread.
 */
void Switch::Router::_Run ()
{
  std::unique_lock <std::mutex> lock (m_routerMutex);

  SWITCH_DEBUG_MSG_0 ("router thread started\n");

  eObjectState currentState = m_routerState.load ();
  while (OS_STOPPED != currentState)
  {
    SWITCH_DEBUG_PING (5000000/m_updateCycleTimeMicros, "router thread running\n");
    if (OS_STARTED == currentState)
    {
      // get the time
      std::chrono::high_resolution_clock::time_point beginTime = std::chrono::high_resolution_clock::now ();

      // add new nodes to the network model
      _HandleEnableNodeRoutingData ();

      // listen for incoming messages
      _ListenAndDispatch ();

      // check ping list
      _CheckConnections ();

      // do routing tasks
      _RouteUnassignedNodes ();

      // transmit data in the network
      _HandleTransmitData ();

      // compute the time spent
      std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now ();
      uint32_t microsecondsElapsed = std::chrono::duration_cast <std::chrono::microseconds> (endTime - beginTime).count ();
      if (microsecondsElapsed < m_updateCycleTimeMicros)
      {
        // wait until notification or time elapsed
        m_updateCondition.wait_for (lock, std::chrono::microseconds (m_updateCycleTimeMicros - microsecondsElapsed));
      }
      else
      {
        //SWITCH_DEBUG_MSG_2 ("router thread has delay of %uus on cycle time of %uus\n", (microsecondsElapsed-m_updateCycleTimeMicros), m_updateCycleTimeMicros);
      }
    }
    else
    {
      // wait until wakeup
      m_updateCondition.wait (lock);
    }

    currentState = m_routerState.load ();
  }

  SWITCH_DEBUG_MSG_0 ("router thread stopped\n");
}

/*!
  \brief Checks if the node is connected to the root of the network

  \param[in] i_deviceAddress Address of the node to check the connection with

  \return True if the node is connected to the root of the network, false otherwise
 */
bool Switch::Router::IsConnected (const switch_device_address_type& i_deviceAddress) const
{
  std::unique_lock <std::mutex> modelLock (m_networkModelMutex);

  // get a pointer to the node
  const Switch::RouterNodeModel* pNodeModel = m_pNetworkModel->GetNode (i_deviceAddress);
  if (0x0 != pNodeModel)
  {
    // check if the node has a network address
    if (pNodeModel->networkAddress != 0x0)
    {
      return true;
    }
  }

  return false;
}

/*!
  \brief Gets the current size of the rx message queue

  \return Const reference to the queue count
 */
const uint8_t& Switch::Router::GetNrRxMessagesQueued () const
{
  std::unique_lock <std::mutex> lock (m_rxMessageQueueMutex);

  return m_rxMessageQueueCount;
}

/*!
  \brief Gets the next available rx message payload

  \return Const reference to the payload of the oldest available rx message in the queue
 */
const Switch::DataPayload& Switch::Router::GetRxMessagePayload (switch_device_address_type& o_deviceAddress) const
{
  std::unique_lock <std::mutex> queueLock (m_rxMessageQueueMutex);

  SWITCH_ASSERT (0 != m_rxMessageQueueCount);

  {
    std::unique_lock <std::mutex> modelLock (m_networkModelMutex);

    const Switch::RouterNodeModel* pNode = m_pNetworkModel->GetNode (m_rxMessageQueue [m_rxMessageQueueBegin].header.fromNetworkAddress);
    o_deviceAddress = pNode->deviceAddress;
  }

  return *reinterpret_cast_ptr <const Switch::DataPayload*> (m_rxMessageQueue [m_rxMessageQueueBegin].payload);
}

/*!
  \brief Releases the oldest message in the rx queue
 */
void Switch::Router::ReleaseRxMessage ()
{
  std::unique_lock <std::mutex> queueLock (m_rxMessageQueueMutex);

  _ReleaseRxMessage ();
}

/*!
  \brief Releases the oldest message in the rx queue.

  \note Not thread-safe. Lock the rx message queue mutex externally.
 */
void Switch::Router::_ReleaseRxMessage ()
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
void Switch::Router::FlushRxMessageQueue ()
{
  std::unique_lock <std::mutex> queueLock (m_rxMessageQueueMutex);

  m_rxMessageQueueBegin = 0;
  m_rxMessageQueueEnd   = 0;
  m_rxMessageQueueCount = 0;
}

/*!
  \brief Transmits data to the root of the network

  \param[in] i_dataPayload The data payload to send to the root of the network
 */
void Switch::Router::TransmitData (const switch_device_address_type& i_nodeDeviceAddress, const Switch::DataPayload& i_dataPayload)
{
  SWITCH_DEBUG_MSG_1 ("adding tx message to queue for node 0x%x\n", i_nodeDeviceAddress);

  std::unique_lock <std::mutex> dataLock (m_dataTransmitDataMutex);

  m_dataTransmitData.push_back (std::make_pair (i_nodeDeviceAddress, i_dataPayload));

  m_updateCondition.notify_all ();
}

/*!
  \brief Handles all data in the TransmitData list.
 */
void Switch::Router::_HandleTransmitData ()
{
  std::list <std::pair <switch_device_address_type, Switch::DataPayload>> txMessageQueue;

  {
    // lock the operations lock
    std::unique_lock <std::mutex> dataLock (m_dataTransmitDataMutex);

    // get the next device address
    if (m_dataTransmitData.empty ())
    {
      return;
    }

    // move all data to the temporary container
    if (m_dataTransmitData.size () <= m_maxNrTxMessagesHandledInOneCycle)
    {
      txMessageQueue.splice (txMessageQueue.begin (), m_dataTransmitData);
    }
    else
    {
      std::list <std::pair <switch_device_address_type, Switch::DataPayload>>::iterator itLast = m_dataTransmitData.begin ();
      std::advance (itLast, m_maxNrTxMessagesHandledInOneCycle);
      txMessageQueue.splice (txMessageQueue.begin (), m_dataTransmitData, m_dataTransmitData.begin (), itLast);
    }
  }

  // handle all data
  uint8_t childIndex = 0;
  Switch::NetworkAddress toNetworkAddress;

  m_bufferMessage.header.fromNetworkAddress = 0x0;
  m_bufferMessage.header.messageType   	    = MT_DATA;

  std::list <std::pair <switch_device_address_type, Switch::DataPayload>>::iterator itTxData;
  for (itTxData = txMessageQueue.begin (); txMessageQueue.end () != itTxData; ++itTxData)
  {
    SWITCH_DEBUG_MSG_0 ("transmit data ... ");

    const switch_device_address_type& nodeDeviceAddress = itTxData->first;
    const Switch::DataPayload&        dataPayload       = itTxData->second;

    {
      // lock the network model
      // NOTE: a read-lock would be sufficient
      std::unique_lock <std::mutex> modelLock (m_networkModelMutex);

      // get information about the node
      const Switch::RouterNodeModel* pNodeModel = m_pNetworkModel->GetNode (nodeDeviceAddress);
      if (0x0 == pNodeModel)
      {
        SWITCH_DEBUG_MSG_0 ("unknown node at ");
        SWITCH_DEBUG_BYTES (nodeDeviceAddress, 4);
        SWITCH_DEBUG_MSG_0 ("\n\r");
        m_eventHandler.NodeDataTransmitted (nodeDeviceAddress, false);
        continue;
      }
      if (pNodeModel->networkAddress == 0x0)
      {
        SWITCH_DEBUG_MSG_0 ("node not connected to router\n\r");
        m_eventHandler.NodeDataTransmitted (nodeDeviceAddress, false);
        continue;
      }

      // set the network address and get the child number to route this message to
      toNetworkAddress = pNodeModel->networkAddress;
      childIndex = pNodeModel->networkAddress.GetChildIndex (0);
    }

    // create the message to transmit
    SWITCH_DEBUG_MSG_0 ("creating new data message ... ");
    m_bufferMessage.header.toNetworkAddress = toNetworkAddress;

    // copy the payload
    SWITCH_ASSERT (sizeof (Switch::DataPayload) <= NM_MAX_PAYLOAD_SIZE);
    memcpy (m_bufferMessage.payload, &dataPayload, NM_MAX_PAYLOAD_SIZE);

    // send the message and return the result
    bool result = _SendMessageTo (childIndex, m_bufferMessage);
    m_eventHandler.NodeDataTransmitted (nodeDeviceAddress, result);
  }
}

/*!
 \brief Enables routing of the node with specified device address

 \param[in] i_deviceAddress Address of the node for which to enable routing
 */
void Switch::Router::EnableNodeRouting (const switch_device_address_type& i_deviceAddress)
{
  SWITCH_DEBUG_MSG_1 ("enabling routing for node 0x%x\n", i_deviceAddress);

  std::unique_lock <std::mutex> dataLock (m_dataEnableNodeRoutingMutex);

  m_dataEnableNodeRouting.push_back (i_deviceAddress);

  m_updateCondition.notify_all ();
}

/*!
  \brief Handles all data in the EnableNodeRouting list.
 */
void Switch::Router::_HandleEnableNodeRoutingData ()
{
  std::list <switch_device_address_type> dataEnableNodeRouting;

  {
    // try locking the operations lock
    std::unique_lock <std::mutex> dataLock (m_dataEnableNodeRoutingMutex, std::defer_lock);
    if (!dataLock.try_lock ())
    {
      // lock failed, handle the data next time
      return;
    }

    // get the next device address
    if (m_dataEnableNodeRouting.empty ())
    {
      return;
    }

    // move all data to the temporary container
    dataEnableNodeRouting.splice (dataEnableNodeRouting.begin (), m_dataEnableNodeRouting);
  }

  // handle all data
  std::unique_lock <std::mutex> modelLock (m_networkModelMutex);

  std::list <switch_device_address_type>::const_iterator itDeviceAddress;
  for (itDeviceAddress = dataEnableNodeRouting.begin (); dataEnableNodeRouting.end () != itDeviceAddress; ++itDeviceAddress)
  {
    const switch_device_address_type& deviceAddress = *itDeviceAddress;
    SWITCH_ASSERT (0x0 != deviceAddress);

    // add the node to the network model
    // note: since it will not yet be routed, broadcasts will be received later on.
    //       only then, update the node by setting the rx address => emit signal.
    m_pNetworkModel->AddNode (deviceAddress);
  }
}

/*!
  \brief Gets a pointer to the next free message in the rx message queue

  \returns A pointer to the next free message in the rx message queue or 0x0 if the queue is full

  \note Not thread-safe. Lock the rx message queue mutex externally until the pointer is no longer used.
 */
Switch::NetworkMessage* Switch::Router::_GetFreeRxMessagePointer ()
{
  if (NODE_RX_MESSAGE_QUEUE_SIZE == m_rxMessageQueueCount)
  {
    SWITCH_DEBUG_MSG_0 ("Rx message queue full\n\r");
    return 0x0;
  }

  Switch::NetworkMessage* pFreeRxMessage = &m_rxMessageQueue [m_rxMessageQueueEnd];
  m_rxMessageQueueEnd = (m_rxMessageQueueEnd + 1) % NODE_RX_MESSAGE_QUEUE_SIZE;
  ++m_rxMessageQueueCount;

  return pFreeRxMessage;
}

/*!
  \brief Checks if the connection with the parent node is still alive
  Resets the node if too many communication attempts have failed
 */
void Switch::Router::_CheckConnections ()
{
  for (uint8_t i=0; i<ROUTER_MAX_NR_CHILD_NODES; ++i)
  {
    if (ROUTER_MAX_NR_COMMUNICATION_FAULTS <= m_txCommunicationPipes [i].nrUnsuccessfulTxAttempts)
    {
      SWITCH_DEBUG_MSG_1 ("too manny communication faults to child %u, unassigning node ... ", i);
      // unassign child on index i
      Switch::NetworkAddress childNetworkAddress;
      childNetworkAddress.SetBranchIndex (1);
      childNetworkAddress.SetChildIndex (0, i);

      // lock the network model
      // NOTE: here we'd need an upgradable shared lock
      std::unique_lock <std::mutex> modelLock (m_networkModelMutex);

      const RouterNodeModel* pChildNode = m_pNetworkModel->GetNode (childNetworkAddress);
      SWITCH_ASSERT (0x0 != pChildNode);
      if (0x0 != pChildNode)
      {
        // NOTE: here we'd upgrade the shared lock to a unique lock

        // unassign only the node as it has (re-)started
        std::list <switch_device_address_type> unassignedNodes;
        m_pNetworkModel->UnAssignNode (unassignedNodes, pChildNode->deviceAddress);

        // NOTE: here we'd downgrade the unique lock to a shared lock

        std::list <switch_device_address_type>::iterator nodeIt;
        for (nodeIt=unassignedNodes.begin (); unassignedNodes.end ()!=nodeIt; ++nodeIt)
        {
          m_eventHandler.NodeConnectionUpdate (*nodeIt, false);
        }
      }

      m_txCommunicationPipes [i].SetTxAddress (0x0);
      SWITCH_DEBUG_MSG_0 ("done\n\r");
    }
  }
}

/*!
  \brief Reads all messages from the radio's rx buffer and dispatches the messages to handler functions
 */
void Switch::Router::_ListenAndDispatch ()
{
  // note about threading variables:
  //   only lock m_networkModelMutex for writing as this is the only thread allowed to write and
  //   concurrent read access with other threads is not harmful.

  // helper vairables
  uint8_t pipeNr;

  // loop until no more messages are available on the rx fifo
  // and at most read NODE_MAX_NR_CONSECUTIVE_RX_READS messages
  for (uint8_t i=0;  m_pRadio->available (&pipeNr) && (i<NODE_MAX_NR_CONSECUTIVE_RX_READS); ++i)
  {
    SWITCH_DEBUG_MSG_2 ("incoming message %u on pipe %u ... ", i, pipeNr);
    SWITCH_ASSERT_RETURN_0 ((pipeNr >=0) && (pipeNr < 6));

    // read a buffer message
    m_pRadio->read (&m_bufferMessage, sizeof (m_bufferMessage));
    SWITCH_DEBUG_MSG_2 ("from 0x%04x to 0x%04x ... ", m_bufferMessage.header.toNetworkAddress.value, m_bufferMessage.header.fromNetworkAddress.value);

    // update the communication stats
    if (pipeNr > 1)
    {
      m_txCommunicationPipes [pipeNr - 2].lastCommunication = Switch::NowInMilliseconds ();
    }

    // parse the message
    if (MT_BROADCAST == m_bufferMessage.header.messageType)
    // broadcast messages
    {
      // this message may come from anywhere but rx pipe nr 1
      SWITCH_ASSERT_RETURN_0 (1 != pipeNr);
      SWITCH_DEBUG_MSG_0 ("broadcast message ... ");

      // get the payload
      Switch::BroadcastPayload& payload = *reinterpret_cast_ptr <Switch::BroadcastPayload*> (m_bufferMessage.payload);

      // determine the device address of the first receiver
      switch_device_address_type firstReceiver;
      if (m_bufferMessage.header.fromNetworkAddress == 0x0)
      {
        SWITCH_ASSERT (0 == pipeNr);
        SWITCH_DEBUG_MSG_0 ("caught by router ... ");

        // message received directly from broadcasting node
        firstReceiver = m_deviceAddress;
      }
      else
      {
        SWITCH_ASSERT (0 != pipeNr);

        // message received through child nodes
        const Switch::RouterNodeModel* pChildNode = m_pNetworkModel->GetNode (m_bufferMessage.header.fromNetworkAddress);
        SWITCH_ASSERT_RETURN_0 (0x0 != pChildNode);
        firstReceiver = pChildNode->deviceAddress;

        SWITCH_DEBUG_MSG_1 ("caught by node 0x%x ... ", firstReceiver);
      }

      // check if the node is already known
      const Switch::RouterNodeModel* pNodeModel = m_pNetworkModel->GetNode (payload.broadcastNodeDeviceAddress);
      if (0x0 == pNodeModel)
      {
        SWITCH_DEBUG_MSG_1 ("0x%08x yet unknown ... ", payload.broadcastNodeDeviceAddress);

        {
          // update the network model
          std::unique_lock <std::mutex> modelLock (m_networkModelMutex);

          SWITCH_ASSERT_RETURN_0 (0x0 != payload.broadcastNodeDeviceAddress);
          SWITCH_ASSERT_RETURN_0 (0x0 != payload.broadcastNodeRxPipeAddress);
          m_pNetworkModel->UpdateNode (payload.broadcastNodeDeviceAddress, payload.broadcastNodeRxPipeAddress);
        }

        pNodeModel = m_pNetworkModel->GetNode (payload.broadcastNodeDeviceAddress);
        if (0x0 != pNodeModel)
        {
          // the node is routable and has an rx address => emit the new node discovered event
          m_eventHandler.NewNodeDiscovered (payload.broadcastNodeDeviceAddress, payload.deviceInfo);
        }
      }
      else
      {
        SWITCH_DEBUG_MSG_1 ("0x%08x already known ... ", payload.broadcastNodeDeviceAddress);
        // check if the node model is already unassigned
        if (pNodeModel->GetIsAssigned ())
        {
          SWITCH_DEBUG_MSG_0 ("currently assigned ... ");
          // let the network model unassign the node
          if (BROADCAST_REASON_BEGIN == payload.broadcastReason)
          {
            // unassign only the node as it has (re-)started
            std::list <switch_device_address_type> unassignedNodes;
            {
              std::unique_lock <std::mutex> modelLock (m_networkModelMutex);

              m_pNetworkModel->UnAssignNode (unassignedNodes, pNodeModel->deviceAddress);
            }

            std::list <switch_device_address_type>::iterator nodeIt;
            for (nodeIt=unassignedNodes.begin (); unassignedNodes.end ()!=nodeIt; ++nodeIt)
            {
              m_eventHandler.NodeConnectionUpdate (*nodeIt, false);
            }
          }
          else
          {
            // unassign the whole branch as one or more of the parents of the
            // node didn't respond to the node
            std::list <switch_device_address_type> unassignedNodes;
            {
              std::unique_lock <std::mutex> modelLock (m_networkModelMutex);

              m_pNetworkModel->UnAssignBranch (unassignedNodes, pNodeModel->deviceAddress);
            }

            std::list <switch_device_address_type>::iterator nodeIt;
            for (nodeIt=unassignedNodes.begin (); unassignedNodes.end ()!=nodeIt; ++nodeIt)
            {
              m_eventHandler.NodeConnectionUpdate (*nodeIt, false);
            }
          }
          SWITCH_ASSERT (!pNodeModel->GetIsAssigned ());
        }
      }

      if (0x0 != pNodeModel)
      {
        // add the relationship between the nodes
        std::unique_lock <std::mutex> modelLock (m_networkModelMutex);

        m_pNetworkModel->SetNodeHearsOtherNode (firstReceiver, payload.broadcastNodeDeviceAddress);
      }

      SWITCH_DEBUG_MSG_0 ("handled\n\r");
    }
    else if (1 == pipeNr)
    // messages from parent node
    {
      // channel 1 is currently unused
      SWITCH_DEBUG_MSG_0 ("received a message in pipe 1\n\r");
    }
    else
    // messages from child nodes
    {
      if (MT_PING == m_bufferMessage.header.messageType)
      {
        // translate into pong
        m_bufferMessage.header.messageType = MT_PONG;
        m_bufferMessage.header.toNetworkAddress = m_bufferMessage.header.fromNetworkAddress;
        m_bufferMessage.header.fromNetworkAddress = 0x0;

        SWITCH_DEBUG_MSG_0 ("reflecting ping\n\r");

        // return message
        _SendMessageTo (pipeNr - 2, m_bufferMessage);
      }
      else if (MT_PONG == m_bufferMessage.header.messageType)
      {
        // what was the time difference to when the ping was sent
        Switch::PingPongPayload* pPayload = reinterpret_cast <Switch::PingPongPayload*> (m_bufferMessage.payload);

        uint64_t pingPongTimeMs = Switch::NowInMilliseconds () - pPayload->transmissionStartTimeMs;

        SWITCH_DEBUG_MSG_1 ("ping pong transmission time %llu\n\r", pingPongTimeMs);

        // todo: keep track of the device addresses and ping time when sending a ping,
        //       keep track of the device addresses and the pingpong time after receiving a pong
      }
      else if (MT_DATA == m_bufferMessage.header.messageType)
      // data received
      {
        // put the message in the rx queue
        std::unique_lock <std::mutex> queueLock (m_rxMessageQueueMutex);

        // get a vacant slot in the rx message queue
        Switch::NetworkMessage* pRxMessageQueueSlot = _GetFreeRxMessagePointer ();

        // make sure the slot is empty
        SWITCH_ASSERT (0x0 != pRxMessageQueueSlot);
        if (0x0 != pRxMessageQueueSlot)
        {
          // copy the message into the slot
          memcpy (pRxMessageQueueSlot, &m_bufferMessage, sizeof (Switch::NetworkMessage));
        }

        // get the node's device address
        const Switch::RouterNodeModel* pNode = m_pNetworkModel->GetNode (m_bufferMessage.header.fromNetworkAddress);
        SWITCH_ASSERT_RETURN_0 (0x0 != pNode);

        bool result = m_eventHandler.NodeDataReceived (pNode->deviceAddress, *reinterpret_cast_ptr <const Switch::DataPayload*> (pRxMessageQueueSlot->payload));
        if (result)
        {
          _ReleaseRxMessage ();
        }

        SWITCH_DEBUG_MSG_0 ("data received\n\r");
      }
      else
      {
        SWITCH_DEBUG_MSG_1 ("unknown message type received: 0x%02x\n\r", m_bufferMessage.header.messageType);
      }
    }
  }
}

/*!
  \brief Computes routes to the unassigned nodes
 */
void Switch::Router::_RouteUnassignedNodes ()
{
  // note about threading variables:
  //   only lock m_networkModelMutex for writing as this is the only thread allowed to write and
  //   concurrent read access with other threads is not harmful.

  // get nodes that are heard most frequently
  std::list <const Switch::RouterNodeModel*> unassignedNodes;
  m_pNetworkModel->FillListUnassignedNodes (unassignedNodes, m_minNodeHearingCountBeforeRouting);

  // iterate over the list of unassigned nodes
  uint8_t nrNodesRouted = 0;
  std::list <const Switch::RouterNodeModel*>::iterator nodesIterator;
  for (nodesIterator = unassignedNodes.begin ();
      ((nodesIterator != unassignedNodes.end ()) && (nrNodesRouted <= m_maxNrNodesRoutedSimultaneously));
      ++nodesIterator, ++nrNodesRouted)
  {
    // get a reference to the node pointer
    const Switch::RouterNodeModel*& pNode = *nodesIterator;
    SWITCH_ASSERT_RETURN_0 (0x0 != pNode);
    SWITCH_DEBUG_MSG_1 ("%u unassigned nodes ... ", unassignedNodes.size ());
    SWITCH_DEBUG_MSG_1 ("Routing node %x ... ", pNode->deviceAddress);

    // find the nodes closest to the router with space available for a child
    uint8_t minDistanceToRouter = std::numeric_limits <uint8_t>::max ();
    uint8_t maxNrChildPositionsAvailable = 0;
    Switch::RouterNodeModel* pOptimalNode = 0x0;
    std::list <RouterNodeModel*> candidateParents;
    uint8_t distanceToRouter, nrChildPositionsAvailable;

    SWITCH_DEBUG_MSG_1 ("heared by %u other nodes ... ", pNode->receiverNodes.size ());

    // start filtering based on the distance to the router
    std::list <RouterNodeModel*>::const_iterator hearingIterator;
    for (hearingIterator = pNode->receiverNodes.begin (); hearingIterator != pNode->receiverNodes.end (); ++hearingIterator)
    {
      RouterNodeModel* const& hearingNode = *hearingIterator;

      // check if the node can handle extra childs
      if (0 < hearingNode->GetNrChildPositionsAvailable ())
      {
        // get the distance to the router and check if this is one
        // of the closest nodes currently found
        distanceToRouter = hearingNode->GetDistanceToRouterNode ();
        SWITCH_DEBUG_MSG_1 ("distance to router %u ... ", distanceToRouter);
        if (minDistanceToRouter >= distanceToRouter)
        {
          // check if this is closer than other nodes found so far
          if (minDistanceToRouter > distanceToRouter)
          {
            // clear all current candidates and update the current minimum distance to the router
            candidateParents.clear ();
            minDistanceToRouter = distanceToRouter;
          }

          // add the node to the candidate list
          candidateParents.push_back (hearingNode);

          SWITCH_DEBUG_MSG_0 ("parent candidate ... ");
        }
      }
    }

    // ensure that there is at least one node remaining
    if (0 == candidateParents.size ())
    {
      SWITCH_DEBUG_MSG_0 ("no candidate parents found\n\r");
      return;
    }

    // retain the node with the most child positions available
    for (hearingIterator = candidateParents.begin (); hearingIterator != candidateParents.end (); ++hearingIterator)
    {
      RouterNodeModel* const& hearingNode = *hearingIterator;

      // get the nr of child positions available
      nrChildPositionsAvailable = hearingNode->GetNrChildPositionsAvailable ();
      if (nrChildPositionsAvailable > maxNrChildPositionsAvailable)
      {
        // keep track of the maximum
        maxNrChildPositionsAvailable = nrChildPositionsAvailable;
        // keep track of this node as current optimal node
        pOptimalNode = hearingNode;
      }
    }

    // ensure the optimal node exists
    SWITCH_ASSERT (0x0 != pOptimalNode);
    Switch::NetworkAddress networkAddress;
    SWITCH_DEBUG_MSG_2 ("optimal parent node at distance %u with %u positions ... ", minDistanceToRouter, maxNrChildPositionsAvailable);
    {
      // route pNode through pOptimalNode
      std::unique_lock <std::mutex> modelLock (m_networkModelMutex);
      networkAddress = m_pNetworkModel->AssignNode (pOptimalNode->deviceAddress, pNode->deviceAddress);
    }

    // prepare the network message
    m_bufferMessage.header.messageType  = MT_ADDRESS_ASSIGNMENT;
    m_bufferMessage.header.fromNetworkAddress = 0x0;
    // create the assignment message payload
    Switch::NodeAssignmentPayload* pPayload = reinterpret_cast <Switch::NodeAssignmentPayload*> (m_bufferMessage.payload);
    pPayload->nodeDeviceAddress   = pNode->deviceAddress;
    pPayload->nodeNetworkAddress  = networkAddress;
    // get the child index on the router node to start routing the message
    uint8_t childIndex = pOptimalNode->networkAddress.GetChildIndex (0);

    if (m_deviceAddress == pOptimalNode->deviceAddress)
    // the node is assigned to the router node
    {
      // set the new child tx address
      m_txCommunicationPipes [childIndex].SetTxAddress (pNode->rxPipeAddress);

      // complete the payload
      pPayload->communicationPipeAddress      = _RxAddress (childIndex + 2);

      // fill in message header
      m_bufferMessage.header.toNetworkAddress = pNode->networkAddress;
    }
    else
    {
      // complete the payload
      pPayload->communicationPipeAddress      = pNode->rxPipeAddress;

      // fill in message header
      m_bufferMessage.header.toNetworkAddress = pOptimalNode->networkAddress;
    }

    // send the message to the node
    SWITCH_DEBUG_MSG_3 ("Sending network address 0x%x, tx address 0x%llx to node 0x%x ... ", pPayload->nodeNetworkAddress.value, pPayload->communicationPipeAddress, pPayload->nodeDeviceAddress);

    SWITCH_DEBUG_MSG_0 ("payload bytes: ");
    SWITCH_DEBUG_BYTES (pPayload, sizeof (Switch::NodeAssignmentPayload));
    SWITCH_DEBUG_MSG_0 (" ... ");

    bool result = _SendMessageTo (childIndex, m_bufferMessage);
    if (!result)
    {
      // revert changes
      std::unique_lock <std::mutex> modelLock (m_networkModelMutex);
      std::list <switch_device_address_type> unassignedChildNodes;
      m_pNetworkModel->UnAssignNode (unassignedChildNodes, pNode->deviceAddress);
      if (m_deviceAddress == pOptimalNode->deviceAddress)
      {
        m_txCommunicationPipes [childIndex].SetTxAddress (0x0);
      }
      SWITCH_DEBUG_MSG_0 ("routing failed\n\r");
    }
    SWITCH_DEBUG_IF (result, SWITCH_DEBUG_MSG_0 ("routing success\n\r"));

    m_eventHandler.NodeConnectionUpdate (pNode->deviceAddress, true);
  }
}

/*!
  \brief Sends a message to a known receiver

  \param [in] i_receiverIndex Index of the known receiver. The receiver at index 0 is the parent node
  and must always be known when this node has a network address.
  \param [in] i_txMessage The message to send.
 */
bool Switch::Router::_SendMessageTo (const uint8_t& i_receiverIndex, const Switch::NetworkMessage& i_txMessage)
{
  SWITCH_DEBUG_MSG_1 ("send message to %u ...", i_receiverIndex);
  SWITCH_ASSERT_RETURN_1 (0x0 != m_txCommunicationPipes [i_receiverIndex].txAddress, false);

  // get a reference to the receiver
  CommunicationInfo& receiver = m_txCommunicationPipes [i_receiverIndex];

  // get the current time
  uint64_t timeNow = Switch::NowInMilliseconds ();

  // prepare to write
  m_pRadio->stopListening ();
  m_pRadio->openWritingPipe (receiver.txAddress);

  // write with auto acknowledgement enabled
  bool result = m_pRadio->write (&i_txMessage, sizeof (i_txMessage), true);

  // resume listening
  m_pRadio->startListening ();

  // update the node communication stats
  receiver.lastCommunicationAttempt = timeNow;
  if (result)
  {
    receiver.nrUnsuccessfulTxAttempts = 0;
    receiver.lastCommunication = timeNow;
    SWITCH_DEBUG_MSG_0 (" transmitted\n\r");
  }
  else
  {
    ++receiver.nrUnsuccessfulTxAttempts;
    SWITCH_DEBUG_MSG_1 (" %u attempts in a row failed\n\r", receiver.nrUnsuccessfulTxAttempts);
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
switch_pipe_address_type Switch::Router::_RxAddress (const uint8_t& i_pipeIndex)
{
  static uint8_t pipeRxBytes [] = { 0x5a, 0x69, 0x96, 0xa5, 0xc3 };

  uint64_t resultAddress;
  if (0 == i_pipeIndex)
  {
    resultAddress = NC_BROADCAST_PIPE;
  }
  else
  {
    resultAddress = m_deviceAddress;
    resultAddress  <<= 8;
    resultAddress |= pipeRxBytes [i_pipeIndex - 1];
  }

  return resultAddress;
}
