/*?*************************************************************************
*                           Switch_ControllerFunctionalFacade.cpp
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

#include "Switch_ControllerFunctionalFacade.h"

// project includes
#include "Switch_ControllerFunctionalInterface.h"

// switch includes

// third-party includes

// other declarations
using namespace std::placeholders;


/*!
  \brief Constructor
 */
Switch::ControllerFunctionalFacade::ControllerFunctionalFacade (Switch::ControllerFunctionalInterface& io_controller)
: mr_controller (io_controller)
{
}

/*!
  \brief Destructor
 */
Switch::ControllerFunctionalFacade::~ControllerFunctionalFacade () noexcept (true)
{
}

/*!
  \brief Copy constructor
 */
Switch::ControllerFunctionalFacade::ControllerFunctionalFacade (const Switch::ControllerFunctionalFacade& i_other)
: mr_controller (i_other.mr_controller)
{
}

boost::signals2::connection Switch::ControllerFunctionalFacade::ConnectToDeviceConnectionUpdateSignal (const DeviceConnectionUpdateSlot& i_receiver)
{
  return mr_controller.ConnectToDeviceConnectionUpdateSignal (i_receiver);
}

boost::signals2::connection Switch::ControllerFunctionalFacade::ConnectToDeviceDataUpdateSignal (const DeviceDataUpdateSlot& i_receiver)
{
  return mr_controller.ConnectToDeviceDataUpdateSignal (i_receiver);
}

Switch::ControllerFunctionalFacade::eCallResult Switch::ControllerFunctionalFacade::AddDevice (const uint32_t& i_deviceAddress)
{
  return mr_controller.AddDevice (i_deviceAddress);
}

Switch::ControllerFunctionalFacade::eCallResult Switch::ControllerFunctionalFacade::EnumerateDevices (std::list <Switch::Interface::Device::Summary>& o_deviceInfo)
{
  return mr_controller.EnumerateDevices (o_deviceInfo);
}

Switch::ControllerFunctionalFacade::eCallResult Switch::ControllerFunctionalFacade::GetDeviceDetails (Switch::Interface::Device& o_deviceDetails, const uint32_t& i_deviceAddress)
{
  return mr_controller.GetDeviceDetails (o_deviceDetails, i_deviceAddress);
}

Switch::ControllerFunctionalFacade::eCallResult Switch::ControllerFunctionalFacade::GetDeviceValues (std::list <Switch::Interface::Device::Value>& o_values, const uint32_t& i_deviceAddress)
{
  return mr_controller.GetDeviceValues (o_values, i_deviceAddress);
}

Switch::ControllerFunctionalFacade::eCallResult Switch::ControllerFunctionalFacade::SetDeviceValues (const uint32_t& i_deviceAddress, const std::list <Switch::Interface::Device::Value>& i_values)
{
  return mr_controller.SetDeviceValues (i_deviceAddress, i_values);
}

