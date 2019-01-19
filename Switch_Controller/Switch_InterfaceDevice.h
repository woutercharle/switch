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

#ifndef _SWITCH_INTERFACE_DEVICE
#define _SWITCH_INTERFACE_DEVICE

// project includes

// Switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"

// third-party includes
#include <string>
#include <list>


namespace Switch
{
  namespace Interface
  {
    class Device
    {
    public:

      class Product
      {
      public:

        Product ();
        ~Product ();

        uint32_t    m_brandId;
        std::string m_brandName;      ///< brand
        uint32_t    m_productId;      ///< Unique product identifier.
        std::string m_productType;    ///< type
        uint32_t    m_productVersion; ///< version
      };

      class Value
      {
      public:

        class DataFormat
        {
        public:

          DataFormat ();
          ~DataFormat ();

          uint32_t    m_address;      ///< Value address.
          uint32_t    m_magicNumber;  ///< Magic number for value adressing verification.
          std::string m_name;         ///< Value name.
          std::string m_description;  ///< Value description.
          uint32_t    m_minValue;     ///< Minimum value.
          uint32_t    m_maxValue;     ///< Maximum value.
        };

        Value ();
        ~Value ();

        uint32_t m_address;     ///< Value address.
        uint32_t m_magicNumber; ///< Magic number for value adressing verification.
        uint32_t m_value;       ///< The actual value.
      };

      class DataFormat
      {
      public:

        DataFormat ();
        ~DataFormat ();

        std::list <Value::DataFormat> m_values; // value layout
      };

      class Connection
      {
      public:

        Connection ();
        ~Connection ();

        bool  m_online; ///< Flags if the device is onliner (true) or not (false).
      };

      class Summary
      {
      public:

        Summary ();
        ~Summary ();

        uint32_t  m_deviceId;     ///< Unique device identifier.
        Product   m_productInfo;
        bool      m_online;       ///< Flags if the device is onliner (true) or not (false).
      };

      Device ();
      ~Device ();

      uint32_t    m_deviceId;       ///< Unique device identifier.
      Product     m_productInfo;    ///<
      Connection  m_connectionInfo; ///<
      DataFormat  m_dataFormat;     ///<

    protected:

    private:

    };
  }
}

#endif
