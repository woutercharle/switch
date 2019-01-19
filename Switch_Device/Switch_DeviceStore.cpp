/*?*************************************************************************
*                           Switch_DeviceStore.cpp
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

#include "Switch_DeviceStore.h"

// switch includes
#include "../Switch_Base/Switch_StdLibExtras.h"
#include "../Switch_Serialization/Switch_JsonSerializer.h"

// third-party includes
#include <thread>
#include <sqlite3.h>
#include <cstdio>
#include <exception>
#include <SSVUtils/Core/FileSystem/FileSystem.hpp>

using namespace std::placeholders;

namespace Switch
{
  int _HandleDatabaseProductRead  (void* i_pEndPoint, int i_nrColumns, char** i_ppRowValues, char** i_ppColumnNames);
  int _HandleDatabaseDeviceRead   (void* i_pEndPoint, int i_nrColumns, char** i_ppRowValues, char** i_ppColumnNames);
}

/*!
  \brief Default constructor
 */
Switch::DeviceStore::Parameters::Parameters ()
{
  m_databaseName              = "switch.db";
  m_resourceDirectory         = "./resources/";
  m_maxNrUnidentifiedDevices  = 99;
}

/*!
  \brief Destructor
 */
Switch::DeviceStore::Parameters::~Parameters ()
{
}

void Switch::DeviceStore::_SetupParameterContainer ()
{
  // set container properties
  _SetContainerName ("Device Store");
  _SetContainerDescription ("Parameters of the device store.");

  // add parameters
  Parameters myParameters;
  _AddParameter (myParameters, myParameters.m_databaseName,             "Database name", "Name of the database to use as device store backend.", "Backend");
  _AddParameter (myParameters, myParameters.m_resourceDirectory,        "Resource directory", "Directory containing resources for the device store.", "Backend");
  _AddParameter (myParameters, myParameters.m_maxNrUnidentifiedDevices, "Max. nr. unknown devices", "The maximum number of unidentified devices in the device store.", "Storage");
  // note: add validation criterium to parameter

  // add sub-module parameters

  // override parameters

}

void Switch::DeviceStore::_SetParameters (const Switch::ParameterizedObject::Parameters& i_parameters)
{
  SWITCH_DEBUG_MSG_0 ("Switch::DeviceStore::SetParameters ... ");

  // push down to super
  // => no super to push to

  // cast to own parameters object
  const Parameters* pInParameters = dynamic_cast <const Parameters*> (&i_parameters);
  if (0x0 == pInParameters)
  {
    return;
  }

  // validate parameters
  if (0 == pInParameters->m_maxNrUnidentifiedDevices)
  {
    throw std::runtime_error ("max. nr. unknown devices must be striclty positive");
  }

  // store parameters
  m_databaseName              = pInParameters->m_databaseName;
  ssvu::FileSystem::Path resourcesDirectoryPath (pInParameters->m_resourceDirectory);
  m_resourceDirectory         = resourcesDirectoryPath.getStr ();
  m_maxNrUnidentifiedDevices  = pInParameters->m_maxNrUnidentifiedDevices;

  SWITCH_DEBUG_MSG_0 ("success\n\r");
}

void Switch::DeviceStore::_GetParameters (Switch::ParameterizedObject::Parameters& o_parameters) const
{
  // push down to super
  // => no super to push to

  // cast to own parameters object
  Parameters* pOutParameters = dynamic_cast <Parameters*> (&o_parameters);
  if (0x0 == pOutParameters)
  {
    return;
  }

  // read parameters
  pOutParameters->m_databaseName              = m_databaseName;
  pOutParameters->m_resourceDirectory         = m_resourceDirectory;
  pOutParameters->m_maxNrUnidentifiedDevices  = m_maxNrUnidentifiedDevices;
}

/*!
  \brief Default constructor
 */
Switch::DeviceStore::DeviceStore ()
{
  _SetupParameterContainer ();

  // set the default parameters
  Parameters parameters;
  _SetParameters (parameters);
}

/*!
  \brief Constructor
 */
Switch::DeviceStore::DeviceStore (const Switch::DeviceStore::Parameters& i_parameters)
{
  _SetupParameterContainer ();

  // set the custom parameters
  _SetParameters (i_parameters);
}

/*!
  \brief Destructor
 */
Switch::DeviceStore::~DeviceStore ()
{
  // make sure the object is stopped and all resources are deallocated
  Stop ();
}

void Switch::DeviceStore::AddDevice (const switch_device_address_type& i_deviceAddress)
{
  SWITCH_DEBUG_MSG_0 ("Switch::DeviceStore::AddDevice ... ");

  std::lock_guard <std::mutex> operationsLock (m_operationsMutex);

  SWITCH_ASSERT (OS_STARTED == m_objectState);
  if (OS_STARTED != m_objectState)
  {
    throw std::runtime_error ("object must be STARTED to add device");
  }

  std::map <switch_device_address_type, Switch::Device>::const_iterator itDevice;

  // ensure the device was not yet added
  itDevice = m_unidentifiedDeviceMap.find (i_deviceAddress);
  SWITCH_ASSERT (m_unidentifiedDeviceMap.end () == itDevice);
  if (m_unidentifiedDeviceMap.end () != itDevice)
  {
    throw std::runtime_error ("device already added");
  }
  itDevice = m_deviceMap.find (i_deviceAddress);
  SWITCH_ASSERT (m_deviceMap.end () == itDevice);
  if (m_deviceMap.end () != itDevice)
  {
    throw std::runtime_error ("device already added and identified");
  }

  SWITCH_ASSERT (m_maxNrUnidentifiedDevices > m_unidentifiedDeviceMap.size ());
  if (m_maxNrUnidentifiedDevices <= m_unidentifiedDeviceMap.size ())
  {
    throw std::runtime_error ("limit unidentified devices reached");
  }

  // add the device to the database
  _WriteDatabaseDevice (i_deviceAddress);

  // create the device and add it to the map of unidentified devices
  Switch::Device device (i_deviceAddress);
  m_unidentifiedDeviceMap.insert (std::make_pair (i_deviceAddress, device));

  SWITCH_DEBUG_MSG_0 ("success!\n\r");
}

Switch::Device& Switch::DeviceStore::SetDeviceType (const switch_device_address_type& i_deviceAddress, const switch_brand_id_type& i_brandId, const switch_product_id_type& i_productId, const switch_product_version_type& i_version)
{
  SWITCH_DEBUG_MSG_0 ("Switch::DeviceStore::SetDeviceType ... ");

  std::lock_guard <std::mutex> operationsLock (m_operationsMutex);

  if (OS_STARTED != m_objectState)
  {
    throw std::runtime_error ("object must be STARTED to set device type");
  }

  std::map <switch_device_address_type, Switch::Device>::iterator itDevice;

  // ensure the device was added but yet not identified
  itDevice = m_deviceMap.find (i_deviceAddress);
  SWITCH_ASSERT (m_deviceMap.end () == itDevice);
  if (m_deviceMap.end () != itDevice)
  {
    throw std::runtime_error ("device already added and identified");
  }
  itDevice = m_unidentifiedDeviceMap.find (i_deviceAddress);
  SWITCH_ASSERT (m_unidentifiedDeviceMap.end () != itDevice);
  if (m_unidentifiedDeviceMap.end () == itDevice)
  {
    throw std::runtime_error ("device not added");
  }

  // read the product key from the database
  uint64_t productKey = _ReadDatabaseProductKey (i_brandId, i_productId, i_version);

  // update the device entry in the database
  _UpdateDatabaseDevice (i_deviceAddress, productKey);

  // find the description in the product map
  std::map <uint64_t, Switch::Device::Description>::iterator itDescription;
  itDescription = m_productDescriptionMap.find (productKey);
  if (m_productDescriptionMap.end () == itDescription)
  {
    // load the product description
    Switch::Device::Description productDescription;
    _LoadProductDescription (productDescription, i_brandId, i_productId, i_version);

    // map the description to the key
    std::pair <std::map <uint64_t, Switch::Device::Description>::iterator, bool> result = m_productDescriptionMap.insert (std::make_pair (productKey, productDescription));
    itDescription = result.first;
  }

  // set the description in the device
  itDevice->second.SetDescription (itDescription->second);

  // move the device from the unidentified devices map to the real devices map
  std::pair <std::map <switch_device_address_type, Switch::Device>::iterator, bool> result = m_deviceMap.insert (*itDevice);
  m_unidentifiedDeviceMap.erase (itDevice);

  // return a reference to the device
  SWITCH_DEBUG_MSG_0 ("success!\n\r");
  return result.first->second;
}

Switch::DeviceStore::DeviceMap& Switch::DeviceStore::GetDevices ()
{
  std::lock_guard <std::mutex> operationsLock (m_operationsMutex);

  if (OS_STOPPED == m_objectState)
  {
    throw std::runtime_error ("object must be READY or STARTED to get devices");
  }

  return m_deviceMap;
}

Switch::DeviceStore::DeviceMap& Switch::DeviceStore::GetUnidentfiedDevices ()
{
  std::lock_guard <std::mutex> operationsLock (m_operationsMutex);

  if (OS_STOPPED == m_objectState)
  {
    throw std::runtime_error ("object must be READY or STARTED to get devices");
  }

  return m_unidentifiedDeviceMap;
}

void Switch::DeviceStore::_Prepare ()
{
  SWITCH_DEBUG_MSG_0 ("Preparing Switch::DeviceStore ... ");

  std::lock_guard <std::mutex> operationsLock (m_operationsMutex);

  try
  {
    // ensure that the resource directory is available
    SWITCH_DEBUG_MSG_0 ("check resouces directory ... ");
    if (!ssvu::FileSystem::exists (m_resourceDirectory))
    {
      ssvu::FileSystem::createFolder (m_resourceDirectory);
    }

    // open the connection to the database
    SWITCH_DEBUG_MSG_0 ("open database connection ... ");
    int result = sqlite3_open (m_databaseName.c_str (), &m_pDatabaseConnection);
    if (0 != result)
    {
      char buffer [128];
      snprintf (buffer, 128, "Can't open database: %s\n", sqlite3_errmsg (m_pDatabaseConnection));
      throw std::system_error (result, std::generic_category (), buffer);
      // note: result and generic_category could be combined into an std::error_code object,
      //       generic_category could be replaced by database_category or something
    }

    // make sure the correct database layout is present
    _UpdateDatabaseLayout ();

    // load all known product descriptions
    _ReadDatabaseProducts ();

    // load all known devices
    _ReadDatabaseDevices ();
  }
  catch (...)
  {
    SWITCH_DEBUG_MSG_0 ("exception thrown\n\r");

    // cleanup
    sqlite3_close (m_pDatabaseConnection);
    m_pDatabaseConnection = 0x0;

    // forward
    throw;
  }

  SWITCH_DEBUG_MSG_0 ("success!\n\r");
}

void Switch::DeviceStore::_Start ()
{
  SWITCH_DEBUG_MSG_0 ("Starting Switch::DeviceStore ... ");

  std::lock_guard <std::mutex> operationsLock (m_operationsMutex);

  SWITCH_DEBUG_MSG_0 ("success!\n\r");
}

void Switch::DeviceStore::_Pause ()
{
  SWITCH_DEBUG_MSG_0 ("Pausing Switch::DeviceStore ... ");

  std::lock_guard <std::mutex> operationsLock (m_operationsMutex);

  SWITCH_DEBUG_MSG_0 ("success!\n\r");
}

void Switch::DeviceStore::_Stop ()
{
  SWITCH_DEBUG_MSG_0 ("Stopping Switch::DeviceStore ... ");

  std::lock_guard <std::mutex> operationsLock (m_operationsMutex);

  // clear all devices
  SWITCH_DEBUG_MSG_0 ("clearing devices ... ");
  m_unidentifiedDeviceMap.clear ();
  m_deviceMap.clear ();

  // clear all product descriptions
  SWITCH_DEBUG_MSG_0 ("clearing products ... ");
  m_productDescriptionMap.clear ();

  // close the connection to the database
  SWITCH_DEBUG_MSG_0 ("closing database connection ... ");
  sqlite3_close (m_pDatabaseConnection);
  m_pDatabaseConnection = 0x0;

  SWITCH_DEBUG_MSG_0 ("success!\n\r");
}

/*!
  \brief Updates the database layout to back the store

  Checks if the database layout matches the requirements and alters the layout
  if needed.

  \note A connection to the database must exist.
 */
void Switch::DeviceStore::_UpdateDatabaseLayout ()
{
  SWITCH_DEBUG_MSG_0 ("update database layout ... ");

  int result;
  char* errorMessage = 0x0;

  try
  {
    // device table
    result = sqlite3_exec
    (
      m_pDatabaseConnection,
      "CREATE TABLE IF NOT EXISTS devices (address INTEGER NOT NULL PRIMARY KEY, productKey INTEGER);",
      0x0,
      0x0,
      &errorMessage
    );

    if (SQLITE_OK != result)
    {
      char buffer [128];
      snprintf (buffer, 128, "Can't update database layout': %s\n", errorMessage);
      throw std::system_error (result, std::generic_category (), buffer);
    }

    // products table
    result = sqlite3_exec
    (
      m_pDatabaseConnection,
      "CREATE TABLE IF NOT EXISTS products (key INTEGER PRIMARY KEY AUTOINCREMENT, brandId int NOT NULL, productId int NOT NULL, version int NOT NULL);",
      0x0,
      0x0,
      &errorMessage
    );

    if (SQLITE_OK != result)
    {
      char buffer [128];
      snprintf (buffer, 128, "Can't update database layout': %s\n", errorMessage);
      throw std::system_error (result, std::generic_category (), buffer);
    }
  }
  catch (...)
  {
    // cleanup
    sqlite3_free (errorMessage);

    // forward
    throw;
  }
}

void Switch::DeviceStore::_WriteDatabaseDevice (const switch_device_address_type& i_deviceAddress)
{
  SWITCH_DEBUG_MSG_0 ("write database device ... ");

  int result;
  char stringBuffer [128];
  char* errorMessage = 0x0;

  try
  {
    // create statement
    snprintf (stringBuffer, 128, "INSERT INTO devices (address) VALUES (%u);", i_deviceAddress);

    // device table
    result = sqlite3_exec
    (
      m_pDatabaseConnection,
      stringBuffer,
      0x0,
      0x0,
      &errorMessage
    );

    if (SQLITE_OK != result)
    {
      snprintf (stringBuffer, 128, "Can't write device to database: %s\n", errorMessage);
      throw std::system_error (result, std::generic_category (), stringBuffer);
    }
  }
  catch (...)
  {
    // cleanup
    sqlite3_free (errorMessage);
    errorMessage = 0x0;

    // forward
    throw;
  }
}

void Switch::DeviceStore::_UpdateDatabaseDevice (const switch_device_address_type& i_deviceAddress, const uint64_t& i_productKey)
{
  SWITCH_DEBUG_MSG_0 ("update database device ... ");

  int result;
  char stringBuffer [128];
  char* errorMessage = 0x0;

  try
  {
    // create statement
    snprintf (stringBuffer, 128, "UPDATE devices SET productKey=%llu WHERE address=%u;", i_productKey, i_deviceAddress);

    // device table
    result = sqlite3_exec
    (
      m_pDatabaseConnection,
     stringBuffer,
     0x0,
     0x0,
     &errorMessage
    );

    if (SQLITE_OK != result)
    {
      snprintf (stringBuffer, 128, "Can't update device in database: %s\n", errorMessage);
      throw std::system_error (result, std::generic_category (), stringBuffer);
    }
  }
  catch (...)
  {
    // cleanup
    sqlite3_free (errorMessage);
    errorMessage = 0x0;

    // forward
    throw;
  }
}

uint64_t Switch::DeviceStore::_ReadDatabaseProductKey (const switch_brand_id_type& i_brandId, const switch_product_id_type& i_productId, const switch_product_version_type& i_version)
{
  SWITCH_DEBUG_MSG_0 ("read database product key ... ");

  int result;
  char stringBuffer [128];
  sqlite3_stmt* pStatementSelectKey = 0x0;
  char* errorMessage = 0x0;

  try
  {
    // assuming product is in database, select its key
    // prepare select statement
    snprintf
    (
      stringBuffer, 128,
      "SELECT (key) FROM products WHERE brandId=%u AND productId=%u AND version=%u;",
      i_brandId,
      i_productId,
      i_version
    );
    result = sqlite3_prepare_v2
    (
      m_pDatabaseConnection,
      stringBuffer,
      -1,
      &pStatementSelectKey,
      0x0
    );
    SWITCH_ASSERT (0x0 != pStatementSelectKey);
    if (SQLITE_OK != result)
    {
      snprintf (stringBuffer, 128, "Can't prepare statement: %s\n", sqlite3_errmsg (m_pDatabaseConnection));
      throw std::system_error (result, std::generic_category (), stringBuffer);
    }

    // execute statement
    result = sqlite3_step (pStatementSelectKey);
    if (SQLITE_ROW == result)
    {
      // get key from statement
      uint64_t key = sqlite3_column_int64 (pStatementSelectKey, 0);

      // cleanup
      sqlite3_finalize (pStatementSelectKey);
      pStatementSelectKey = 0x0;

      // return key
      return key;
    }
    else if (SQLITE_DONE != result)
    {
      snprintf (stringBuffer, 128, "Can't execute statement: %s\n", sqlite3_errmsg (m_pDatabaseConnection));
      throw std::system_error (result, std::generic_category (), stringBuffer);
    }

    // product is not yet in database, add it and then return key
    // create statement
    snprintf
    (
      stringBuffer, 128,
      "INSERT INTO products (brandId, productId, version) VALUES (%u, %u, %u);",
      i_brandId,
      i_productId,
      i_version
    );

    // execuyte statement
    result = sqlite3_exec
    (
      m_pDatabaseConnection,
      stringBuffer,
      0x0,
      0x0,
      &errorMessage
    );

    // handle errors
    if (SQLITE_OK != result)
    {
      snprintf (stringBuffer, 128, "Can't write product to database: %s\n", errorMessage);
      throw std::system_error (result, std::generic_category (), stringBuffer);
    }

    // return the primary key of the last insertion
    return sqlite3_last_insert_rowid (m_pDatabaseConnection);
  }
  catch (...)
  {
    // cleanup
    sqlite3_finalize (pStatementSelectKey);
    pStatementSelectKey = 0x0;

    sqlite3_free (errorMessage);
    errorMessage = 0x0;

    // forward
    throw;
  }
}

int Switch::_HandleDatabaseProductRead (void* i_pEndPoint, int i_nrColumns, char** i_ppRowValues, char** i_ppColumnNames)
{
  std::thread::id threadId = std::this_thread::get_id ();
  std::cout << "_HandleDatabaseProductRead thread Id " << threadId << std::endl;

  try
  {
    SQLiteCallback* pCallback = static_cast <SQLiteCallback*> (i_pEndPoint);
    return pCallback->operator () (i_nrColumns, i_ppRowValues, i_ppColumnNames);
  }
  catch (const std::exception& i_exception)
  {
    SWITCH_DEBUG_MSG_1 ("bad callback method for _HandleDatabaseProductRead: %s\n\r", i_exception.what ());
    return SQLITE_ABORT;
  }
}

void Switch::DeviceStore::_ReadDatabaseProducts ()
{
  SWITCH_DEBUG_MSG_0 ("read database products ... ");

  int result;
  char stringBuffer [128];
  char* errorMessage = 0x0;

  m_productDescriptionMap.clear ();

  try
  {
    // bind to the callback function
    SQLiteCallback callback = std::bind (&Switch::DeviceStore::_OnDatabaseProductRead, this, _1, _2, _3);

    // device table
    result = sqlite3_exec
    (
      m_pDatabaseConnection,
     "SELECT key, brandId, productId, version FROM products;",
     _HandleDatabaseProductRead,
     &callback,
     &errorMessage
    );

    if (SQLITE_OK != result)
    {
      snprintf (stringBuffer, 128, "Can't read products from database: %s\n", errorMessage);
      throw std::system_error (result, std::generic_category (), stringBuffer);
    }
  }
  catch (...)
  {
    // cleanup
    sqlite3_free (errorMessage);
    errorMessage = 0x0;

    // forward
    throw;
  }
}

int Switch::DeviceStore::_OnDatabaseProductRead (const int& i_nrColumns, char const* const* i_ppRowValues, char const* const* i_ppColumnNames)
{
  std::stringstream conversionStream;

  // verify the database scheme
  SWITCH_ASSERT_RETURN_1 (0 == strcmp ("key",       i_ppColumnNames [0]), SQLITE_FORMAT);
  SWITCH_ASSERT_RETURN_1 (0 == strcmp ("brandId",   i_ppColumnNames [1]), SQLITE_FORMAT);
  SWITCH_ASSERT_RETURN_1 (0 == strcmp ("productId", i_ppColumnNames [2]), SQLITE_FORMAT);
  SWITCH_ASSERT_RETURN_1 (0 == strcmp ("version",   i_ppColumnNames [3]), SQLITE_FORMAT);

  // get the product key
  uint64_t productKey;
  conversionStream.clear ();
  conversionStream << i_ppRowValues [0];
  conversionStream >> productKey;

  // get the brand id
  switch_brand_id_type brandId;
  conversionStream.clear ();
  conversionStream << i_ppRowValues [1];
  conversionStream >> brandId;

  // get the product id
  switch_product_id_type productId;
  conversionStream.clear ();
  conversionStream << i_ppRowValues [2];
  conversionStream >> productId;

  // get the product version
  switch_product_version_type version;
  conversionStream.clear ();
  conversionStream << i_ppRowValues [3];
  conversionStream >> version;

  // load the product description
  Switch::Device::Description productDescription;
  _LoadProductDescription (productDescription, brandId, productId, version);

  // map the description to the key
  m_productDescriptionMap.insert (std::make_pair (productKey, productDescription));

  // all operations succeeded
  return SQLITE_OK;
}

int Switch::_HandleDatabaseDeviceRead (void* i_pEndPoint, int i_nrColumns, char** i_ppRowValues, char** i_ppColumnNames)
{
  std::thread::id threadId = std::this_thread::get_id ();
  std::cout << "_HandleDatabaseDeviceRead thread Id " << threadId << std::endl;

  try
  {
    SQLiteCallback* pCallback = static_cast <SQLiteCallback*> (i_pEndPoint);
    return pCallback->operator () (i_nrColumns, i_ppRowValues, i_ppColumnNames);
  }
  catch (const std::exception& i_exception)
  {
    SWITCH_DEBUG_MSG_1 ("bad callback method for _HandleDatabaseDeviceRead: %s\n\r", i_exception.what ());
    return SQLITE_ABORT;
  }
}

void Switch::DeviceStore::_ReadDatabaseDevices ()
{
  SWITCH_DEBUG_MSG_0 ("read database devices ... ");

  int result;
  char stringBuffer [128];
  char* errorMessage = 0x0;

  m_unidentifiedDeviceMap.clear ();
  m_deviceMap.clear ();

  try
  {
    // bind to the callback function
    SQLiteCallback callback = std::bind (&Switch::DeviceStore::_OnDatabaseDeviceRead, this, _1, _2, _3);

    // device table
    result = sqlite3_exec
    (
      m_pDatabaseConnection,
      "SELECT address, productKey FROM devices;",
      _HandleDatabaseDeviceRead,
      &callback,
      &errorMessage
    );

    if (SQLITE_OK != result)
    {
      snprintf (stringBuffer, 128, "Can't read devices from database: %s\n", errorMessage);
      throw std::system_error (result, std::generic_category (), stringBuffer);
    }
  }
  catch (...)
  {
    // cleanup
    sqlite3_free (errorMessage);
    errorMessage = 0x0;

    // forward
    throw;
  }
}

int Switch::DeviceStore::_OnDatabaseDeviceRead (const int& i_nrColumns, char const* const* i_ppRowValues, char const* const* i_ppColumnNames)
{
  std::stringstream conversionStream;

  // verify the database scheme
  SWITCH_ASSERT_RETURN_1 (0 == strcmp ("address",     i_ppColumnNames [0]), SQLITE_FORMAT);
  SWITCH_ASSERT_RETURN_1 (0 == strcmp ("productKey",  i_ppColumnNames [1]), SQLITE_FORMAT);

  // get the device address
  switch_device_address_type deviceAddress;
  conversionStream.clear ();
  conversionStream << i_ppRowValues [0];
  conversionStream >> deviceAddress;

  // create the device
  Switch::Device device (deviceAddress);

  // check if the device is linked to a product
  if (0x0 == i_ppRowValues [1])
  {
    // add the device to the list of unidentified devices
    m_unidentifiedDeviceMap.insert (std::make_pair (deviceAddress, device));

    SWITCH_DEBUG_MSG_1 ("device %u added to unidentified devices ... ", deviceAddress);
    return SQLITE_OK;
  }

  // get the product key
  uint64_t productKey;
  conversionStream.clear ();
  conversionStream << i_ppRowValues [1];
  conversionStream >> productKey;

  // fetch the description of the device
  std::map <uint64_t, Switch::Device::Description>::const_iterator itDescription = m_productDescriptionMap.find (productKey);
  if (m_productDescriptionMap.end () == itDescription)
  {
    // device description unavailable
    // add the device to the list of unidentified devices
    m_unidentifiedDeviceMap.insert (std::make_pair (deviceAddress, device));

    SWITCH_DEBUG_MSG_1 ("description of device with address %u is unavailable ... ", deviceAddress);
    return SQLITE_OK;
  }

  // set the device's description
  device.SetDescription (itDescription->second);

  // map the device to its address
  m_deviceMap.insert (std::make_pair (deviceAddress, device));

  // all operations succeeded
  SWITCH_DEBUG_MSG_1 ("device %u added to device map ... ", deviceAddress);
  return SQLITE_OK;
}

void Switch::DeviceStore::_LoadProductDescription (Switch::Device::Description& o_description, const switch_brand_id_type& i_brandId, const switch_product_id_type& i_productId, const switch_product_version_type& i_version)
{
  SWITCH_DEBUG_MSG_0 ("load product description ... ");

  // assemble the name of the description file
  const size_t bufferSize = 64;
  char fileNameBuffer [bufferSize];
  snprintf (fileNameBuffer, bufferSize, "product_%u_%u_%u.spd", i_brandId, i_productId, i_version);

  // assemble the path to the description file
  std::stringstream filePathStream;
  filePathStream << m_resourceDirectory
                 << fileNameBuffer;

  // extract the path to the description file
  std::string filePath = filePathStream.str ();
  if (!ssvu::FileSystem::exists (filePath))
  {
    throw std::ios_base::failure ("description file does not exist");
  }

  // open the stream to the description file
  std::ifstream inDescriptionStream (filePath);
  if (!inDescriptionStream.good ())
  {
    throw std::ios_base::failure ("failed to open the description file");
  }

  // parse the stream to a product description
  Switch::JsonSerializer::Deserialize (o_description, inDescriptionStream);
}
