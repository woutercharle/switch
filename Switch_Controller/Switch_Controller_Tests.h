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

#ifndef _SWITCH_CONTROLLER_TESTS
#define _SWITCH_CONTROLLER_TESTS

#define _GLIBCXX_USE_NANOSLEEP

// project includes
#include "Switch_Controller.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"
#include "../Switch_Base/Switch_Debug.h"
#include "../Switch_Serialization/Switch_JsonSerializer.h"

// third party includes
#include <string>
#include <fstream>
#include <thread>
#include <SSVUtils/Core/FileSystem/FileSystem.hpp>

namespace Switch
{
  namespace ControllerTests
  {
    void Cleanup ();
    void Run ();
    void TestParameters ();
    void TestStates ();
    void TestFunctional ();
  }
}

void Switch::ControllerTests::TestParameters ()
{
  if (!ssvu::FileSystem::exists ("./resources/"))
  {
    ssvu::FileSystem::createFolder ("./resources/");
  }

  Switch::Controller controller;

  Switch::ParameterContainer parameterContainer;
  controller.GetParameters (parameterContainer);

  // write to json file
  std::ofstream jsonOutStream ("./resources/parameters.spf");
  Switch::JsonSerializer::Serialize (jsonOutStream, parameterContainer);
  jsonOutStream.close ();

  // read from json file
  std::ifstream jsonInStream ("./resources/parameters.spf");
  Switch::ParameterContainer inputParameterContainer;
  Switch::JsonSerializer::Deserialize (inputParameterContainer, jsonInStream);
  jsonInStream.close ();

  //SWITCH_ASSERT (inputParameterContainer == parameterContainer);
}

void Switch::ControllerTests::TestStates ()
{
  std::chrono::milliseconds sleepDuration (5000);

  Switch::Controller controller;

  // read parameters from json file
  std::ifstream jsonInStream ("./resources/parameters.spf");
  Switch::ParameterContainer parameterContainer;
  Switch::JsonSerializer::Deserialize (parameterContainer, jsonInStream);
  jsonInStream.close ();

  controller.SetParameters (parameterContainer);

  controller.Prepare ();
  controller.Stop ();

  controller.Prepare ();
  controller.Start ();
  controller.Pause ();
  controller.Stop ();

  controller.Prepare ();
  controller.Start ();
  controller.Pause ();
  controller.Start ();
  controller.Pause ();
  controller.Stop ();

  /*controller.Prepare ();
  std::this_thread::sleep_for (sleepDuration);
  controller.Start ();
  std::this_thread::sleep_for (sleepDuration);
  controller.Pause ();
  std::this_thread::sleep_for (sleepDuration);
  controller.Stop ();*/
}

void Switch::ControllerTests::TestFunctional ()
{
  if (!ssvu::FileSystem::exists ("./resources/"))
  {
    ssvu::FileSystem::createFolder ("./resources/");
  }

  Switch::Controller controller;

  // create the device description file
  Switch::Device::Description deviceDescription;
  deviceDescription.m_deviceBrandId         = 1;
  deviceDescription.m_deviceBrandName       = "Switch";
  deviceDescription.m_deviceProductId       = 1;
  deviceDescription.m_deviceProductName     = "Dimmer";
  deviceDescription.m_deviceProductVersion  = 1;

  Switch::DataContainer::DataFormat& dataFormat = deviceDescription.m_dataFormat;
  dataFormat.m_name = "SwitchDimmer";
  dataFormat.m_description = "Dataformat of the Switch Dimmer v1";

  Switch::DataAdapter::DataFormat valueDataFormat;
  valueDataFormat.m_name         = "dimLevel";
  valueDataFormat.m_description  = "Dimming level";
  valueDataFormat.m_group        = "Output";
  valueDataFormat.m_bitOffset    = 0;
  valueDataFormat.m_bitSize      = 8;
  valueDataFormat.m_minValue     = 0;
  valueDataFormat.m_maxValue     = 255;
  dataFormat.m_nestedAdapters.push_back (valueDataFormat);

  // write to json file
  {
    std::ofstream jsonOutStream ("./resources/product_1_1_1.spd");
    Switch::JsonSerializer::Serialize (jsonOutStream, deviceDescription);
    jsonOutStream.close ();
  }

  // read parameters from json file
  {
    Switch::ParameterContainer parameterContainer;
    if (!ssvu::FileSystem::exists ("./resources/parameters.spf"))
    {
      // write to json file
      controller.GetParameters (parameterContainer);
      std::ofstream jsonOutStream ("./resources/parameters.spf");
      Switch::JsonSerializer::Serialize (jsonOutStream, parameterContainer);
      jsonOutStream.close ();
    }
    std::ifstream jsonInStream ("./resources/parameters.spf");
    Switch::JsonSerializer::Deserialize (parameterContainer, jsonInStream);
    jsonInStream.close ();

    controller.SetParameters (parameterContainer);
  }

  controller.Prepare ();
  controller.Start ();

  /*std::cout << "Press enter to continue ... ";
  std::cin.ignore().get();*/

  controller.AddDevice (0xFF55DE23);

  /*std::cout << "Press enter to continue ... ";
  std::cin.ignore().get();

  Switch::Interface::Device deviceDetails;
  controller.GetDeviceDetails (deviceDetails, 0xFF55DE23);

  printf ("device id: 0x%08x\n", deviceDetails.m_deviceId);
  printf ("brand id: %u\n", deviceDetails.m_productInfo.m_brandId);
  printf ("brand: %s\n", deviceDetails.m_productInfo.m_brandName.c_str ());
  printf ("product id: %u\n", deviceDetails.m_productInfo.m_productId);
  printf ("product: %s\n", deviceDetails.m_productInfo.m_productType.c_str ());
  printf ("product version: %u\n", deviceDetails.m_productInfo.m_productVersion);
  printf ("connection: %u\n", deviceDetails.m_connectionInfo.m_online);
  std::list <Switch::Interface::Device::Value::DataFormat>::iterator itValues;
  for (itValues = deviceDetails.m_dataFormat.m_values.begin (); deviceDetails.m_dataFormat.m_values.end () != itValues; ++itValues)
  {
    printf ("value name: %s\n", (*itValues).m_name.c_str ());
    printf ("value description: %s\n", (*itValues).m_description.c_str ());
    printf ("value min: %u\n", (*itValues).m_minValue);
    printf ("value max: %u\n", (*itValues).m_maxValue);
  }*/

  std::cout << "Press enter to continue ... ";
  std::cin.ignore().get();

  std::list <Switch::Interface::Device::Value> values;
  controller.GetDeviceValues (values, 0xFF55DE23);


  int32_t input = 0;
  while (-1 != input)
  {
    std::cout << "Give dimmer value [0 - 255] or -1 to stop:" << std::endl
                << " > ";
    std::cin >> input;

    if ((-1 != input) && (256 > input))
    {
      values.front ().m_value = static_cast <uint32_t> (input);
      controller.SetDeviceValues (0xFF55DE23, values);
    }
  }

  controller.Pause ();
  controller.Stop ();
}

void Switch::ControllerTests::Cleanup ()
{
  if (ssvu::FileSystem::exists ("./switch.db"))
  {
    ssvu::FileSystem::removeFile ("./switch.db");
  }
  if (ssvu::FileSystem::exists ("./resources/parameters.spf"))
  {
    ssvu::FileSystem::removeFile ("./resources/parameters.spf");
  }
}

void Switch::ControllerTests::Run ()
{
//#ifdef _DEBUG

  std::thread::id threadId = std::this_thread::get_id ();
  std::cout << "Starting tests with thread Id " << threadId << std::endl;

  Cleanup ();

  try
  {
    /*TestParameters ();

    std::cout << "Press enter to continue ... ";
    std::cin.ignore().get();

    TestStates ();

    std::cout << "Press enter to continue ... ";
    std::cin.ignore().get();*/

    TestFunctional ();
  }
  catch (const std::exception& i_exception)
  {
    std::cout << "Uncaught excpetion: " << i_exception.what () << std::endl;
  }

//#endif
}

#endif // _SWITCH_CONTROLLER_TESTS






