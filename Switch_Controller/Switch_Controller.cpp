/*?*************************************************************************
*                           Switch_Controller.cpp
*                           -----------------------
*    copyright            : (C) 2014 by Wouter Charle
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

#include "Switch_Controller.h"

// project includes
#include "Switch_InterfaceTranslations.h"

// switch includes
#include <Switch_Device/Switch_DeviceStore.h>
#include <Switch_Router/Switch_Router.h>
#include <Switch_Network/Switch_DataPayload.h>

// third-party includes

// other declarations
using namespace std::placeholders;


/*!
  \brief Default constructor
 */
Switch::Controller::Parameters::Parameters ()
{
  m_updateCycleTimeMicros = 500000;
}

/*!
  \brief Destructor
 */
Switch::Controller::Parameters::~Parameters ()
{
}

void Switch::Controller::_SetupParameterContainer ()
{
  // set container properties
  _SetContainerName ("Switch controller");
  _SetContainerDescription ("Parameters of the Switch application controller.");

  // add parameters
  Parameters myParameters;
  _AddParameter (myParameters, myParameters.m_updateCycleTimeMicros,            "Update cycle time (us)", "The time in microseconds between two update cycles.", "General");
  // note: add validation criterium to parameter

  // add sub-module parameters
  _LinkParameterizedObject (*m_pDeviceStore, "Device Store");
  _LinkParameterizedObject (*m_pRouter, "Router");

  // override parameters

}

void Switch::Controller::_SetParameters (const Switch::ParameterizedObject::Parameters& i_parameters)
{
  SWITCH_DEBUG_MSG_0 ("Switch::Controller::SetParameters ... ");

  // push down to super
  // => no super to push to

  // cast to own parameters object
  const Parameters* pInParameters = dynamic_cast <const Parameters*> (&i_parameters);
  if (0x0 == pInParameters)
  {
    return;
  }

  // validate parameters

  // store parameters
  m_updateCycleTimeMicros = pInParameters->m_updateCycleTimeMicros;

  SWITCH_DEBUG_MSG_0 ("success\n\r");
}

void Switch::Controller::_GetParameters (Switch::ParameterizedObject::Parameters& o_parameters) const
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
  pOutParameters->m_updateCycleTimeMicros = m_updateCycleTimeMicros;
}

/*!
  \brief Default constructor
 */
Switch::Controller::Controller ()
: m_pDeviceStore (0x0),
  m_pRouter (0x0)
{
  _Construct ();

  // set the default parameters
  Parameters parameters;
  _SetParameters (parameters);
}

/*!
  \brief Constructor
 */
Switch::Controller::Controller (const Switch::Controller::Parameters& i_parameters)
: m_pDeviceStore (0x0),
  m_pRouter (0x0)
{
  _Construct ();

  // set the custom parameters
  _SetParameters (i_parameters);
}

void Switch::Controller::_Construct ()
{
  try
  {
    // create modules
    m_pDeviceStore  = new Switch::DeviceStore ();
    m_pRouter       = new Switch::Router ();

    // link modules
    Switch::Router::EventHandler routerEventHandler
    (
      std::bind (&Switch::Controller::_OnRouterNewNodeDiscovered, this, _1, _2),
      std::bind (&Switch::Controller::_OnRouterNodeConnectionUpdate, this, _1, _2),
      std::bind (&Switch::Controller::_OnRouterNodeDataReceived, this, _1, _2),
      std::bind (&Switch::Controller::_OnRouterNodeDataTransmitted, this, _1, _2)
    );
    m_pRouter->SetEventHandler (routerEventHandler);

    // setup parameter container
    _SetupParameterContainer ();
  }
  catch (...)
  {
    // cleanup
    delete m_pRouter;
    m_pRouter = 0x0;

    delete m_pDeviceStore;
    m_pDeviceStore = 0x0;

    // forward
    throw;
  }
}

/*!
  \brief Destructor
 */
Switch::Controller::~Controller () noexcept (true)
{
  // make sure the object is stopped and all resouces are deallocated
  Stop ();
}

void Switch::Controller::_Prepare ()
{
  SWITCH_DEBUG_MSG_0 ("Preparing Switch::Controller ... ");

  // obtain lock on the operations mutex
  std::unique_lock <std::mutex> controllLock (m_controllerMutex);

  try
  {
    // prepare the device store
    m_pDeviceStore->Prepare ();
    // get the map of all devices
    const Switch::DeviceStore::DeviceMap& identifiedDevices = m_pDeviceStore->GetDevices ();
    const Switch::DeviceStore::DeviceMap& unidentifiedDevices = m_pDeviceStore->GetUnidentfiedDevices ();

    // prepare the router
    m_pRouter->Prepare ();
    // enable routing all already known devices
    Switch::DeviceStore::DeviceMap::const_iterator itDevices;
    for (itDevices = identifiedDevices.begin (); identifiedDevices.end () != itDevices; ++itDevices)
    {
      m_pRouter->EnableNodeRouting (itDevices->first);
    }
    for (itDevices = unidentifiedDevices.begin (); unidentifiedDevices.end () != itDevices; ++itDevices)
    {
      m_pRouter->EnableNodeRouting (itDevices->first);
    }

    // start the thread
    SWITCH_ASSERT_THROW (!m_controllerThread.joinable (), std::runtime_error ("controller thread already running"));
    m_controllerThread = std::thread (std::bind (&Switch::Controller::_Run, this));

    // switch the router state
    m_controllerState.store (OS_READY);
  }
  catch (...)
  {
    SWITCH_DEBUG_MSG_0 ("exception thrown\n\r");

    // cleanup

    // forward
    throw;
  }

  SWITCH_DEBUG_MSG_0 ("success!\n\r");
}

void Switch::Controller::_Start ()
{
  SWITCH_DEBUG_MSG_0 ("Starting Switch::Controller ... ");

  // obtain lock on operations mutex
  std::unique_lock <std::mutex> controllLock (m_controllerMutex);

  try
  {
    // start the modules
    m_pDeviceStore->Start ();
    m_pRouter->Start ();

    // switch the controller state
    SWITCH_ASSERT_THROW (m_controllerThread.joinable (), std::runtime_error ("controller thread not running"));
    m_controllerState.store (OS_STARTED);
    m_updateCondition.notify_all ();
  }
  catch (...)
  {
    SWITCH_DEBUG_MSG_0 ("exception thrown\n\r");

    // cleanup

    // forward
    throw;
  }

  SWITCH_DEBUG_MSG_0 ("success!\n\r");
}

void Switch::Controller::_Pause ()
{
  SWITCH_DEBUG_MSG_0 ("Pausing Switch::Controller ... ");

  SWITCH_ASSERT (m_controllerThread.joinable ());

  // switch the router's state to ready
  m_controllerState.store (OS_READY);

  // obtain lock on the operations mutex
  std::unique_lock <std::mutex> controllLock (m_controllerMutex);

  // pause the modules
  m_pRouter->Pause ();
  m_pDeviceStore->Pause ();

  SWITCH_DEBUG_MSG_0 ("success!\n\r");
}

void Switch::Controller::_Stop ()
{
  SWITCH_DEBUG_MSG_0 ("Stopping Switch::Controller ... ");

  SWITCH_ASSERT (m_controllerThread.joinable ());

  std::unique_lock <std::mutex> controllLock (m_controllerMutex);

  // switch the controller's state to stopped
  m_controllerState.store (OS_STOPPED);

  // wakeup the thread
  controllLock.unlock ();
  m_updateCondition.notify_all ();

  // join the controller thread
  m_controllerThread.join ();
  controllLock.lock ();

  // stop the submodules
  m_pRouter->Stop ();
  m_pDeviceStore->Stop ();

  SWITCH_DEBUG_MSG_0 ("success!\n\r");
}

void Switch::Controller::_Run ()
{
  bool sleepAllowed = true;
  std::chrono::high_resolution_clock::time_point beginTime, endTime;
  uint32_t microsecondsElapsed;

  std::unique_lock <std::mutex> controllLock (m_controllerMutex);

  eObjectState currentState = m_controllerState.load ();
  while (OS_STOPPED != currentState)
  {
    SWITCH_DEBUG_PING (5000000/m_updateCycleTimeMicros, "controller thread running\n");
    if (OS_STARTED == currentState)
    {
      // get the time
      beginTime = std::chrono::high_resolution_clock::now ();

      // reset variables
      sleepAllowed = true;

      // do controller tasks
      sleepAllowed &= _HandleDataNodeConnectionUpdate ();
      sleepAllowed &= _HandleDataNodeDataTransmitted ();
      sleepAllowed &= _HandleDataNodeDataReceived ();
      sleepAllowed &= _HandleDataSetDeviceValues ();
      sleepAllowed &= _HandleDataNewNodeDiscovered ();
      sleepAllowed &= _HandleDataAddDevice ();

      // compute the time spent
      endTime = std::chrono::high_resolution_clock::now ();
      microsecondsElapsed = std::chrono::duration_cast <std::chrono::microseconds> (endTime - beginTime).count ();
      sleepAllowed &= (microsecondsElapsed < m_updateCycleTimeMicros);
      // sleep if no more tasks need to be done
      if (sleepAllowed)
      {
        // wait until notification or time elapsed
        m_updateCondition.wait_for (controllLock, std::chrono::microseconds (m_updateCycleTimeMicros - microsecondsElapsed));
      }
      else
      {
        //SWITCH_DEBUG_MSG_2 ("controller thread not sleeping, delay of %ius on cycle time of %uus\n", (microsecondsElapsed-m_updateCycleTimeMicros), m_updateCycleTimeMicros);
      }
    }
    else
    {
      // wait until wakeup
      m_updateCondition.wait (controllLock);
    }

    currentState = m_controllerState.load ();
  }
}

/*!
  \brief Handles the data from the router's NewNodeDiscovered callback.

  \return True if all data has been handled, false otherwise.
 */
bool Switch::Controller::_HandleDataNewNodeDiscovered ()
{
  std::pair <switch_device_address_type, Switch::DeviceInfo> data;

  {
    std::unique_lock <std::mutex> dataLock (m_routerDataMutex);

    // check if new nodes were discovered
    if (m_dataNewNodeDiscovered.empty ())
    {
      return true;
    }

    // get the address of the first discovered node
    data = m_dataNewNodeDiscovered.front ();
    m_dataNewNodeDiscovered.pop_front ();
  }

  // handle the discovered node
  SWITCH_ASSERT_THROW (nullptr != m_pDeviceStore, std::runtime_error ("device store not allocated"));
  Switch::DeviceStore::DeviceMap& deviceMap = m_pDeviceStore->GetDevices ();

  Switch::DeviceStore::DeviceMap::iterator itDevice = deviceMap.find (data.first);
  if (itDevice == deviceMap.end ())
  {
    // node is unidentified
    // note: this check is needed because the first time the node is discovered it will be treated as a new node by the router
    m_pDeviceStore->SetDeviceType (data.first, data.second.brandId, data.second.productId, data.second.productVersion);
  }

  return false;
}

/*!
  \brief Handles the data from the router's NodeConnectionUpdate callback.

  \return True if all data has been handled, false otherwise.
 */
bool Switch::Controller::_HandleDataNodeConnectionUpdate ()
{
  std::pair <switch_device_address_type, bool> data;

  {
    std::unique_lock <std::mutex> dataLock (m_routerDataMutex);

    // check if new nodes were discovered
    if (m_dataNodeConnectionUpdate.empty ())
    {
      return true;
    }

    // get the address of the first discovered node
    data = m_dataNodeConnectionUpdate.front ();
    m_dataNodeConnectionUpdate.pop_front ();
  }

  // handle the connection update

  // . get the device
  SWITCH_ASSERT_THROW (nullptr != m_pDeviceStore, std::runtime_error ("device store not allocated"));
  Switch::DeviceStore::DeviceMap& deviceMap = m_pDeviceStore->GetDevices ();
  Switch::DeviceStore::DeviceMap::iterator itDevice = deviceMap.find (data.first);
  SWITCH_ASSERT_THROW (itDevice != deviceMap.end (), std::runtime_error ("connection update received for unknown device"));

  // . set the data in the device
  itDevice->second.SetConnectionState (data.second);

  // . translate and forward the signal
  if (0 != m_deviceConnectionUpdateSignal.num_slots ())
  {
    Switch::Interface::Device::Connection connection;
    connection.m_online = data.second;

    // forward the signal
    m_deviceConnectionUpdateSignal (static_cast <uint32_t> (data.first), connection);
  }

  return false;
}

/*!
  \brief Handles the data from the router's NodeDataReceived callback.

  \return True if all data has been handled, false otherwise.
 */
bool Switch::Controller::_HandleDataNodeDataReceived ()
{
  std::pair <switch_device_address_type, Switch::DataPayload> data;

  {
    std::unique_lock <std::mutex> dataLock (m_routerDataMutex);

    // check if new nodes were discovered
    if (m_dataNodeDataReceived.empty ())
    {
      return true;
    }

    // get the address of the first discovered node
    data = m_dataNodeDataReceived.front ();
    m_dataNodeDataReceived.pop_front ();
  }

  // handle the received data

  // . get the device
  SWITCH_ASSERT_THROW (nullptr != m_pDeviceStore, std::runtime_error ("device store not allocated"));
  Switch::DeviceStore::DeviceMap& deviceMap = m_pDeviceStore->GetDevices ();
  Switch::DeviceStore::DeviceMap::iterator itDevice = deviceMap.find (data.first);
  SWITCH_ASSERT_THROW (itDevice != deviceMap.end (), std::runtime_error ("data received from unknown device"));

  // . set the data in the device
  Switch::DataContainer& deviceData = itDevice->second.GetDataContainer ();
  std::list <Switch::DataContainer::Element> changedElements;
  bool dataChanged = deviceData.SetContent (changedElements, data.second.data);

  // handle dataChanged
  if (dataChanged && (0 != m_deviceDataUpdateSignal.num_slots ()))
  {
    // translate the changed data elements
    std::list <Switch::Interface::Device::Value> changedValues;
    std::list <Switch::DataContainer::Element>::iterator itElement;
    for (itElement=changedElements.begin (); changedElements.end ()!=itElement; ++itElement)
    {
      Switch::Interface::Device::Value changedValue;
      Switch::Interface::Translate (changedValue, *itElement);
      changedValues.push_back (changedValue);
    }

    // forward the signal
    m_deviceDataUpdateSignal (static_cast <uint32_t> (data.first), changedValues);
  }

  return false;
}

/*!
  \brief Handles the data from the router's NodeDataTransmitted callback.

  \return True if all data has been handled, false otherwise.
 */
bool Switch::Controller::_HandleDataNodeDataTransmitted ()
{
  std::pair <switch_device_address_type, bool> data;

  {
    std::unique_lock <std::mutex> dataLock (m_routerDataMutex);

    // check if new nodes were discovered
    if (m_dataNodeDataTransmitted.empty ())
    {
      return true;
    }

    // get the address of the first discovered node
    data = m_dataNodeDataTransmitted.front ();
    m_dataNodeDataTransmitted.pop_front ();
  }

  // handle the transmission result
  // TODO: schedule to re-transmit?

  return false;
}

bool Switch::Controller::_HandleDataAddDevice ()
{
  std::list <switch_device_address_type> data;

  {
    std::unique_lock <std::mutex> dataLock (m_interfaceDataMutex);

    // check if new nodes were discovered
    if (m_dataAddDevice.empty ())
    {
      return true;
    }

    // get the address of the nodes to add
    data.splice (data.begin (), m_dataAddDevice);
  }

  std::list <switch_device_address_type>::iterator itData;
  for (itData = data.begin (); data.end () != itData; ++itData)
  {
    switch_device_address_type& deviceAddress = *itData;
    m_pDeviceStore->AddDevice (deviceAddress);
    m_pRouter->EnableNodeRouting (deviceAddress);
  }

  return false;
}

bool Switch::Controller::_HandleDataSetDeviceValues ()
{
  std::pair <switch_device_address_type, std::list <Switch::DataContainer::Element>> dataSetDeviceValues;

  {
    std::unique_lock <std::mutex> dataLock (m_interfaceDataMutex);

    // check if new nodes were discovered
    if (m_dataSetDeviceValues.empty ())
    {
      return true;
    }

    // get the device address and the values to set in the device
    dataSetDeviceValues = m_dataSetDeviceValues.front ();
    m_dataSetDeviceValues.pop_front ();
  }

  Switch::DeviceStore::DeviceMap& devices = m_pDeviceStore->GetDevices ();
  Switch::DeviceStore::DeviceMap::iterator itDevice = devices.find (dataSetDeviceValues.first);
  if (devices.end () == itDevice)
  {
    return false;
  }

  // set the data in the device
  Switch::Device& device = itDevice->second;
  Switch::DataContainer& dataContainer = device.GetDataContainer ();
  std::list <Switch::DataContainer::Element> changedElements;
  bool dataChanged = dataContainer.SetElements (changedElements, dataSetDeviceValues.second);
  if (!dataChanged)
  {
    return false;
  }

  // transmit the data to the device
  Switch::DataPayload txPayload;
  dataContainer.GetContent (txPayload.data);
  m_pRouter->TransmitData (dataSetDeviceValues.first, txPayload);

  return false;
}

/*!
  \brief Signals that a new node was discovered in the network with specified device address.

  \param [in] i_deviceAddress The device address of the new node.
 */
void Switch::Controller::_OnRouterNewNodeDiscovered (const switch_device_address_type& i_deviceAddress, const Switch::DeviceInfo& i_deviceInfo)
{
  SWITCH_DEBUG_MSG_0 ("_OnRouterNewNodeDiscovered ... ");
  std::unique_lock <std::mutex> dataLock (m_routerDataMutex);

  m_dataNewNodeDiscovered.push_back (std::make_pair (i_deviceAddress, i_deviceInfo));

  m_updateCondition.notify_all ();

  SWITCH_DEBUG_MSG_0 ("done\n");
}

/*!
  \brief Signals that a node's connection status has changed

  \param [in] i_connected The new connection status. True if connected, false otherwise.
  \param [in] i_deviceAddress The device address of the node.
 */
void Switch::Controller::_OnRouterNodeConnectionUpdate (const switch_device_address_type& i_deviceAddress, const bool& i_connected)
{
  SWITCH_DEBUG_MSG_0 ("_OnRouterNodeConnectionUpdate ... ");
  std::unique_lock <std::mutex> dataLock (m_routerDataMutex);

  m_dataNodeConnectionUpdate.push_back (std::make_pair (i_deviceAddress, i_connected));

  m_updateCondition.notify_all ();

  SWITCH_DEBUG_MSG_0 ("done\n");
}

/*!
  \brief Signals that data was received from a node

  \param [in] i_dataPayload Const reference to the received data payload
  \param [in] i_deviceAddress The device address of the node.

  \return True if the data may be released, false otherwise.
 */
bool Switch::Controller::_OnRouterNodeDataReceived (const switch_device_address_type& i_deviceAddress, const Switch::DataPayload& i_dataPayload)
{
  SWITCH_DEBUG_MSG_0 ("_OnRouterNodeDataReceived ... ");
  std::unique_lock <std::mutex> dataLock (m_routerDataMutex);

  m_dataNodeDataReceived.push_back (std::make_pair (i_deviceAddress, i_dataPayload));

  m_updateCondition.notify_all ();

  SWITCH_DEBUG_MSG_0 ("done\n");

  return true;
}

/*!
  \brief Signals that data was transmitted to a node

  \param [in] i_deviceAddress The device address of the node.
  \param [in] i_result Flags if the transmission was successfull (true) or not (false).
 */
void Switch::Controller::_OnRouterNodeDataTransmitted (const switch_device_address_type& i_deviceAddress, const bool& i_result)
{
  SWITCH_DEBUG_MSG_0 ("_OnRouterNodeDataTransmitted ... ");
  std::unique_lock <std::mutex> dataLock (m_routerDataMutex);

  m_dataNodeDataTransmitted.push_back (std::make_pair (i_deviceAddress, i_result));

  m_updateCondition.notify_all ();

  SWITCH_DEBUG_MSG_0 ("done\n");

  return;
}

//************* Interface methods *************//

boost::signals2::connection Switch::Controller::ConnectToDeviceConnectionUpdateSignal (const DeviceConnectionUpdateSignal::slot_type& i_receiver)
{
  return m_deviceConnectionUpdateSignal.connect (i_receiver);
}

boost::signals2::connection Switch::Controller::ConnectToDeviceDataUpdateSignal (const DeviceDataUpdateSignal::slot_type& i_receiver)
{
  return m_deviceDataUpdateSignal.connect (i_receiver);
}

Switch::Controller::eCallResult Switch::Controller::AddDevice (const uint32_t& i_deviceAddress)
{
  SWITCH_DEBUG_MSG_0 ("AddDevice ... ");

  std::unique_lock <std::mutex> stateLock (m_stateMutex);

  if (OS_STARTED != m_objectState)
  {
    return CR_STOPPED;
  }

  std::unique_lock <std::mutex> interfaceDataLock (m_interfaceDataMutex);

  m_dataAddDevice.push_back (static_cast <switch_device_address_type> (i_deviceAddress));

  m_updateCondition.notify_all ();

  SWITCH_DEBUG_MSG_0 ("done\n");

  return CR_OK;
}

Switch::Controller::eCallResult Switch::Controller::EnumerateDevices (std::list <Switch::Interface::Device::Summary>& o_deviceInfo)
{
  SWITCH_DEBUG_MSG_0 ("EnumerateDevices ... ");
  o_deviceInfo.clear ();

  std::unique_lock <std::mutex> stateLock (m_stateMutex);

  if (OS_STARTED != m_objectState)
  {
    return CR_STOPPED;
  }

  const Switch::DeviceStore::DeviceMap& devices = m_pDeviceStore->GetDevices ();
  Switch::DeviceStore::DeviceMap::const_iterator itDevice;
  for (itDevice = devices.begin (); devices.end () != itDevice; ++itDevice)
  {
    Switch::Interface::Device::Summary deviceSummary;

    deviceSummary.m_deviceId = (*itDevice).first;
    const Switch::Device& device = (*itDevice).second;

    // set device product information
    Switch::Interface::Translate (deviceSummary.m_productInfo, device);

    // set device connection information
    deviceSummary.m_online = device.GetConnectionState ();

    o_deviceInfo.push_back (deviceSummary);
  }

  SWITCH_DEBUG_MSG_0 ("done\n");

  return CR_OK;
}

Switch::Controller::eCallResult Switch::Controller::GetDeviceDetails (Switch::Interface::Device& o_deviceDetails, const uint32_t& i_deviceAddress)
{
  SWITCH_DEBUG_MSG_0 ("GetDeviceDetails ... ");

  std::unique_lock <std::mutex> stateLock (m_stateMutex);

  if (OS_STARTED != m_objectState)
  {
    return CR_STOPPED;
  }

  const Switch::DeviceStore::DeviceMap& devices = m_pDeviceStore->GetDevices ();
  Switch::DeviceStore::DeviceMap::const_iterator itDevice = devices.find (i_deviceAddress);
  if (itDevice == devices.end ())
  {
    return CR_UNKNOWN_DEVICE;
  }

  o_deviceDetails.m_deviceId = (*itDevice).first;
  const Switch::Device& device = (*itDevice).second;

  // set device product information
  Switch::Interface::Translate (o_deviceDetails.m_productInfo, device);

  // set device data format information
  Switch::Interface::Translate (o_deviceDetails.m_dataFormat, device);

  // set device connection information
  o_deviceDetails.m_connectionInfo.m_online = device.GetConnectionState ();

  SWITCH_DEBUG_MSG_0 ("done\n");

  return CR_OK;
}

Switch::Controller::eCallResult Switch::Controller::GetDeviceValues (std::list <Switch::Interface::Device::Value>& o_values, const uint32_t& i_deviceAddress)
{
  SWITCH_DEBUG_MSG_0 ("GetDeviceValues ... ");

  o_values.clear ();

  std::unique_lock <std::mutex> stateLock (m_stateMutex);

  if (OS_STARTED != m_objectState)
  {
    return CR_STOPPED;
  }

  // find the device
  const Switch::DeviceStore::DeviceMap& devices = m_pDeviceStore->GetDevices ();
  Switch::DeviceStore::DeviceMap::const_iterator itDevice = devices.find (i_deviceAddress);
  if (itDevice == devices.end ())
  {
    return CR_UNKNOWN_DEVICE;
  }

  // translate the device's data values
  const Switch::Device& device = itDevice->second;
  const Switch::DataContainer& dataContainer = device.GetDataContainer ();
  std::list <Switch::DataContainer::Element> containerElements;
  dataContainer.GetElements (containerElements);
  std::list <Switch::DataContainer::Element>::iterator itElement;
  for (itElement = containerElements.begin (); containerElements.end () != itElement; ++itElement)
  {
    Switch::Interface::Device::Value value;
    Switch::Interface::Translate (value, *itElement);
    o_values.push_back (value);
  }

  SWITCH_DEBUG_MSG_0 ("done\n");

  return CR_OK;
}

Switch::Controller::eCallResult Switch::Controller::SetDeviceValues (const uint32_t& i_deviceAddress, const std::list <Switch::Interface::Device::Value>& i_values)
{
  SWITCH_DEBUG_MSG_0 ("SetDeviceValues ... ");

  std::unique_lock <std::mutex> stateLock (m_stateMutex);

  if (OS_STARTED != m_objectState)
  {
    return CR_STOPPED;
  }

  std::unique_lock <std::mutex> interfaceDataLock (m_interfaceDataMutex);

  m_dataSetDeviceValues.push_back (std::make_pair (i_deviceAddress, std::list <Switch::DataContainer::Element> ()));

  std::list <Switch::DataContainer::Element>& containerElements = m_dataSetDeviceValues.back ().second;
  std::list <Switch::Interface::Device::Value>::const_iterator itValue;
  for (itValue = i_values.begin (); i_values.end () != itValue; ++itValue)
  {
    Switch::DataContainer::Element element;
    Switch::Interface::Translate (element, *itValue);
    containerElements.push_back (element);
  }

  SWITCH_DEBUG_MSG_0 ("done\n");

  return CR_OK;
}

