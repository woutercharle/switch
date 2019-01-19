/*?*************************************************************************
*                           Switch_ControllerFunctionalInterface.h
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

#ifndef _SWITCH_CONTROLLERFUNCTIONALINTERFACE
#define _SWITCH_CONTROLLERFUNCTIONALINTERFACE

// project includes
#include "Switch_InterfaceDevice.h"

// switch includes
#include <Switch_Base/Switch_CompilerConfiguration.h>

// third party includes
#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/signals2.hpp>


namespace Switch
{
  class ControllerFunctionalInterface
  {
  public:

    typedef boost::signals2::signal <void (const uint32_t&, const Switch::Interface::Device::Connection&)>::slot_type DeviceConnectionUpdateSlot;
    typedef boost::signals2::signal <void (const uint32_t&, const std::list <Switch::Interface::Device::Value>&)>::slot_type DeviceDataUpdateSlot;

    enum eCallResult
    {
      CR_INVALID        =-1,
      CR_OK             = 0,
      CR_STOPPED        = 1,
      CR_UNKNOWN_DEVICE = 2
    };

    // contsructor and destructor
    ControllerFunctionalInterface () = default;
    virtual ~ControllerFunctionalInterface () = default;

    // copy constructor and assignment operator
    ControllerFunctionalInterface (const ControllerFunctionalInterface& i_other) = delete;
    ControllerFunctionalInterface& operator= (const ControllerFunctionalInterface& i_other) = delete;

    // signals
    virtual boost::signals2::connection ConnectToDeviceConnectionUpdateSignal (const DeviceConnectionUpdateSlot& i_receiver) = 0;
    virtual boost::signals2::connection ConnectToDeviceDataUpdateSignal (const DeviceDataUpdateSlot& i_receiver) = 0;

    // functionality
    virtual eCallResult AddDevice        (const uint32_t& i_deviceAddress) = 0;
    virtual eCallResult EnumerateDevices (std::list <Switch::Interface::Device::Summary>& o_deviceInfo) = 0;
    virtual eCallResult GetDeviceDetails (Switch::Interface::Device& o_deviceDetails, const uint32_t& i_deviceAddress) = 0;
    virtual eCallResult GetDeviceValues  (std::list <Switch::Interface::Device::Value>& o_values, const uint32_t& i_deviceAddress) = 0;
    virtual eCallResult SetDeviceValues  (const uint32_t& i_deviceAddress, const std::list <Switch::Interface::Device::Value>& i_values) = 0;

  };
}

#endif // _SWITCH_CONTROLLERFUNCTIONALINTERFACE
