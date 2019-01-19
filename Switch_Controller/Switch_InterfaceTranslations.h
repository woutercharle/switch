/*?*************************************************************************
*                           Switch_InterfaceDevice.h
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

#ifndef _SWITCH_INTERFACE_TRANSLATIONS
#define _SWITCH_INTERFACE_TRANSLATIONS

// project includes
#include "Switch_InterfaceDevice.h"

// Switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Device/Switch_Device.h"

// third-party includes


namespace Switch
{
  namespace Interface
  {
    void Translate (Switch::Interface::Device::Product& o_product, const Switch::Device& i_device);
    void Translate (Switch::Interface::Device::DataFormat& o_dataFormat, const Switch::Device& i_device);
    void Translate (Switch::Interface::Device::Value& o_value, const Switch::DataContainer::Element& i_containerElement);
    void Translate (Switch::DataContainer::Element& o_containerElement, const Switch::Interface::Device::Value& i_value);

    void _Translate (std::list <Switch::Interface::Device::Value::DataFormat>& io_dataFormats, uint32_t& io_startByte, const Switch::DataContainer::DataFormat& i_dataFormat);
  }
}

inline
void Switch::Interface::Translate (Switch::Interface::Device::Product& o_product, const Switch::Device& i_device)
{
  const Switch::Device::Description& deviceDescription = i_device.GetDescription ();
  o_product.m_brandId         = deviceDescription.m_deviceBrandId;
  o_product.m_brandName       = deviceDescription.m_deviceBrandName;
  o_product.m_productId       = deviceDescription.m_deviceProductId;
  o_product.m_productType     = deviceDescription.m_deviceProductName;
  o_product.m_productVersion  = deviceDescription.m_deviceProductVersion;
}

inline
void Switch::Interface::Translate (Switch::Interface::Device::DataFormat& o_dataFormat, const Switch::Device& i_device)
{
  const Switch::Device::Description& deviceDescription = i_device.GetDescription ();
  const Switch::DataContainer::DataFormat& dataFormat = deviceDescription.m_dataFormat;

  uint32_t address = 0;
  _Translate (o_dataFormat.m_values, address, dataFormat);
}

inline
void Switch::Interface::Translate (Switch::Interface::Device::Value& o_value, const Switch::DataContainer::Element& i_containerElement)
{
  o_value.m_address     = i_containerElement.m_address;
  o_value.m_magicNumber = i_containerElement.m_magicNumber;
  o_value.m_value       = i_containerElement.m_value;
}

inline
void Switch::Interface::Translate (Switch::DataContainer::Element& o_containerElement, const Switch::Interface::Device::Value& i_value)
{
  o_containerElement.m_address      = i_value.m_address;
  o_containerElement.m_magicNumber  = i_value.m_magicNumber;
  o_containerElement.m_value        = i_value.m_value;
}

#endif
