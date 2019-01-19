/*?*************************************************************************
*                           Switch_Http_Test.h
*                           -----------------------
*    copyright            : (C) 2014 by Wouter Charle
*    email                : wouter.charle@gmail.com
*
***************************************************************************/

#ifndef _SWITCH_HTTP_TESTS
#define _SWITCH_HTTP_TESTS

// project includes
#include "Switch_Http/Switch_HttpInterface.h"

// switch includes
#include <Switch_Base/Switch_CompilerConfiguration.h>
#include <Switch_Base/Switch_Types.h>
#include <Switch_Base/Switch_Debug.h>
#include <Switch_API/Switch_FunctionalFacade.h>
#include <Switch_API/Switch_FunctionalInterfaceDummyImpl.h>
#include <Switch_Device/Switch_Device.h>
#include <Switch_Controller/Switch_Controller.h>
#include <Switch_Serialization/Switch_JsonSerializer.h>

// third party includes
#include <thread>
#include <SSVUtils/Core/FileSystem/FileSystem.hpp>
#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>


namespace Switch
{
  namespace HttpTests
  {
    std::mutex g_stdCoutMutex;

    void Run ();
    void TestFunctional ();
    void TestNoInterProcess ();
  }
}

void Switch::HttpTests::TestFunctional ()
{
  int   nrArguments = 3;
  char* arguments [3] = {"./", "-c", "./config.js"};

  Switch::FunctionalInterfaceDummyImpl controller;

  // create the http interface
  Switch::FunctionalFacade controllerFacade (controller);
  cppcms::service httpService (nrArguments, arguments);

  httpService.applications_pool ().mount (cppcms::applications_factory <Switch::HttpInterface, Switch::FunctionalFacade> (controllerFacade));
  httpService.run ();
}

void Switch::HttpTests::TestNoInterProcess ()
{
  int   nrArguments = 3;
  char* arguments [3] = {"./", "-c", "./config.js"};

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

  // create the http interface
  Switch::FunctionalFacade controllerFacade (controller);
  cppcms::service httpService (nrArguments, arguments);

  httpService.applications_pool ().mount (cppcms::applications_factory <Switch::HttpInterface, Switch::FunctionalFacade> (controllerFacade));
  httpService.run ();
}

void Switch::HttpTests::Run ()
{
//#ifdef _DEBUG

  std::thread::id threadId = std::this_thread::get_id ();
  std::cout << "Starting tests with thread Id " << threadId << std::endl;

  try
  {
    /*char input = '\0';
    while (true)
    {
      {
        std::unique_lock <std::mutex> stdCoutLock (g_stdCoutMutex);
        std::cout << "choose test type:" << std::endl
                  << "0. dummy controller" << std::endl
                  << "1. no interprocess" << std::endl
                  << "> ";

        std::cin.get (input);
      }

      if (('0' == input) || ('1' == input))
      {
        break;
      }

      {
        std::unique_lock <std::mutex> stdCoutLock (g_stdCoutMutex);
        std::cout << "invalid choice" << std::endl << std::endl;
      }
    }

    if ('0' == input)
    {
      TestFunctional ();
    }
    else if ('1' == input)
    {
      TestNoInterProcess ();
    }*/

    TestNoInterProcess ();
  }
  catch (const std::exception& i_exception)
  {
    std::unique_lock <std::mutex> stdCoutLock (g_stdCoutMutex);
    std::cout << "Uncaught excpetion: " << i_exception.what () << std::endl;
  }
  catch (...)
  {
    std::unique_lock <std::mutex> stdCoutLock (g_stdCoutMutex);
    std::cout << "Uncaught excpetion: unknown-type" << std::endl;
  }
//#endif
}


#endif // _SWITCH_CONTROLLER_TESTS






