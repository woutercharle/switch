/*?*************************************************************************
*                           Switch_HttpInterfaceTraits.h
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
*    email                : wouter.charle@gmail.com
*
***************************************************************************/

#ifndef _SWITCH_HTTPINTERFACETRAITS
#define _SWITCH_HTTPINTERFACETRAITS

// Switch includes
#include <Switch_Base/Switch_CompilerConfiguration.h>
#include <Switch_API/Switch_InterfaceTypes.h>

// third-party includes
#include <cppcms/json.h>


namespace cppcms
{
  namespace json
  {
    template <>
    struct traits <Switch::Interface::eCallResult>
    {
      static Switch::Interface::eCallResult get (const value& i_value)
      {
        // get the numeric value
        int32_t enumValue = static_cast <int32_t> (i_value.number ());

        // translate to a valid enum value
        switch (enumValue)
        {
          case Switch::Interface::CR_OK:
            return Switch::Interface::CR_OK;
          case Switch::Interface::CR_STOPPED:
            return Switch::Interface::CR_STOPPED;
          case Switch::Interface::CR_UNKNOWN_DEVICE:
            return Switch::Interface::CR_UNKNOWN_DEVICE;
          case Switch::Interface::CR_CLIENT_ALREADY_REGISTERED:
            return Switch::Interface::CR_CLIENT_ALREADY_REGISTERED;
          case Switch::Interface::CR_CLIENT_NOT_REGISTERED:
            return Switch::Interface::CR_CLIENT_NOT_REGISTERED;
          case Switch::Interface::CR_CLIENT_ALREADY_SUBSCRIBED:
            return Switch::Interface::CR_CLIENT_ALREADY_SUBSCRIBED;
          case Switch::Interface::CR_CLIENT_NOT_SUBSCRIBED:
            return Switch::Interface::CR_CLIENT_NOT_SUBSCRIBED;
          case Switch::Interface::CR_CLIENT_ALREADY_CONNECTED_TO_SIGNAL:
            return Switch::Interface::CR_CLIENT_ALREADY_CONNECTED_TO_SIGNAL;
          default:
            return Switch::Interface::CR_INVALID;
        }
      }

      static void set (value& io_value, const Switch::Interface::eCallResult& i_callResult)
      {
        io_value.number (static_cast <double> (i_callResult));
      }
    };

    template <typename T>
    struct traits <std::list <T>>
    {
      static std::list<T> get (const value& i_value)
      {
        std::list <T> result;
        const json::array& jsonArray = i_value.array ();

        for (uint32_t i=0; i<jsonArray.size (); ++i)
        {
          result.push_back (jsonArray [i].get_value <T> ());
        }

        return result;
      }

      static void set (value& io_value, const std::list <T>& i_list)
      {
        io_value = json::array ();
        json::array& jsonArray = io_value.array ();
        jsonArray.resize (i_list.size ());

        typename std::list <T>::const_iterator itList = i_list.begin ();
        for (uint32_t i=0; i<i_list.size (); ++i, ++itList)
        {
          jsonArray [i].set_value (*itList);
        }
      }
    };

    template <>
    struct traits <Switch::Interface::Device::Product>
    {
      static Switch::Interface::Device::Product get (const value& i_value)
      {
        // check pre-conditions
        if (is_object != i_value.type())
        {
          throw bad_value_cast ();
        }

        // create translated object
        Switch::Interface::Device::Product outDeviceProduct;

        // fill in values
        outDeviceProduct.m_brandId        = i_value.get <uint32_t> ("brandId");
        outDeviceProduct.m_brandName      = i_value.get <std::string> ("brandName");
        outDeviceProduct.m_productId      = i_value.get <uint32_t> ("productId");
        outDeviceProduct.m_productType    = i_value.get <std::string> ("productType");
        outDeviceProduct.m_productVersion = i_value.get <uint32_t> ("productVersion");

        // return translation
        return outDeviceProduct;
      }

      static void set (value& io_value, const Switch::Interface::Device::Product& i_deviceProduct)
      {
        // fill in json value
        io_value.set ("brandId",        i_deviceProduct.m_brandId);
        io_value.set ("brandName",      i_deviceProduct.m_brandName);
        io_value.set ("productId",      i_deviceProduct.m_productId);
        io_value.set ("productType",    i_deviceProduct.m_productType);
        io_value.set ("productVersion", i_deviceProduct.m_productVersion);
      }
    };

    template <>
    struct traits <Switch::Interface::Device::Value::DataFormat>
    {
      static Switch::Interface::Device::Value::DataFormat get (const value& i_value)
      {
        // check pre-conditions
        if (is_object != i_value.type())
        {
          throw bad_value_cast ();
        }

        // create translated object
        Switch::Interface::Device::Value::DataFormat outDeviceValueDataformat;

        // fill in values
        outDeviceValueDataformat.m_address      = i_value.get <uint32_t> ("address");
        outDeviceValueDataformat.m_magicNumber  = i_value.get <uint32_t> ("magicNumber");
        outDeviceValueDataformat.m_name         = i_value.get <std::string> ("name");
        outDeviceValueDataformat.m_description  = i_value.get <std::string> ("description");
        outDeviceValueDataformat.m_minValue     = i_value.get <int32_t> ("minValue");
        outDeviceValueDataformat.m_maxValue     = i_value.get <int32_t> ("maxValue");

        // return translation
        return outDeviceValueDataformat;
      }

      static void set (value& io_value, const Switch::Interface::Device::Value::DataFormat& i_deviceValueDataformat)
      {
        // fill in json value
        io_value.set ("address",      i_deviceValueDataformat.m_address);
        io_value.set ("magicNumber",  i_deviceValueDataformat.m_magicNumber);
        io_value.set ("name",         i_deviceValueDataformat.m_name);
        io_value.set ("description",  i_deviceValueDataformat.m_description);
        io_value.set ("minValue",     i_deviceValueDataformat.m_minValue);
        io_value.set ("maxValue",     i_deviceValueDataformat.m_maxValue);
      }
    };

    template <>
    struct traits <Switch::Interface::Device::Value>
    {
      static Switch::Interface::Device::Value get (const value& i_value)
      {
        // check pre-conditions
        if (is_object != i_value.type())
        {
          throw bad_value_cast ();
        }

        // create translated object
        Switch::Interface::Device::Value outDeviceValue;

        // fill in values
        outDeviceValue.m_address     = i_value.get <uint32_t> ("address");
        outDeviceValue.m_magicNumber = i_value.get <uint32_t> ("magicNumber");
        outDeviceValue.m_value       = i_value.get <int32_t> ("value");

        // return translation
        return outDeviceValue;
      }

      static void set (value& io_value, const Switch::Interface::Device::Value& i_deviceValue)
      {
        // fill in json value
        io_value.set ("address",      i_deviceValue.m_address);
        io_value.set ("magicNumber",  i_deviceValue.m_magicNumber);
        io_value.set ("value",        i_deviceValue.m_value);
      }
    };

    template <>
    struct traits <Switch::Interface::Device::DataFormat>
    {
      static Switch::Interface::Device::DataFormat get (const value& i_value)
      {
        // check pre-conditions
        if (is_object != i_value.type())
        {
          throw bad_value_cast ();
        }

        // create translated object
        Switch::Interface::Device::DataFormat outDeviceDataformat;

        // fill in values
        outDeviceDataformat.m_values = i_value.get <std::list <Switch::Interface::Device::Value::DataFormat>> ("values");

        // return translation
        return outDeviceDataformat;
      }

      static void set (value& io_value, const Switch::Interface::Device::DataFormat& i_deviceDataformat)
      {
        // fill in json value
        io_value.set ("values", i_deviceDataformat.m_values);
      }
    };

    template <>
    struct traits <Switch::Interface::Device::Connection>
    {
      static Switch::Interface::Device::Connection get (const value& i_value)
      {
        // check pre-conditions
        if (is_object != i_value.type())
        {
          throw bad_value_cast ();
        }

        // create translated object
        Switch::Interface::Device::Connection outDeviceConnection;

        // fill in values
        outDeviceConnection.m_online = i_value.get <bool> ("online");

        // return translation
        return outDeviceConnection;
      }

      static void set (value& io_value, const Switch::Interface::Device::Connection& i_deviceConnection)
      {
        // fill in json value
        io_value.set ("online", i_deviceConnection.m_online);
      }
    };

    template <>
    struct traits <Switch::Interface::Device::Summary>
    {
      static Switch::Interface::Device::Summary get (const value& i_value)
      {
        // check pre-conditions
        if (is_object != i_value.type())
        {
          throw bad_value_cast ();
        }

        // create translated object
        Switch::Interface::Device::Summary outDeviceSummary;

        // fill in values
        outDeviceSummary.m_deviceId     = i_value.get <uint32_t> ("deviceId");
        outDeviceSummary.m_productInfo  = i_value.get <Switch::Interface::Device::Product> ("productInfo");
        outDeviceSummary.m_online       = i_value.get <bool> ("online");

        // return translation
        return outDeviceSummary;
      }

      static void set (value& io_value, const Switch::Interface::Device::Summary& i_deviceSummary)
      {
        // fill in json value
        io_value.set ("deviceId",     i_deviceSummary.m_deviceId);
        io_value.set ("productInfo",  i_deviceSummary.m_productInfo);
        io_value.set ("online",       i_deviceSummary.m_online);
      }
    };

    template <>
    struct traits <Switch::Interface::Device>
    {
      static Switch::Interface::Device get (const value& i_value)
      {
        // check pre-conditions
        if (is_object != i_value.type())
        {
          throw bad_value_cast ();
        }

        // create translated object
        Switch::Interface::Device outDevice;

        // fill in values
        outDevice.m_deviceId        = i_value.get <uint32_t> ("deviceId");
        outDevice.m_productInfo     = i_value.get <Switch::Interface::Device::Product> ("productInfo");
        outDevice.m_connectionInfo  = i_value.get <Switch::Interface::Device::Connection> ("connectionInfo");
        outDevice.m_dataFormat      = i_value.get <Switch::Interface::Device::DataFormat> ("dataFormat");

        // return translation
        return outDevice;
      }

      static void set (value& io_value, const Switch::Interface::Device& i_device)
      {
        // fill in json value
        io_value.set ("deviceId",       i_device.m_deviceId);
        io_value.set ("productInfo",    i_device.m_productInfo);
        io_value.set ("connectionInfo", i_device.m_connectionInfo);
        io_value.set ("dataFormat",     i_device.m_dataFormat);
      }
    };
  } // json
} // cppcms

#endif
