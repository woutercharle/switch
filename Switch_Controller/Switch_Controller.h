/*?*************************************************************************
*                           Switch_Controller.h
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

#ifndef _SWITCH_CONTROLLER
#define _SWITCH_CONTROLLER

// project includes
#include "Switch_InterfaceDevice.h"
#include "Switch_ControllerFunctionalInterface.h"

// switch includes
#include <Switch_Base/Switch_CompilerConfiguration.h>
#include <Switch_Base/Switch_Types.h>
#include <Switch_Application/Switch_ApplicationModule.h>
#include <Switch_Device/Switch_DataContainer.h>

// third party includes
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/signals2.hpp>


namespace Switch
{
  // forward declarations
  class DeviceStore;
  class Router;
  class DataPayload;

  class Controller : public ControllerFunctionalInterface, public Switch::ApplicationModule
  {
  public:

    typedef boost::signals2::signal <void (const uint32_t&, const Switch::Interface::Device::Connection&)> DeviceConnectionUpdateSignal;
    typedef boost::signals2::signal <void (const uint32_t&, const std::list <Switch::Interface::Device::Value>&)> DeviceDataUpdateSignal;

    /*!
      \brief Parameters container class
     */
    class Parameters : public Switch::ApplicationModule::Parameters
    {
    public:

      /*!
        \brief Default constructor.
       */
      explicit Parameters ();
      /*!
        \brief Destructor.
       */
      virtual ~Parameters ();

      // using default copy constructor and assignment operator
      Parameters (const Parameters& i_other) = default;
      Parameters& operator= (const Parameters& i_other) = default;

      // parameter members
      uint32_t    m_updateCycleTimeMicros;            ///< The time in microseconds between two update cycles.
    };

    /*!
      \brief Default constructor.
     */
    Controller ();
    /*!
      \brief Constructor.

      \param [in] i_parameters Parameters to initialize the object with.
     */
    explicit Controller (const Parameters& i_parameters);
    /*!
      \brief Destructor.
     */
    virtual ~Controller () noexcept (true);

    // copy constructor and assignment operator are disabled
    Controller (const Controller& i_other) = delete;
    Controller& operator= (const Controller& i_other) = delete;

    // signals
    boost::signals2::connection ConnectToDeviceConnectionUpdateSignal (const DeviceConnectionUpdateSlot& i_receiver);
    boost::signals2::connection ConnectToDeviceDataUpdateSignal (const DeviceDataUpdateSlot& i_receiver);

    // functionality
    eCallResult AddDevice        (const uint32_t& i_deviceAddress);
    eCallResult EnumerateDevices (std::list <Switch::Interface::Device::Summary>& o_deviceInfo);
    eCallResult GetDeviceDetails (Switch::Interface::Device& o_deviceDetails, const uint32_t& i_deviceAddress);
    eCallResult GetDeviceValues  (std::list <Switch::Interface::Device::Value>& o_values, const uint32_t& i_deviceAddress);
    eCallResult SetDeviceValues  (const uint32_t& i_deviceAddress, const std::list <Switch::Interface::Device::Value>& i_values);

  protected:

    void _SetupParameterContainer ();
    virtual void _SetParameters (const Switch::ParameterizedObject::Parameters& i_parameters);
    virtual void _GetParameters (Switch::ParameterizedObject::Parameters& o_parameters) const;

    virtual void _Prepare ();
    virtual void _Start   ();
    virtual void _Pause   ();
    virtual void _Stop    ();

  private:

    // utility methods
    void _Construct ();
    void _Run ();
    bool _HandleDataNewNodeDiscovered ();
    bool _HandleDataNodeConnectionUpdate ();
    bool _HandleDataNodeDataReceived ();
    bool _HandleDataNodeDataTransmitted ();
    bool _HandleDataAddDevice ();
    bool _HandleDataSetDeviceValues ();

    // callback handlers
    void _OnRouterNewNodeDiscovered (const switch_device_address_type& i_deviceAddress, const Switch::DeviceInfo& i_deviceInfo);
    void _OnRouterNodeConnectionUpdate (const switch_device_address_type& i_deviceAddress, const bool& i_connected);
    bool _OnRouterNodeDataReceived (const switch_device_address_type& i_deviceAddress, const Switch::DataPayload& i_dataPayload);
    void _OnRouterNodeDataTransmitted (const switch_device_address_type& i_deviceAddress, const bool& i_result);

    // variables

    // threading variables
    std::atomic <eObjectState>  m_controllerState;
    std::thread                 m_controllerThread;
    mutable std::mutex          m_controllerMutex;
    std::condition_variable     m_updateCondition;

    // signal variables
    DeviceConnectionUpdateSignal  m_deviceConnectionUpdateSignal;
    DeviceDataUpdateSignal        m_deviceDataUpdateSignal;

    // router callback variables
    mutable std::mutex m_routerDataMutex;
    std::list <std::pair <switch_device_address_type, Switch::DeviceInfo>>  m_dataNewNodeDiscovered;
    std::list <std::pair <switch_device_address_type, bool>>                m_dataNodeConnectionUpdate;
    std::list <std::pair <switch_device_address_type, Switch::DataPayload>> m_dataNodeDataReceived;
    std::list <std::pair <switch_device_address_type, bool>>                m_dataNodeDataTransmitted;

    // interface input variables
    mutable std::mutex m_interfaceDataMutex;
    std::list <switch_device_address_type>                                                          m_dataAddDevice;
    std::list <std::pair <switch_device_address_type, std::list <Switch::DataContainer::Element>>>  m_dataSetDeviceValues;

    // data members
    Switch::DeviceStore*  m_pDeviceStore;
    Switch::Router*       m_pRouter;

    // parameters
    uint32_t    m_updateCycleTimeMicros;            ///< The time in microseconds between two update cycles.
  };
}

#endif // _SWITCH_CONTROLLER
