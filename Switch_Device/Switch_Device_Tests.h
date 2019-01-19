/*?*************************************************************************
*                           Switch_Device_Test.h
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

#ifndef _SWITCH_DEVICE_TESTS
#define _SWITCH_DEVICE_TESTS

// project includes
#include "Switch_Device.h"
#include "Switch_DeviceStore.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"
#include "../Switch_Base/Switch_Debug.h"
#include "../Switch_Serialization/Switch_JsonSerializer.h"

// third party includes
#include <thread>
#include <string>
#include <fstream>
#include <SSVUtils/Core/FileSystem/FileSystem.hpp>


namespace Switch
{
  namespace DeviceTests
  {
    void Run ();
    void TestDataAdapter ();
    void TestDataContainer ();
    void TestDeviceDescription ();
    void TestDeviceStore ();
  }
}

void Switch::DeviceTests::TestDataAdapter ()
{
#ifdef _DEBUG

  std::cout << ">>>>>>>>> Test Switch::DataAdapter >>>>>>>>>" << std::endl;

  Switch::DataAdapter::DataFormat adapterDataFormat;
  Switch::DataAdapter::DataFormat adapterDataFormat_c1 ("my name", "my description", 1, 2);
  Switch::DataAdapter::DataFormat adapterDataFormat_c2 ("my name", "my description", "my group", 1, 2, 1, 3);
  adapterDataFormat_c1 = adapterDataFormat_c2;
  SWITCH_ASSERT (adapterDataFormat_c1 == adapterDataFormat_c2);
  adapterDataFormat = adapterDataFormat_c1;
  SWITCH_ASSERT (adapterDataFormat_c1 == adapterDataFormat);

  SWITCH_ASSERT (adapterDataFormat.m_name         == "my name");
  SWITCH_ASSERT (adapterDataFormat.m_description  == "my description");
  SWITCH_ASSERT (adapterDataFormat.m_group        == "my group");
  SWITCH_ASSERT (adapterDataFormat.m_bitOffset    == 1);
  SWITCH_ASSERT (adapterDataFormat.m_bitSize      == 2);
  SWITCH_ASSERT (adapterDataFormat.m_minValue     == 1);
  SWITCH_ASSERT (adapterDataFormat.m_maxValue     == 3);

  SWITCH_ASSERT (adapterDataFormat.ValidateValue (1));
  SWITCH_ASSERT (adapterDataFormat.ValidateValue (2));
  SWITCH_ASSERT (adapterDataFormat.ValidateValue (3));
  SWITCH_ASSERT (!adapterDataFormat.ValidateValue (-2));
  SWITCH_ASSERT (!adapterDataFormat.ValidateValue (-1));
  SWITCH_ASSERT (!adapterDataFormat.ValidateValue ( 0));
  SWITCH_ASSERT (!adapterDataFormat.ValidateValue ( 4));
  SWITCH_ASSERT (!adapterDataFormat.ValidateValue ( 5));

  Switch::DataAdapter adapter (adapterDataFormat);
  SWITCH_ASSERT (&adapterDataFormat == &adapter.GetDataFormat ());

  int32_t value;
  SWITCH_ASSERT (!adapter.SetDataValue (0));
  SWITCH_ASSERT (!adapter.SetDataValue (4));
  adapter.GetDataValue (value);
  SWITCH_ASSERT (1 == value);
  SWITCH_ASSERT (adapter.SetDataValue (2));
  adapter.GetDataValue (value);
  SWITCH_ASSERT (2 == value);
  SWITCH_ASSERT (adapter.SetDataValue (3));
  adapter.GetDataValue (value);
  SWITCH_ASSERT (3 == value);

  uint32_t one   = 0x00000000; // big-endian: 0 00 0 0000 0000 0000 0000 0000 0000 0000
  uint32_t two   = 0x00000020; // big-endian: 0 01 0 0000 0000 0000 0000 0000 0000 0000
  uint32_t three = 0x00000040; // big-endian: 0 10 0 0000 0000 0000 0000 0000 0000 0000
  uint32_t four  = 0x00000060; // big-endian: 0 11 0 0000 0000 0000 0000 0000 0000 0000
  SWITCH_ASSERT (!adapter.SetDataValue (reinterpret_cast <uint8_t*> (&four)));
  SWITCH_ASSERT (adapter.SetDataValue (reinterpret_cast <uint8_t*> (&one)));
  adapter.GetDataValue (value);
  SWITCH_ASSERT (1 == value);
  SWITCH_ASSERT (adapter.SetDataValue (reinterpret_cast <uint8_t*> (&two)));
  adapter.GetDataValue (value);
  SWITCH_ASSERT (2 == value);
  SWITCH_ASSERT (adapter.SetDataValue (reinterpret_cast <uint8_t*> (&three)));
  adapter.GetDataValue (value);
  SWITCH_ASSERT (3 == value);

  uint8_t buffer [4];
  buffer [0] = 0xFF; // 11 11 1111
  buffer [1] = 0xFF; // 11 01 1111
  buffer [2] = 0x00; // 11 10 1111
  buffer [3] = 0x00; // 11 00 1111
  SWITCH_ASSERT (!adapter.SetDataValue (buffer));

  std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

#endif
}

void Switch::DeviceTests::TestDataContainer ()
{
#ifdef _DEBUG

  std::cout << ">>>>>>>> Test Switch::DataContainer >>>>>>>>" << std::endl;

  Switch::DataContainer::DataFormat containerDataFormat;
  Switch::DataContainer::DataFormat containerDataFormat_c1 ("my name", "my description");
  containerDataFormat = containerDataFormat_c1;

  SWITCH_ASSERT (containerDataFormat.m_name        == "my name");
  SWITCH_ASSERT (containerDataFormat.m_description == "my description");

  containerDataFormat.m_nestedAdapters.push_back (Switch::DataAdapter::DataFormat ("ad0", "ad0d", "group", 0, 2, 1, 3));
  containerDataFormat.m_nestedAdapters.push_back (Switch::DataAdapter::DataFormat ("ad1", "ad1d", "group", 2, 7, 1, 64));

  containerDataFormat.m_nestedContainers.push_back (Switch::DataContainer::DataFormat ("container", "cont descrip"));
  Switch::DataContainer::DataFormat& nestedContainerDataFormat = containerDataFormat.m_nestedContainers.back ();

  nestedContainerDataFormat.m_nestedAdapters.push_back (Switch::DataAdapter::DataFormat ("ad2", "ad2d", "group", 1, 3, 6, 7));
  nestedContainerDataFormat.m_nestedAdapters.push_back (Switch::DataAdapter::DataFormat ("ad3", "ad3d", "group", 4, 4, 1, 7));

  SWITCH_ASSERT ( 7 == nestedContainerDataFormat.GetTotalBitSize ());
  SWITCH_ASSERT (16 == containerDataFormat.GetTotalBitSize ());

  Switch::DataContainer container (containerDataFormat);
  SWITCH_ASSERT (&containerDataFormat == &container.GetDataFormat ());

  std::list <Switch::DataContainer::Element> changedElements;
  uint16_t content;
  content = 0x841F; // 1, 64, 6, 5
  SWITCH_ASSERT (container.SetContent (changedElements, reinterpret_cast <uint8_t*> (&content)));
  container.GetContent (reinterpret_cast <uint8_t*> (&content));
  SWITCH_ASSERT (content == 0x841F);
  SWITCH_ASSERT (4 == changedElements.size ());
  changedElements.clear ();
  content = 0x0420; // 1, 65, 6, 5
  SWITCH_ASSERT (!container.SetContent (changedElements, reinterpret_cast <uint8_t*> (&content)));
  container.GetContent (reinterpret_cast <uint8_t*> (&content));
  SWITCH_ASSERT (content == 0x841F);
  SWITCH_ASSERT (1 == changedElements.size ());
  changedElements.clear ();

  std::list <Switch::DataContainer::Element> containerElements;
  container.GetElements (containerElements);
  SWITCH_ASSERT (4 == containerElements.size ());
  std::list <Switch::DataContainer::Element>::iterator itElement;
  for (itElement = containerElements.begin (); containerElements.end () != itElement; ++itElement)
  {
    Switch::DataContainer::Element& element = *itElement;
    if (element.m_address == 0)
    {
      SWITCH_ASSERT (element.m_address == 0);
      SWITCH_ASSERT (element.m_value == 1);

      element.m_value = 2;
    }
    else if (element.m_address == 2)
    {
      SWITCH_ASSERT (element.m_address == 2);
      SWITCH_ASSERT (element.m_value == 64);

      element.m_value = 63;
    }
    else if (element.m_address == 9)
    {
      SWITCH_ASSERT (element.m_address == 9);
      SWITCH_ASSERT (element.m_value == 6);

      element.m_value = 7;
    }
    else if (element.m_address == 12)
    {
      SWITCH_ASSERT (element.m_address == 12);
      SWITCH_ASSERT (element.m_value == 5);

      element.m_value = 7;
    }
  }

  SWITCH_ASSERT (container.SetElements (changedElements, containerElements));
  SWITCH_ASSERT (4 == changedElements.size ());
  changedElements.clear ();
  container.GetContent (reinterpret_cast <uint8_t*> (&content));
  SWITCH_ASSERT (content == 0x165F);

  std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

#endif
}

void Switch::DeviceTests::TestDeviceDescription ()
{
#ifdef _DEBUG

  std::cout << ">>>>> Test Switch::Device::Description >>>>>" << std::endl;

  if (!ssvu::FileSystem::exists ("./resources/"))
  {
    ssvu::FileSystem::createFolder ("./resources/");
  }
  Switch::Device::Description deviceDescription;

  deviceDescription.m_deviceBrandName       = "Switch";
  deviceDescription.m_deviceProductId       = 1;
  deviceDescription.m_deviceProductName     = "TestDevice";
  deviceDescription.m_deviceProductVersion  = 1;

  Switch::DataContainer::DataFormat& dataFormat = deviceDescription.m_dataFormat;
  dataFormat.m_name = "TestDataFormat";
  dataFormat.m_description = "Description of the data format of test device";

  Switch::DataAdapter::DataFormat nestedDataFormat;
  nestedDataFormat.m_name = "Val0";
  nestedDataFormat.m_description = "Val0 description";
  nestedDataFormat.m_group = "Values";
  nestedDataFormat.m_bitOffset = 0;
  nestedDataFormat.m_bitSize = 4;
  nestedDataFormat.m_minValue = 1;
  nestedDataFormat.m_maxValue = 8;
  dataFormat.m_nestedAdapters.push_back (nestedDataFormat);
  nestedDataFormat.m_name = "Val1";
  nestedDataFormat.m_description = "Val1 description";
  nestedDataFormat.m_group = "Values";
  nestedDataFormat.m_bitOffset = 4;
  nestedDataFormat.m_bitSize = 6;
  nestedDataFormat.m_minValue = 1;
  nestedDataFormat.m_maxValue = 32;
  dataFormat.m_nestedAdapters.push_back (nestedDataFormat);

  Switch::DataContainer::DataFormat nestedContainer;
  nestedContainer.m_name = "TestNestedDataFormat";
  nestedContainer.m_description = "Description of the data format of the nested container in the test device";

  nestedDataFormat.m_name = "Val0";
  nestedDataFormat.m_description = "Val0 description";
  nestedDataFormat.m_group = "Values";
  nestedDataFormat.m_bitOffset = 2;
  nestedDataFormat.m_bitSize = 14;
  nestedDataFormat.m_minValue = 1;
  nestedDataFormat.m_maxValue = 16383;
  nestedContainer.m_nestedAdapters.push_back (nestedDataFormat);

  dataFormat.m_nestedContainers.push_back (nestedContainer);

  // write to json file
  std::ofstream jsonOutStream ("./resources/product_1_1_1.spd");
  Switch::JsonSerializer::Serialize (jsonOutStream, deviceDescription);
  jsonOutStream.close ();

  // read from json file
  std::ifstream jsonInStream ("./resources/product_1_1_1.spd");
  Switch::Device::Description inputDeviceDescription;
  Switch::JsonSerializer::Deserialize (inputDeviceDescription, jsonInStream);
  jsonInStream.close ();

  SWITCH_ASSERT (inputDeviceDescription == deviceDescription);

  std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

#endif
}

void Switch::DeviceTests::TestDeviceStore ()
{
#ifdef _DEBUG

  std::cout << ">>>>>>>>> Test Switch::DeviceStore >>>>>>>>>" << std::endl;

  Switch::DeviceStore::Parameters parameters;
  parameters.m_databaseName = "./resources/switch.db";
  parameters.m_resourceDirectory = "./resources/";
  parameters.m_maxNrUnidentifiedDevices = 1;

  Switch::DeviceStore deviceStore;
  deviceStore.SetParameters (parameters);

  try
  {
    std::cout << "********************************************" << std::endl;

    deviceStore.Prepare ();
    deviceStore.Start ();

    deviceStore.AddDevice (101010101);
    try
    {
      deviceStore.AddDevice (101010101);
      SWITCH_ASSERT (true == false);
    }
    catch (const std::exception& i_exception)
    {
      SWITCH_DEBUG_MSG_1 ("Exception caught: %s\n\r", i_exception.what ());
    }
    try
    {
      deviceStore.AddDevice (1010101010);
      SWITCH_ASSERT (true == false);
    }
    catch (const std::exception& i_exception)
    {
      SWITCH_DEBUG_MSG_1 ("Exception caught: %s\n\r", i_exception.what ());
    }

    Switch::DeviceStore::DeviceMap& devices = deviceStore.GetDevices ();
    SWITCH_ASSERT (devices.empty ());

    deviceStore.Pause ();
    deviceStore.Stop ();
  }
  catch (const std::exception& i_exception)
  {
    std::cout << "Exception caught: " << i_exception.what () << std::endl;

    deviceStore.Pause ();
    deviceStore.Stop ();
  }

  try
  {
    std::cout << "********************************************" << std::endl;

    deviceStore.Prepare ();
    deviceStore.Start ();

    try
    {
      deviceStore.AddDevice (101010101);
      SWITCH_ASSERT (true == false);
    }
    catch (const std::exception& i_exception)
    {
      SWITCH_DEBUG_MSG_1 ("Exception caught: %s\n\r", i_exception.what ());
    }
    try
    {
      deviceStore.AddDevice (1010101010);
      SWITCH_ASSERT (true == false);
    }
    catch (const std::exception& i_exception)
    {
      SWITCH_DEBUG_MSG_1 ("Exception caught: %s\n\r", i_exception.what ());
    }

    deviceStore.SetDeviceType (101010101, 1, 1, 1);
    Switch::DeviceStore::DeviceMap& devices = deviceStore.GetDevices ();
    SWITCH_ASSERT (1 == devices.size ());

    deviceStore.Pause ();

    std::cout << "stopping" << std::endl;

    deviceStore.Stop ();

    std::cout << "leaving scope" << std::endl;
  }
  catch (const std::exception& i_exception)
  {
    std::cout << "Exception caught: " << i_exception.what () << std::endl;

    deviceStore.Pause ();
    deviceStore.Stop ();
  }

  try
  {
    std::cout << "********************************************" << std::endl;

    deviceStore.Prepare ();
    deviceStore.Start ();

    try
    {
      deviceStore.AddDevice (101010101);
      SWITCH_ASSERT (true == false);
    }
    catch (const std::exception& i_exception)
    {
      SWITCH_DEBUG_MSG_1 ("Exception caught: %s\n\r", i_exception.what ());
    }
    try
    {
      deviceStore.SetDeviceType (101010101, 1, 1, 1);
      SWITCH_ASSERT (true == false);
    }
    catch (const std::exception& i_exception)
    {
      SWITCH_DEBUG_MSG_1 ("Exception caught: %s\n\r", i_exception.what ());
    }

    Switch::DeviceStore::DeviceMap& devices = deviceStore.GetDevices ();
    SWITCH_ASSERT (1 == devices.size ());

    deviceStore.AddDevice (1010101010);

    deviceStore.Pause ();
    deviceStore.Stop ();
  }
  catch (const std::exception& i_exception)
  {
    std::cout << "Exception caught: " << i_exception.what () << std::endl;

    deviceStore.Pause ();
    deviceStore.Stop ();
  }

  try
  {
    std::cout << "********************************************" << std::endl;

    deviceStore.Prepare ();
    deviceStore.Start ();

    Switch::DeviceStore::DeviceMap& devices = deviceStore.GetDevices ();
    SWITCH_ASSERT (1 == devices.size ());

    Switch::Device& device = devices.begin ()->second;
    const Switch::Device::Description& deviceDescription = device.GetDescription ();
    Switch::DataContainer& deviceDataContainer = device.GetDataContainer ();

    // test the content of the device description
    SWITCH_ASSERT ("Switch" == deviceDescription.m_deviceBrandName);
    SWITCH_ASSERT (1 == deviceDescription.m_deviceProductId);
    SWITCH_ASSERT (1 == deviceDescription.m_deviceProductVersion);
    SWITCH_ASSERT ("TestDevice" == deviceDescription.m_deviceProductName);
    SWITCH_ASSERT ("TestDataFormat" == deviceDescription.m_dataFormat.m_name);
    SWITCH_ASSERT ("Description of the data format of test device" == deviceDescription.m_dataFormat.m_description);
    SWITCH_ASSERT (2 == deviceDescription.m_dataFormat.m_nestedAdapters.size ());
    SWITCH_ASSERT (1 == deviceDescription.m_dataFormat.m_nestedContainers.size ());

    // test the data container
    std::list <Switch::DataContainer::Element> containerElements;
    deviceDataContainer.GetElements (containerElements);
    SWITCH_ASSERT (3 == containerElements.size ());

    deviceStore.Pause ();
    deviceStore.Stop ();
  }
  catch (const std::exception& i_exception)
  {
    std::cout << "Exception caught: " << i_exception.what () << std::endl;

    deviceStore.Pause ();
    deviceStore.Stop ();
  }

  std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

#endif
}

void Switch::DeviceTests::Run ()
{
#ifdef _DEBUG

  std::thread::id threadId = std::this_thread::get_id ();
  std::cout << "Starting tests with thread Id " << threadId << std::endl;

  try
  {
    // 1. Test the data adapter class
    TestDataAdapter ();

    // 2. Test the data container class
    TestDataContainer ();

    // 3. Test the device description class
    TestDeviceDescription ();

    // 4. Test the device store class
    TestDeviceStore ();
  }
  catch (const std::exception& i_exception)
  {
    std::cout << "Uncaught excpetion: " << i_exception.what () << std::endl;
  }

#endif
}

#endif // _SWITCH_DEVICE_TESTS

