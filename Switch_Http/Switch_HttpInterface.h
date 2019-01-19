/*?*************************************************************************
*                           Switch_HttpInterface.h
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
*    email                : wouter.charle@gmail.com
*
***************************************************************************/

#ifndef _SWITCH_HTTPINTERFACE
#define _SWITCH_HTTPINTERFACE

// project includes
#include "Switch_HttpInterfaceBase.h"

// Switch includes
#include <Switch_Base/Switch_CompilerConfiguration.h>
#include <Switch_API/Switch_FunctionalFacade.h>

// third-party includes
#include <set>

namespace Switch
{
  // forward declarations
  class FunctionalFacade;

  /*!
    \brief
   */
  class HttpInterface : public HttpInterfaceBase
  {
  public:

    // constructor and destructor
    explicit HttpInterface (cppcms::service& i_service, FunctionalInterface& i_controller);
    explicit HttpInterface (cppcms::service& i_service, FunctionalFacade i_controllerFacade);
    virtual ~HttpInterface ();

    // copy constructor and assignment operator disabled
    HttpInterface (const HttpInterface& i_other) = delete;
    HttpInterface& operator= (const HttpInterface& i_other) = delete;

  protected:

    // system methods
    virtual Switch::Interface::eCallResult _AddDevice         (const Switch::Interface::Device::Id& i_deviceId);
    virtual Switch::Interface::eCallResult _EnumerateDevices  (std::list <Switch::Interface::Device::Summary>& o_deviceInfo);
    virtual Switch::Interface::eCallResult _GetDeviceDetails  (Switch::Interface::Device& o_deviceDetails, const Switch::Interface::Device::Id& i_deviceId);
    virtual Switch::Interface::eCallResult _GetDeviceValues   (std::list <Switch::Interface::Device::Value>& o_values, const Switch::Interface::Device::Id& i_deviceId);
    virtual Switch::Interface::eCallResult _SetDeviceValues   (const Switch::Interface::Device::Id& i_deviceId, const std::list <Switch::Interface::Device::Value>& i_values);
    //virtual Switch::Interface::eCallResult _SetDeviceProperties (const uint32_t& i_deviceId, const Switch_Device::Properties& i_deviceProperties);

    // subscription methods
    virtual Switch::Interface::eCallResult _SubscribeToDeviceUpdates      (const client_id_type& i_listenerId, const Switch::Interface::Device::Id& i_deviceId);
    virtual Switch::Interface::eCallResult _UnsubscribeFromDeviceUpdates  (const client_id_type& i_listenerId, const Switch::Interface::Device::Id& i_deviceId);
    void _SendBufferedUpdatesToListener (const client_id_type& i_listenerId);

    // slots for controller signal
    void _OnControllerDeviceConnectionUpdateSignal (const Switch::Interface::Device::Id& i_deviceId, const Switch::Interface::Device::Connection& i_connection);
    void _OnControllerDeviceDataUpdateSignal (const Switch::Interface::Device::Id& i_deviceId, const std::list <Switch::Interface::Device::Value>& i_values);

  private:

    FunctionalInterface& mr_controller;

    std::map <client_id_type, std::set <Switch::Interface::Device::Id>> m_deviceIdsPerListener;  ///< maps the id of a listeners subscribed to updates of specific devices to those device's ids
    std::map <Switch::Interface::Device::Id, std::set <client_id_type>> m_listenerIdsPerDevice;  ///< maps the id's of listeners subscribed to updates of a specific device on that device's id
    std::map <Switch::Interface::Device::Id, DeviceConnectionUpdate>  m_deviceConnectionUpdateBuffer;
    std::map <Switch::Interface::Device::Id, DeviceDataUpdate>        m_deviceDataUpdateBuffer;
    mutable std::mutex m_deviceUpdateBufferMutex;
  };
}

#endif // _SWITCH_HTTPINTERFACE
