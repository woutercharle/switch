/*?*************************************************************************
*                           ControllerFunctionalFacade.h
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

#ifndef _SWITCH_CONTROLLERFUNCTIONALFACADE
#define _SWITCH_CONTROLLERFUNCTIONALFACADE

// project includes
#include "Switch_ControllerFunctionalInterface.h"
#include "Switch_InterfaceDevice.h"

// switch includes
#include <Switch_Base/Switch_CompilerConfiguration.h>
#include <Switch_Base/Switch_Types.h>

// third party includes
#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/signals2.hpp>


namespace Switch
{
  class ControllerFunctionalFacade : public ControllerFunctionalInterface
  {
  public:

    /*!
      \brief Constructor.
     */
    explicit ControllerFunctionalFacade (ControllerFunctionalInterface& io_controller);
    /*!
      \brief Destructor.
     */
    virtual ~ControllerFunctionalFacade () noexcept (true);

    // copy constructor and assignment operator
    ControllerFunctionalFacade (const ControllerFunctionalFacade& i_other);
    ControllerFunctionalFacade& operator= (const ControllerFunctionalFacade& i_other) = delete;

    // signals
    boost::signals2::connection ConnectToDeviceConnectionUpdateSignal (const DeviceConnectionUpdateSlot& i_receiver);
    boost::signals2::connection ConnectToDeviceDataUpdateSignal (const DeviceDataUpdateSlot& i_receiver);

    // functionality
    eCallResult AddDevice        (const uint32_t& i_deviceAddress);
    eCallResult EnumerateDevices (std::list <Switch::Interface::Device::Summary>& o_deviceInfo);
    eCallResult GetDeviceDetails (Switch::Interface::Device& o_deviceDetails, const uint32_t& i_deviceAddress);
    eCallResult GetDeviceValues  (std::list <Switch::Interface::Device::Value>& o_values, const uint32_t& i_deviceAddress);
    eCallResult SetDeviceValues  (const uint32_t& i_deviceAddress, const std::list <Switch::Interface::Device::Value>& i_values);

  private:

    ControllerFunctionalInterface& mr_controller;
  };
}

#endif // _SWITCH_CONTROLLERFUNCTIONALFACADE
