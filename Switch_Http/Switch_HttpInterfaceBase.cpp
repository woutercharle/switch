/*?*************************************************************************
*                           Switch_HttpInterfaceBase.cpp
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
*    email                : wouter.charle@gmail.com
*
***************************************************************************/

#include "Switch_HttpInterfaceBase.h"

// Switch includes
#include <Switch_Base/Switch_Debug.h>

// project includes
#include "Switch_HttpInterfaceTraits.h"

// third-party includes
#include <cppcms/http_response.h>
#include <cppcms/http_context.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/session_interface.h>
#include <cppcms/http_request.h>

// other declarations
namespace args = std::placeholders;


Switch::HttpInterfaceBase::HttpInterfaceBase (cppcms::service& i_service)
: cppcms::rpc::json_rpc_server (i_service)
{
  // bind all rpc calls to methods
  //bind ("Help", cppcms::rpc::json_method (&Switch::HttpInterfaceBase::Help,  this), method_role);

  bind ("Register",         cppcms::rpc::json_method (&Switch::HttpInterfaceBase::Register,          this), method_role);
  bind ("AddDevice",        cppcms::rpc::json_method (&Switch::HttpInterfaceBase::AddDevice,         this), method_role);
  bind ("EnumerateDevices", cppcms::rpc::json_method (&Switch::HttpInterfaceBase::EnumerateDevices,  this), method_role);
  bind ("GetDeviceDetails", cppcms::rpc::json_method (&Switch::HttpInterfaceBase::GetDeviceDetails,  this), method_role);
  bind ("GetDeviceValues",  cppcms::rpc::json_method (&Switch::HttpInterfaceBase::GetDeviceValues,   this), method_role);
  bind ("SetDeviceValues",  cppcms::rpc::json_method (&Switch::HttpInterfaceBase::SetDeviceValues,   this), method_role);
  //bind ("SetDeviceProperties",          cppcms::rpc::json_method (&Switch::HttpInterfaceBase::SetDeviceProperties,           this), method_role);

  bind ("SubscribeToDeviceUpdates", 	  cppcms::rpc::json_method (&Switch::HttpInterfaceBase::SubscribeToDeviceUpdates,      this), method_role);
  bind ("UnsubscribeFromDeviceUpdates", cppcms::rpc::json_method (&Switch::HttpInterfaceBase::UnsubscribeFromDeviceUpdates,  this), method_role);
  bind ("ListenToDeviceUpdates", 	      cppcms::rpc::json_method (&Switch::HttpInterfaceBase::ListenToDeviceUpdates,         this), method_role);

  dispatcher().assign ("/Help", &Switch::HttpInterfaceBase::Help, this);
  mapper().assign ("Help, /Help");

  dispatcher().assign ("/About", &Switch::HttpInterfaceBase::About, this);
  mapper().assign ("About, /About");

  /*dispatcher().assign ("", &Switch::HttpInterfaceBase::About, this);
  mapper().assign ("");*/

  // set the root
  mapper ().root ("/Switch");
}

Switch::HttpInterfaceBase::~HttpInterfaceBase ()
{
}

void Switch::HttpInterfaceBase::main (std::string i_string)
{
  SWITCH_DEBUG_MSG_2 ("RPC called from %s: %s\n", request ().getenv ("HTTP_ORIGIN").c_str (), i_string.c_str ());

  // handle CORS (Cross Origin Resource Sharing)
  response ().set_header ("Access-Control-Allow-Origin", request ().getenv ("HTTP_ORIGIN"));
  response ().set_header ("Access-Control-Allow-Credentials", "true");
  response ().set_header ("Access-Control-Allow-Headers", "Content-Type");
  if ("OPTIONS" == request ().request_method ())
  {
    // this was a pure options query
    return;
  }
  else if ("GET" == request ().request_method ())
  {
    // this is a long polling call
    SWITCH_DEBUG_MSG_0 ("GET\n");
  }
  cppcms::rpc::json_rpc_server::main (i_string);
}

void Switch::HttpInterfaceBase::About ()
{
  printf ("About called\n");
  response().set_html_header ();
  response().out() << "<h1>This is the switch system interface</h1>\n";
}

void Switch::HttpInterfaceBase::Help ()
{
  printf ("Help called\n");
  response().set_html_header ();
  response().out() << "<h1>This is the switch system interface help</h1>\n";
}

void Switch::HttpInterfaceBase::Register ()
{
  try
  {
    // 0. Validate the call

    // 1. Validate the arguments

    // 2. Register the client
    client_id_type clientId;
    if (!session ().is_set ("clientId"))
    {
      // generate new client id and set in the session
      _GenerateClientId (clientId);
      session ().set ("clientId", clientId);
    }
    else
    {
      // load the listener id from the session
      clientId = session ().get <client_id_type> ("clientId");
    }

    // 3. Send response
    cppcms::json::value result;
    result.set ("result", Switch::Interface::CR_OK);
    cppcms::rpc::json_rpc_server::return_result (result);
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}

void Switch::HttpInterfaceBase::AddDevice (const uint32_t& i_deviceId)
{
  try
  {
    // 0. Validate the call
    if (!session ().is_set ("clientId"))
    {
      cppcms::rpc::json_rpc_server::return_error ("client not registered");
    }

    // 1. Validate the arguments

    // 2. Call the framework
    Switch::Interface::eCallResult callResult = _AddDevice (i_deviceId);

    // 3. Send response
    cppcms::json::value result;
    result.set ("result", callResult);
    cppcms::rpc::json_rpc_server::return_result (result);
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}

void Switch::HttpInterfaceBase::EnumerateDevices ()
{
  try
  {
    // 0. Validate the call
    if (!session ().is_set ("clientId"))
    {
      cppcms::rpc::json_rpc_server::return_error ("client not registered");
    }

    // 1. Validate the arguments

    // 2. Call the framework
    std::list <Switch::Interface::Device::Summary> outDevices;
    Switch::Interface::eCallResult callResult = _EnumerateDevices (outDevices);

    // 3. Send response
    cppcms::json::value result;
    result.set ("result", callResult);
    result.set ("devices", outDevices);
    cppcms::rpc::json_rpc_server::return_result (result);
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}

void Switch::HttpInterfaceBase::GetDeviceDetails (const uint32_t& i_deviceId)
{
  try
  {
    // 0. Validate the call
    if (!session ().is_set ("clientId"))
    {
      cppcms::rpc::json_rpc_server::return_error ("client not registered");
    }

    // 1. Validate the arguments

    // 2. Call the framework
    Switch::Interface::Device outDeviceDetails;
    Switch::Interface::eCallResult callResult = _GetDeviceDetails (outDeviceDetails, i_deviceId);

    // 3. Send response
    cppcms::json::value result;
    result.set ("result", callResult);
    result.set ("deviceDetails", outDeviceDetails);
    cppcms::rpc::json_rpc_server::return_result (result);
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}

void Switch::HttpInterfaceBase::GetDeviceValues (const uint32_t& i_deviceId)
{
  try
  {
    // 0. Validate the call
    if (!session ().is_set ("clientId"))
    {
      cppcms::rpc::json_rpc_server::return_error ("client not registered");
    }

    // 1. Validate the arguments

    // 2. Call the framework
    std::list <Switch::Interface::Device::Value> outValues;
    Switch::Interface::eCallResult callResult = _GetDeviceValues (outValues, i_deviceId);

    // 3. Send response
    cppcms::json::value result;
    result.set ("result", callResult);
    result.set ("deviceValues", outValues);
    cppcms::rpc::json_rpc_server::return_result (result);
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}

void Switch::HttpInterfaceBase::SetDeviceValues (const uint32_t& i_deviceId, const std::list <Switch::Interface::Device::Value>& i_values)
{
  try
  {
    // 0. Validate the call
    if (!session ().is_set ("clientId"))
    {
      cppcms::rpc::json_rpc_server::return_error ("client not registered");
    }

    // 1. Validate the arguments

    // 2. Call the framework
    Switch::Interface::eCallResult callResult = _SetDeviceValues (i_deviceId, i_values);

    // 3. Send response
    cppcms::json::value result;
    result.set ("result", callResult);
    cppcms::rpc::json_rpc_server::return_result (result);
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}

/*void Switch::HttpInterfaceBase::SetDeviceProperties (const uint32_t& i_deviceId, const Switch_Device::Properties& i_deviceProperties)
{
  try
  {
    // 0. Validate the call
    if (!session ().is_set ("clientId"))
    {
      cppcms::rpc::json_rpc_server::return_error ("client not registered");
    }

    // 1. Validate the arguments

    // 2. Call the framework
    Switch::Interface::eCallResult callResult = _SetDeviceProperties (i_deviceId, i_deviceProperties);

    // 3. Send response
    cppcms::json::value result;
    result.set ("result", callResult);
    cppcms::rpc::json_rpc_server::return_result (result);
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}*/

void Switch::HttpInterfaceBase::SubscribeToDeviceUpdates (const uint32_t& i_deviceId)
{
  try
  {
    // 0. Validate the call
    if (!session ().is_set ("clientId"))
    {
      cppcms::rpc::json_rpc_server::return_error ("client not registered");
    }
    client_id_type clientId = session ().get <client_id_type> ("clientId");

    // 1. Validate the arguments

    // 2. Call the framework
    Switch::Interface::eCallResult callResult = _SubscribeToDeviceUpdates (clientId, i_deviceId);

    // 3. Send response
    cppcms::json::value result;
    result.set ("result", callResult);
    cppcms::rpc::json_rpc_server::return_result (result);
  }
  catch (std::exception& i_exception)
  {
    cppcms::rpc::json_rpc_server::return_error (i_exception.what ());
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}

void Switch::HttpInterfaceBase::UnsubscribeFromDeviceUpdates (const uint32_t& i_deviceId)
{
  try
  {
    // 0. Validate the call
    if (!session ().is_set ("clientId"))
    {
      cppcms::rpc::json_rpc_server::return_error ("client not registered");
    }
    client_id_type clientId = session ().get <client_id_type> ("clientId");

    // 1. Validate the arguments

    // 2. Call the framework
    Switch::Interface::eCallResult callResult = _UnsubscribeFromDeviceUpdates (clientId, i_deviceId);

    // 3. Send response
    cppcms::json::value result;
    result.set ("result", callResult);
    cppcms::rpc::json_rpc_server::return_result (result);
  }
  catch (std::exception& i_exception)
  {
    cppcms::rpc::json_rpc_server::return_error (i_exception.what ());
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}

void Switch::HttpInterfaceBase::ListenToDeviceUpdates ()
{
  try
  {
    // 0. Validate the call
    if (!session ().is_set ("clientId"))
    {
      cppcms::rpc::json_rpc_server::return_error ("client not registered");
      return;
    }
    client_id_type clientId = session ().get <client_id_type> ("clientId");

    // 1. Validate the arguments

    // 2. Append the context to the map of device update listener
    booster::shared_ptr <cppcms::rpc::json_call> listenerCall = release_call ();
    listenerCall->context ().response ().io_mode (cppcms::http::response::asynchronous);
    std::pair <device_update_listeners_type::iterator, bool> insertionResult ({ m_deviceUpdateListeners.end (), false });
    {
      std::unique_lock <std::mutex> deviceUpdateListenersLock (m_deviceUpdateListenersMutex);
      insertionResult = m_deviceUpdateListeners.insert ({ clientId, listenerCall });
    }
    // note: is access to m_deviceUpdateListeners thread-safe?
    if (!insertionResult.second)
    {
      cppcms::rpc::json_rpc_server::return_error ("error, listener with same clientId already listening");
      return;
    }

    // 3. Handle connection time-outs
    listenerCall->context ().async_on_peer_reset
    (
      std::bind (&Switch::HttpInterfaceBase::_RemoveListenerContext, this, clientId)
    );

    // 4. Send buffered updates
    _SendBufferedUpdatesToListener (clientId);
  }
  catch (std::exception& i_exception)
  {
    cppcms::rpc::json_rpc_server::return_error (i_exception.what ());
  }
  catch (...)
  {
    cppcms::rpc::json_rpc_server::return_error ("error");
  }
}

Switch::HttpInterfaceBase::DeviceConnectionUpdate::DeviceConnectionUpdate ()
{
  static uint32_t nextAvailableIndex = 0;
  m_index = nextAvailableIndex;
  ++nextAvailableIndex;
}

Switch::HttpInterfaceBase::DeviceDataUpdate::DeviceDataUpdate ()
{
  static uint32_t nextAvailableIndex = 0;
  m_index = nextAvailableIndex;
  ++nextAvailableIndex;
}

Switch::HttpInterfaceBase::DeviceDataUpdate& Switch::HttpInterfaceBase::DeviceDataUpdate::operator= (const Switch::HttpInterfaceBase::DeviceDataUpdate& i_other)
{
  if (this != &i_other)
  {
    SWITCH_ASSERT (i_other.m_deviceId == m_deviceId);

    m_index = i_other.m_index;
    std::list <Switch::Interface::Device::Value>::const_iterator itOtherValues;
    for (itOtherValues=i_other.m_dataValues.begin (); i_other.m_dataValues.end ()!=itOtherValues; ++itOtherValues)
    {
      const Switch::Interface::Device::Value& otherValue = *itOtherValues;

      // find the corresponding already buffered value
      std::list <Switch::Interface::Device::Value>::iterator itUpdateValues;
      for (itUpdateValues=m_dataValues.begin (); m_dataValues.end ()!=itUpdateValues; ++itUpdateValues)
      {
        if ((*itUpdateValues).m_address == otherValue.m_address)
        {
          break;
        }
      }
      if (m_dataValues.end () != itUpdateValues)
      {
        (*itUpdateValues).m_value = otherValue.m_value;
      }
      else
      {
        // this value is not yet buffered, add it to the list
        m_dataValues.push_back (otherValue);
      }
    }
  }

  return *this;
}

void Switch::HttpInterfaceBase::_OnDeviceUpdate (const std::set <client_id_type>& i_deviceUpdateListenerIds, const DeviceDataUpdate& i_update)
{
  // create the message
  cppcms::json::value deviceData;
  deviceData.set ("deviceId", i_update.m_deviceId);
  deviceData.set ("values", i_update.m_dataValues);
  cppcms::json::value deviceUpdate;
  deviceUpdate.set ("index", i_update.m_index);
  deviceUpdate.set ("event", "dataUpdate");
  deviceUpdate.set ("data", deviceData);

  // send the message to the handler
  _HandleDeviceUpdate (i_deviceUpdateListenerIds, i_update.m_deviceId, deviceUpdate);
}

void Switch::HttpInterfaceBase::_OnDeviceUpdate (const std::set <client_id_type>& i_deviceUpdateListenerIds, const DeviceConnectionUpdate& i_update)
{
  // create the message
  cppcms::json::value deviceData;
  deviceData.set ("deviceId", i_update.m_deviceId);
  deviceData.set ("connection", i_update.m_connection);
  cppcms::json::value deviceUpdate;
  deviceUpdate.set ("index", i_update.m_index);
  deviceUpdate.set ("event", "connectionUpdate");
  deviceUpdate.set ("data", deviceData);

  // send the message to the handler
  _HandleDeviceUpdate (i_deviceUpdateListenerIds, i_update.m_deviceId, deviceUpdate);
}

void Switch::HttpInterfaceBase::_HandleDeviceUpdate (const std::set <client_id_type>& i_deviceUpdateListenerIds, const uint32_t& i_deviceId, const cppcms::json::value& i_message)
{
  std::unique_lock <std::mutex> deviceUpdateListenersLock (m_deviceUpdateListenersMutex);

  // send the message to all listeners
  for (std::set <client_id_type>::const_iterator itListenerId=i_deviceUpdateListenerIds.begin (); i_deviceUpdateListenerIds.end ()!=itListenerId; ++itListenerId)
  {
    // get the link to the listener's context
    device_update_listeners_type::iterator itListener = m_deviceUpdateListeners.find (*itListenerId);
    if (m_deviceUpdateListeners.end () == itListener)
    {
      continue;
    }

    // get the listener's context
    booster::shared_ptr <cppcms::rpc::json_call> listenerCall = itListener->second;

    // send the response to the listener
    //listenerCall->context ().response ().set_plain_text_header ();
    listenerCall->context ().response ().out () << i_message;
    listenerCall->context ().async_flush_output
    (
      std::bind (&Switch::HttpInterfaceBase::_OnListenerAsyncFlushOutputCompleted, this, itListener->first, args::_1)
    );
  }
}

void Switch::HttpInterfaceBase::_GenerateClientId (client_id_type& o_clientId)
{
  static client_id_type nextClientId = 0;
  o_clientId = nextClientId;
  ++nextClientId;
}

void Switch::HttpInterfaceBase::_RemoveListenerContext (const client_id_type& i_clientId)
{
  std::unique_lock <std::mutex> deviceUpdateListenersLock (m_deviceUpdateListenersMutex);
  m_deviceUpdateListeners.erase (i_clientId);
}

void Switch::HttpInterfaceBase::_OnListenerAsyncFlushOutputCompleted (const client_id_type& i_clientId, const cppcms::http::context::completion_type& i_completionType)
{
  if (cppcms::http::context::operation_aborted == i_completionType)
  {
    // note: is access to m_deviceUpdateListeners thread-safe?
    _RemoveListenerContext (i_clientId);
  }
}

void Switch::HttpInterfaceBase::_Help ()
{
}

