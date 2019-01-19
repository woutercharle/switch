/*?*************************************************************************
*                           Switch_HttpInterfaceBase.h
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
*    email                : wouter.charle@gmail.com
*
***************************************************************************/

#ifndef _SWITCH_HTTPINTERFACEBASE
#define _SWITCH_HTTPINTERFACEBASE

// Switch includes
#include <Switch_Base/Switch_CompilerConfiguration.h>
#include <Switch_API/Switch_InterfaceTypes.h>

// third-party includes
#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/rpc_json.h>
#include <cppcms/http_context.h>
#include <map>
#include <set>
#include <mutex>

namespace Switch
{

  /*!
    \brief
   */
  class HttpInterfaceBase : public cppcms::rpc::json_rpc_server
  {
  public:

    // constructor and destructor
    explicit HttpInterfaceBase (cppcms::service& i_service);
    virtual ~HttpInterfaceBase ();

    // copy constructor and assignment operator disabled
    HttpInterfaceBase (const HttpInterfaceBase& i_other) = delete;
    HttpInterfaceBase& operator= (const HttpInterfaceBase& i_other) = delete;

    void main (std::string);

    // utility methods
    void Register ();
    void About ();
    void Help ();

    // system methods
    void AddDevice        (const Switch::Interface::Device::Id& i_deviceId);
    void EnumerateDevices ();
    void GetDeviceDetails (const Switch::Interface::Device::Id& i_deviceId);
    void GetDeviceValues  (const Switch::Interface::Device::Id& i_deviceId);
    void SetDeviceValues  (const Switch::Interface::Device::Id& i_deviceId, const std::list <Switch::Interface::Device::Value>& i_deviceValues);
    //void SetDeviceProperties (const uint32_t& i_deviceId, const Switch_Device::Properties& i_deviceProperties);

    // subscription methods
    void SubscribeToDeviceUpdates     (const Switch::Interface::Device::Id& i_deviceId);
    void UnsubscribeFromDeviceUpdates (const Switch::Interface::Device::Id& i_deviceId);
    void ListenToDeviceUpdates        ();

  protected:

    typedef size_t client_id_type;  ///< Type of client identifiers.
    typedef std::map <client_id_type, booster::shared_ptr <cppcms::rpc::json_call>> device_update_listeners_type;

    class DeviceConnectionUpdate
    {
    public:

      DeviceConnectionUpdate ();

      uint32_t                              m_index;      ///< Unique index identifying this device update.
      Switch::Interface::Device::Id         m_deviceId;   ///< Id of the device this update is related to.
      Switch::Interface::Device::Connection m_connection;
    };

    class DeviceDataUpdate
    {
    public:

      DeviceDataUpdate ();
      DeviceDataUpdate& operator= (const DeviceDataUpdate& i_other);

      uint32_t                                      m_index;      ///< Unique index identifying this device update.
      Switch::Interface::Device::Id                 m_deviceId;   ///< Id of the device this update is related to.
      std::list <Switch::Interface::Device::Value>  m_dataValues;
    };

    // utility methods
    void _Help ();
    void _OnDeviceUpdate (const std::set <client_id_type>& i_deviceUpdateListenerIds, const DeviceConnectionUpdate& i_update);
    void _OnDeviceUpdate (const std::set <client_id_type>& i_deviceUpdateListenerIds, const DeviceDataUpdate& i_update);

    // system methods
    virtual Switch::Interface::eCallResult _AddDevice         (const Switch::Interface::Device::Id& i_deviceId) = 0;
    virtual Switch::Interface::eCallResult _EnumerateDevices  (std::list <Switch::Interface::Device::Summary>& o_deviceInfo) = 0;
    virtual Switch::Interface::eCallResult _GetDeviceDetails  (Switch::Interface::Device& o_deviceDetails, const Switch::Interface::Device::Id& i_deviceId) = 0;
    virtual Switch::Interface::eCallResult _GetDeviceValues   (std::list <Switch::Interface::Device::Value>& o_values, const Switch::Interface::Device::Id& i_deviceId) = 0;
    virtual Switch::Interface::eCallResult _SetDeviceValues   (const Switch::Interface::Device::Id& i_deviceId, const std::list <Switch::Interface::Device::Value>& i_values) = 0;
    //virtual Switch::Interface::eCallResult _SetDeviceProperties (const uint32_t& i_deviceId, const Switch_Device::Properties& i_deviceProperties) = 0;

    // subscription methods
    virtual Switch::Interface::eCallResult _SubscribeToDeviceUpdates      (const client_id_type& i_clientId, const Switch::Interface::Device::Id& i_deviceId) = 0;
    virtual Switch::Interface::eCallResult _UnsubscribeFromDeviceUpdates  (const client_id_type& i_clientId, const Switch::Interface::Device::Id& i_deviceId) = 0;
    virtual void _SendBufferedUpdatesToListener (const client_id_type& i_clientId) = 0;

  private:

    void _GenerateClientId      (client_id_type& o_clientId);
    void _RemoveListenerContext (const client_id_type& i_clientId);
    void _OnListenerAsyncFlushOutputCompleted (const client_id_type& i_clientId, const cppcms::http::context::completion_type& i_completionType);
    void _HandleDeviceUpdate    (const std::set <client_id_type>& i_deviceUpdateListenerIds, const Switch::Interface::Device::Id& i_deviceId, const cppcms::json::value& i_message);

    device_update_listeners_type  m_deviceUpdateListeners;      ///< Container with the context of all active listeners, mapped to from listener identifiers.
    mutable std::mutex            m_deviceUpdateListenersMutex; ///< Protects concurrently accessing and using m_deviceUpdateListeners
  };
}

#endif // _SWITCH_HTTPINTERFACEBASE
