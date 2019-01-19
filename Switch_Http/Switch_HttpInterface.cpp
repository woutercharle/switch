/*?*************************************************************************
*                           Switch_HttpInterface.cpp
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
*    email                : wouter.charle@gmail.com
*
***************************************************************************/

#include "Switch_HttpInterface.h"

// Switch includes
#include <Switch_API/Switch_FunctionalFacade.h>
#include <Switch_Base/Switch_Debug.h>

// third-party includes

// other declarations
namespace args = std::placeholders;


/*!
  \brief
 */
Switch::HttpInterface::HttpInterface (cppcms::service& i_service, Switch::FunctionalInterface& i_controller)
: HttpInterfaceBase (i_service),
  mr_controller (i_controller)
{
  // connect to controller callbacks
  mr_controller.ConnectToDeviceConnectionUpdateSignal (std::bind (&Switch::HttpInterface::_OnControllerDeviceConnectionUpdateSignal, this, args::_1, args::_2));
  mr_controller.ConnectToDeviceDataUpdateSignal (std::bind (&Switch::HttpInterface::_OnControllerDeviceDataUpdateSignal, this, args::_1, args::_2));
}

/*!
  \brief
 */
Switch::HttpInterface::HttpInterface (cppcms::service& i_service, Switch::FunctionalFacade i_controllerFacade)
: HttpInterfaceBase (i_service),
  mr_controller (i_controllerFacade.Get ())
{
  // connect to controller callbacks
  mr_controller.ConnectToDeviceConnectionUpdateSignal (std::bind (&Switch::HttpInterface::_OnControllerDeviceConnectionUpdateSignal, this, args::_1, args::_2));
  mr_controller.ConnectToDeviceDataUpdateSignal (std::bind (&Switch::HttpInterface::_OnControllerDeviceDataUpdateSignal, this, args::_1, args::_2));
}

Switch::HttpInterface::~HttpInterface ()
{
}

Switch::Interface::eCallResult Switch::HttpInterface::_AddDevice (const uint32_t& i_deviceId)
{
  return mr_controller.AddDevice (i_deviceId);
}

Switch::Interface::eCallResult Switch::HttpInterface::_EnumerateDevices (std::list <Switch::Interface::Device::Summary>& o_deviceInfo)
{
  return mr_controller.EnumerateDevices (o_deviceInfo);
}

Switch::Interface::eCallResult Switch::HttpInterface::_GetDeviceDetails (Switch::Interface::Device& o_deviceDetails, const uint32_t& i_deviceId)
{
  return mr_controller.GetDeviceDetails (o_deviceDetails, i_deviceId);
}

Switch::Interface::eCallResult Switch::HttpInterface::_GetDeviceValues (std::list <Switch::Interface::Device::Value>& o_values, const uint32_t& i_deviceId)
{
  return mr_controller.GetDeviceValues (o_values, i_deviceId);
}

Switch::Interface::eCallResult Switch::HttpInterface::_SetDeviceValues (const uint32_t& i_deviceId, const std::list <Switch::Interface::Device::Value>& i_values)
{
  return mr_controller.SetDeviceValues (i_deviceId, i_values);
}

Switch::Interface::eCallResult Switch::HttpInterface::_SubscribeToDeviceUpdates (const client_id_type& i_clientId, const uint32_t& i_deviceId)
{
  std::unique_lock <std::mutex> deviceUpdateBufferLock (m_deviceUpdateBufferMutex);

  // link listener id to device id
  std::pair <std::set <client_id_type>::iterator, bool> insertionResult;
  insertionResult = m_listenerIdsPerDevice [i_deviceId].insert (i_clientId);

  if (!insertionResult.second)
  {
    return Switch::Interface::CR_CLIENT_ALREADY_SUBSCRIBED;
  }

  // now also link the device id to the listener
  m_deviceIdsPerListener [i_clientId].insert (i_deviceId);

  return Switch::Interface::CR_OK;
}

Switch::Interface::eCallResult Switch::HttpInterface::_UnsubscribeFromDeviceUpdates (const client_id_type& i_listenerId, const uint32_t& i_deviceId)
{
  std::unique_lock <std::mutex> deviceUpdateBufferLock (m_deviceUpdateBufferMutex);

  std::set <client_id_type>& listenerIds = m_listenerIdsPerDevice [i_deviceId];
  size_t nrErased = listenerIds.erase (i_listenerId);

  // check if there are still other listeners for updates of the device
  if (0 == nrErased)
  {
    return Switch::Interface::CR_CLIENT_NOT_SUBSCRIBED;
  }

  // erase the device id from the listener's device ids
  std::map <client_id_type, std::set <uint32_t>>::iterator itListenerDeviceIds = m_deviceIdsPerListener.find (i_listenerId);
  SWITCH_ASSERT (m_deviceIdsPerListener.end () != itListenerDeviceIds);

  std::set <uint32_t>& listenerDeviceIds = itListenerDeviceIds->second;
  listenerDeviceIds.erase (i_deviceId);
  if (listenerDeviceIds.empty ())
  {
    // the listener is not listening to any device's updates anymore
    m_deviceIdsPerListener.erase (itListenerDeviceIds);
  }

  if (listenerIds.empty ())
  {
    // clear the update buffers for the device
    m_deviceConnectionUpdateBuffer.erase (i_deviceId);
    m_deviceDataUpdateBuffer.erase (i_deviceId);
  }

  // return the result
  return Switch::Interface::CR_OK;
}

void Switch::HttpInterface::_SendBufferedUpdatesToListener (const client_id_type& i_listenerId)
{
  std::unique_lock <std::mutex> deviceUpdateBufferLock (m_deviceUpdateBufferMutex);

  std::set <client_id_type> listenerIds;
  listenerIds.insert (i_listenerId);

  const std::set <uint32_t>& listenerDeviceIds = m_deviceIdsPerListener [i_listenerId];
  std::set <uint32_t>::const_iterator itListenerDeviceIds;
  for (itListenerDeviceIds=listenerDeviceIds.begin (); listenerDeviceIds.end ()!=itListenerDeviceIds; ++itListenerDeviceIds)
  {
    const uint32_t& listenerDeviceId = *itListenerDeviceIds;

    // check if there was a connection update
    std::map <uint32_t, DeviceConnectionUpdate>::iterator itConnectionUpdate = m_deviceConnectionUpdateBuffer.find (listenerDeviceId);
    if (m_deviceConnectionUpdateBuffer.end () != itConnectionUpdate)
    {
      const DeviceConnectionUpdate& connectionUpdate = itConnectionUpdate->second;

      // send the device connection update
      _OnDeviceUpdate (listenerIds, connectionUpdate);
    }

    // check if there was a data update
    std::map <uint32_t, DeviceDataUpdate>::iterator itDataUpdate = m_deviceDataUpdateBuffer.find (listenerDeviceId);
    if (m_deviceDataUpdateBuffer.end () != itDataUpdate)
    {
      const DeviceDataUpdate& dataUpdate = itDataUpdate->second;

      // send the device data update
      _OnDeviceUpdate (listenerIds, dataUpdate);
    }
  }
}

void Switch::HttpInterface::_OnControllerDeviceConnectionUpdateSignal (const uint32_t& i_deviceId, const Switch::Interface::Device::Connection& i_connection)
{
  std::unique_lock <std::mutex> deviceUpdateBufferLock (m_deviceUpdateBufferMutex);

  const std::set <client_id_type>& listenerIds = m_listenerIdsPerDevice [i_deviceId];
  SWITCH_DEBUG_MSG_1 ("_OnControllerDeviceConnectionUpdateSignal => %u clients listening\n", listenerIds.size ());
  if (!listenerIds.empty ())
  {
    // create the device update
    DeviceConnectionUpdate connectionUpdate;
    connectionUpdate.m_deviceId   = i_deviceId;
    connectionUpdate.m_connection = i_connection;

    // buffer the update
    m_deviceConnectionUpdateBuffer [i_deviceId] = connectionUpdate;

    // forward the update
    _OnDeviceUpdate (listenerIds, connectionUpdate);
  }
}

void Switch::HttpInterface::_OnControllerDeviceDataUpdateSignal (const uint32_t& i_deviceId, const std::list <Switch::Interface::Device::Value>& i_values)
{
  std::unique_lock <std::mutex> deviceUpdateBufferLock (m_deviceUpdateBufferMutex);

  const std::set <client_id_type>& listenerIds = m_listenerIdsPerDevice [i_deviceId];
  SWITCH_DEBUG_MSG_1 ("_OnControllerDeviceDataUpdateSignal => %u clients listening\n", listenerIds.size ());
  if (!listenerIds.empty ())
  {
    // create the device update
    DeviceDataUpdate dataUpdate;
    dataUpdate.m_deviceId   = i_deviceId;
    dataUpdate.m_dataValues = i_values;

    // buffer the update
    m_deviceDataUpdateBuffer [i_deviceId] = dataUpdate;

    // forward the update
    _OnDeviceUpdate (listenerIds, dataUpdate);
  }
}
