/*?*************************************************************************
*                           Switch_DeviceStore.h
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

#ifndef _SWITCH_DEVICESTORE
#define _SWITCH_DEVICESTORE

// project includes
#include "Switch_Device.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Application/Switch_ApplicationModule.h"

// third party includes
#include <map>

// forward declarations
class sqlite3;


namespace Switch
{
  // design:
  // - beheer devices in deze store
  // - gebruik devices op leen
  // - alle persistente data van de devices worden door de store direct naar de database geschreven
  // maar:
  // - hoe omgaan met concurrency
  // - aanpassen eigenschappen
  class DeviceStore : public Switch::ApplicationModule
  {
  public:

    typedef std::map <switch_device_address_type, Switch::Device> DeviceMap;

    /*!
      \brief Parameters container class
     */
    class Parameters : public Switch::ApplicationModule::Parameters
    {
    public:

      /*!
        \brief Default constructor.
       */
      explicit Parameters ();
      /*!
        \brief Destructor.
       */
      virtual ~Parameters ();

      // using default copy constructor and assignment operator
      Parameters (const Parameters& i_other) = default;
      Parameters& operator= (const Parameters& i_other) = default;

      // parameter members
      std::string m_databaseName;             ///< Name of the database to use as device store backend.
      std::string m_resourceDirectory;        ///< Directory containing resources for the device store.
      uint32_t    m_maxNrUnidentifiedDevices; ///< The maximum number of unidentified devices in the device store.
    };

    explicit DeviceStore ();
    explicit DeviceStore (const Parameters& i_parameters);
    virtual ~DeviceStore ();

    // copy constructor and assignment operator are disabled
    DeviceStore (const DeviceStore& i_other) = delete;
    DeviceStore& operator= (const DeviceStore& i_other) = delete;

    void            AddDevice     (const switch_device_address_type& i_deviceAddress);
    Switch::Device& SetDeviceType (const switch_device_address_type& i_deviceAddress, const switch_brand_id_type& i_brandId, const switch_product_id_type& i_productId, const switch_product_version_type& i_version);
    DeviceMap&      GetDevices    ();
    DeviceMap&      GetUnidentfiedDevices ();

  protected:

    void _SetupParameterContainer ();
    virtual void _SetParameters (const Switch::ParameterizedObject::Parameters& i_parameters);
    virtual void _GetParameters (Switch::ParameterizedObject::Parameters& o_parameters) const;

    virtual void _Prepare ();
    virtual void _Start   ();
    virtual void _Pause   ();
    virtual void _Stop    ();

  private:

    // utility methods
    // > database
    void      _UpdateDatabaseLayout       ();
    void      _WriteDatabaseDevice        (const switch_device_address_type& i_deviceAddress);
    void      _UpdateDatabaseDevice       (const switch_device_address_type& i_deviceAddress, const uint64_t& i_productKey);
    uint64_t  _ReadDatabaseProductKey     (const switch_brand_id_type& i_brandId, const switch_product_id_type& i_productId, const switch_product_version_type& i_version);
    void      _ReadDatabaseProducts       ();
    int       _OnDatabaseProductRead      (const int& i_nrColumns, char const* const* i_ppRowValues, char const* const* i_ppColumnNames);
    void      _ReadDatabaseDevices        ();
    int       _OnDatabaseDeviceRead       (const int& i_nrColumns, char const* const* i_ppRowValues, char const* const* i_ppColumnNames);
    // > product description files
    void      _LoadProductDescription (Switch::Device::Description& o_description, const switch_brand_id_type& i_brandId, const switch_product_id_type& i_productId, const switch_product_version_type& i_version);

    // variables
    sqlite3* m_pDatabaseConnection;

    // data members
    DeviceMap                                         m_unidentifiedDeviceMap;  ///< Map of all devices whose type is still unknown (maps address to device object).
    DeviceMap                                         m_deviceMap;              ///< Map of all devices (maps address to device object).
    std::map <uint64_t, Switch::Device::Description>  m_productDescriptionMap;  ///< Map of all known product descriptions (database key to description).

    // parameters
    std::string m_databaseName;             ///< Name of the database to use as device store backend.
    std::string m_resourceDirectory;        ///< Directory containing resources for the device store.
    uint32_t    m_maxNrUnidentifiedDevices; ///< The maximum number of unidentified devices in the device store.
  };
}

#endif // _SWITCH_DEVICESTORE
