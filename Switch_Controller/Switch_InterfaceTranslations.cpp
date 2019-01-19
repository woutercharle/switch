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

// project includes
#include "Switch_InterfaceTranslations.h"


void Switch::Interface::_Translate (std::list <Switch::Interface::Device::Value::DataFormat>& io_dataFormats, uint32_t& io_address, const Switch::DataContainer::DataFormat& i_dataFormat)
{
  std::list <Switch::DataAdapter::DataFormat>::const_iterator itAdapter;
  for (itAdapter = i_dataFormat.m_nestedAdapters.begin (); i_dataFormat.m_nestedAdapters.end () != itAdapter; ++itAdapter)
  {
    const Switch::DataAdapter::DataFormat& dataFormat = *itAdapter;
    Switch::Interface::Device::Value::DataFormat valueDataFormat;

    valueDataFormat.m_name        = dataFormat.m_name;
    valueDataFormat.m_description = dataFormat.m_description;
    valueDataFormat.m_address     = io_address;
    valueDataFormat.m_magicNumber = dataFormat.ComputeMagicNumber (io_address);
    valueDataFormat.m_minValue    = dataFormat.m_minValue;
    valueDataFormat.m_maxValue    = dataFormat.m_maxValue;

    io_dataFormats.push_back (valueDataFormat);

    io_address += dataFormat.m_bitSize;
  }

  std::list <Switch::DataContainer::DataFormat>::const_iterator itContainer;
  for (itContainer = i_dataFormat.m_nestedContainers.begin (); i_dataFormat.m_nestedContainers.end () != itContainer; ++itContainer)
  {
    const Switch::DataContainer::DataFormat& dataFormat = *itContainer;
    _Translate (io_dataFormats, io_address, dataFormat);
  }
}
