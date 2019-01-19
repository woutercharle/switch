/*?*************************************************************************
*                           Switch_InterfaceDevice.cpp
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

#include "Switch_InterfaceDevice.h"


Switch::Interface::Device::Product::Product ()
: m_brandId (0),
  m_productId (0),
  m_productVersion (0)
{
}

Switch::Interface::Device::Product::~Product ()
{
}

Switch::Interface::Device::Value::DataFormat::DataFormat ()
: m_address (0),
  m_magicNumber (0),
  m_minValue (0),
  m_maxValue (0)
{
}

Switch::Interface::Device::Value::DataFormat::~DataFormat ()
{
}

Switch::Interface::Device::Value::Value ()
: m_address (0),
  m_magicNumber (0),
  m_value (0)
{
}

Switch::Interface::Device::Value::~Value ()
{
}

Switch::Interface::Device::DataFormat::DataFormat ()
{
}

Switch::Interface::Device::DataFormat::~DataFormat ()
{
}

Switch::Interface::Device::Connection::Connection ()
: m_online (false)
{
}

Switch::Interface::Device::Connection::~Connection ()
{
}

Switch::Interface::Device::Summary::Summary ()
: m_deviceId (0),
  m_online (false)
{
}

Switch::Interface::Device::Summary::~Summary ()
{
}

Switch::Interface::Device::Device ()
: m_deviceId (0)
{
}

Switch::Interface::Device::~Device ()
{
}

