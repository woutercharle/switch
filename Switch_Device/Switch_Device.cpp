/*?*************************************************************************
*                           Switch_Device.cpp
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

#include "Switch_Device.h"
#include "Switch_DataContainer.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"

// third-party includes
#include <stdexcept>
#include <json/json.h>


/*!
  \brief Constructor
 */
Switch::Device::Description::Description ()
: m_deviceBrandId (0),
  m_deviceProductId (0),
  m_deviceProductVersion (0)
{
}

/*!
  \brief Destructor
 */
Switch::Device::Description::~Description ()
{
}

/*!
 * \brief Copy constructor.
 *
 * \param [in] i_other Object to copy.
 */
Switch::Device::Description::Description (const Switch::Device::Description& i_other)
{
  m_deviceBrandId         = i_other.m_deviceBrandId;
  m_deviceBrandName       = i_other.m_deviceBrandName;
  m_deviceProductId       = i_other.m_deviceProductId;
  m_deviceProductName     = i_other.m_deviceProductName;
  m_deviceProductVersion  = i_other.m_deviceProductVersion;
  m_dataFormat            = i_other.m_dataFormat;
}

/*!
 * \brief Assignment operator.
 *
 * \param [in] i_other Object to assign.
 *
 * \return Reference to this object.
 */
Switch::Device::Description& Switch::Device::Description::operator= (const Switch::Device::Description& i_other)
{
  if (this != &i_other)
  {
    m_deviceBrandId         = i_other.m_deviceBrandId;
    m_deviceBrandName       = i_other.m_deviceBrandName;
    m_deviceProductId       = i_other.m_deviceProductId;
    m_deviceProductName     = i_other.m_deviceProductName;
    m_deviceProductVersion  = i_other.m_deviceProductVersion;
    m_dataFormat            = i_other.m_dataFormat;
  }

  return *this;
}

/*!
 \brief Equality comparator.

 \param [in] i_other Object to compare to.

 \return True if the compared object is equal, false otherwise.
 */
bool Switch::Device::Description::operator== (const Switch::Device::Description& i_other) const
{
  if (this == &i_other)
  {
    return true;
  }

  if (m_deviceBrandId != i_other.m_deviceBrandId)
  {
    return false;
  }
  if (m_deviceProductId != i_other.m_deviceProductId)
  {
    return false;
  }
  if (m_deviceProductVersion != i_other.m_deviceProductVersion)
  {
    return false;
  }
  if (m_dataFormat != i_other.m_dataFormat)
  {
    return false;
  }

  SWITCH_ASSERT (m_deviceBrandName == i_other.m_deviceBrandName);
  SWITCH_ASSERT (m_deviceProductName == i_other.m_deviceProductName);

  return true;
}

/*!
 \brief Inequality comparator.

 \param [in] i_other Object to compare to.

 \return True if the compared object is inequal, false otherwise.
 */
bool Switch::Device::Description::operator!= (const Switch::Device::Description& i_other) const
{
  return !(this->operator== (i_other));
}

/*!
  \brief Serializes the object to a JSON value.

  \param [out] o_root Root value of the object as JSON.
 */
void Switch::Device::Description::Serialize (Json::Value& o_root) const
{
  o_root ["brandId"]        = m_deviceBrandId;
  o_root ["brand"]          = m_deviceBrandName;
  o_root ["productId"]      = m_deviceProductId;
  o_root ["productName"]    = m_deviceProductName;
  o_root ["productVersion"] = m_deviceProductVersion;

  // serialize the container's data format
  Json::Value dataFormatValue;
  m_dataFormat.Serialize (dataFormatValue);
  o_root ["dataFormat"] = dataFormatValue;

# ifdef SWITCH_SERIALIZE_WITH_COMMENTS
  // add comments
  o_root ["brandId"].         setComment ("// The id of the brand of the device.", Json::commentAfterOnSameLine);
  o_root ["brand"].           setComment ("// The brand of the device.", Json::commentAfterOnSameLine);
  o_root ["productId"].       setComment ("// The id of the product the device.", Json::commentAfterOnSameLine);
  o_root ["productName"].     setComment ("// The name of the product.", Json::commentAfterOnSameLine);
  o_root ["productVersion"].  setComment ("// The version of the product.", Json::commentAfterOnSameLine);
  o_root ["dataFormat"].      setComment ("// The format of data used by the device.", Json::commentAfterOnSameLine);
# endif
}

/*!
  \brief De-serializes the object from a JSON value.

  \param [in] i_root Root value of the object as JSON.
 */
void Switch::Device::Description::Deserialize (const Json::Value& i_root)
{
  // de-serialize members
  m_deviceBrandId         = i_root ["brandId"].       asUInt ();
  m_deviceBrandName       = i_root ["brand"].         asString ();
  m_deviceProductId       = i_root ["productId"].     asUInt ();
  m_deviceProductName     = i_root ["productName"].   asString ();
  m_deviceProductVersion  = i_root ["productVersion"].asUInt ();

  // de-serialize the data format
  const Json::Value& dataFormatValue = i_root ["dataFormat"];
  m_dataFormat.Deserialize (dataFormatValue);
}


/*!
  \brief Constructor

  \param [in] i_dataFormat The data format of the data adapter
 */
Switch::Device::Device (const switch_device_address_type& i_deviceAddress)
: m_address         (i_deviceAddress),
  m_pDescription    (0x0),
  m_pDataContainer  (0x0)
{
}

/*!
  \brief Destructor
 */
Switch::Device::~Device ()
{
  // cleanup data
  delete m_pDataContainer;
  m_pDataContainer = 0x0;

  // forget the device description
  m_pDescription = 0x0;
}

/*!
  \brief Copy constructor.

  \param [in] i_other Object to copy.
 */
Switch::Device::Device (const Device& i_other)
: m_address (i_other.m_address),
  m_pDescription (i_other.m_pDescription),
  m_pDataContainer (0x0)
{
  try
  {
    if (0x0 != i_other.m_pDataContainer)
    {
      const Switch::DataContainer& otherContainer = *i_other.m_pDataContainer;
      m_pDataContainer = new Switch::DataContainer (otherContainer);
    }
  }
  catch (...)
  {
    // cleanup data
    delete m_pDataContainer;
    m_pDataContainer = 0x0;

    // forget the device description
    m_pDescription = 0x0;

    throw;
  }
}

/*!
  \brief Assignment operator.

  \param [in] i_other Object to assign.

  \return Reference to this object.

  \note Only objects with same address can be assigned to each-other.
 */
Switch::Device& Switch::Device::operator= (const Device& i_other)
{
  if (this != &i_other)
  {
    SWITCH_ASSERT (m_address == i_other.m_address);

    try
    {
      m_pDescription  = i_other.m_pDescription;

      if (0x0 == i_other.m_pDataContainer)
      {
        delete m_pDataContainer;
        m_pDataContainer = 0x0;
      }
      else
      {
        const Switch::DataContainer& otherContainer = *i_other.m_pDataContainer;

        if (0x0 == m_pDataContainer)
        {
          m_pDataContainer = new Switch::DataContainer (otherContainer);
        }
        else
        {
          Switch::DataContainer& thisContainer = *m_pDataContainer;
          thisContainer = otherContainer;
        }
      }
    }
    catch (...)
    {
      // cleanup data
      delete m_pDataContainer;
      m_pDataContainer = 0x0;

      // forget the device description
      m_pDescription = 0x0;

      throw;
    }
  }

  return *this;
}

/*!
  \brief Sets the device's description

  Sets general device properties and initializes the data container.

  \param [in] i_deviceDescription Description of the device.

  \throw std::rutime_error if the device description was already set.

  \see GetDescription
 */
void Switch::Device::SetDescription (const Switch::Device::Description& i_deviceDescription)
{
  // ensure the the device description was not yet set
  SWITCH_ASSERT_THROW (0x0 == m_pDescription, std::runtime_error ("device description already set"));
  SWITCH_ASSERT_THROW (0x0 == m_pDataContainer, std::runtime_error ("device description not set, but data container allocated"));
  if (0x0 != m_pDescription)
  {
    throw std::runtime_error ("device description already set");
  }

  try
  {
    // store device description
    m_pDescription = &i_deviceDescription;

    // allocate a new data container
    m_pDataContainer = new Switch::DataContainer (i_deviceDescription.m_dataFormat);
  }
  catch (...)
  {
    // cleanup data
    delete m_pDataContainer;
    m_pDataContainer = 0x0;

    // forget the device description
    m_pDescription = 0x0;

    // rethrow
    throw;
  }
}

/*!
 \brief Gets the device's description

 Gets the general device properties.

 \return Description of the device.

 \note The device's description must be set.

 \throw std::runtime_error if the device description was not set.

 \see SetDescription
 */
const Switch::Device::Description& Switch::Device::GetDescription () const
{
  // ensure the the device description is set
  SWITCH_ASSERT_THROW (0x0 != m_pDescription, std::runtime_error ("device description not set"));
  SWITCH_ASSERT_THROW (0x0 != m_pDataContainer, std::runtime_error ("device description set, but data container not allocated"));
  if (0x0 == m_pDescription)
  {
    throw std::runtime_error ("device description not set");
  }

  return *m_pDescription;
}

/*!
  \brief Gets the device's connection state.

  \return True if the device is connected, false otherwise.
 */
bool Switch::Device::GetConnectionState () const
{
  return m_connected;
}

/*!
  \brief Sets the device's connection state.

  \param [in] i_connected The device's connection state: True if the device is connected, false otherwise.
 */
void Switch::Device::SetConnectionState (const bool& i_connected)
{
  m_connected = i_connected;
}

/*!
  \brief Gets a const reference to the device's data container.

  \return Const reference to the device's data container.

  \throw std::runtime_error if the device description was not set.

  \see SetDescription
 */
const Switch::DataContainer& Switch::Device::GetDataContainer () const
{
  // ensure the the device description is set
  SWITCH_ASSERT_THROW (0x0 != m_pDescription, std::runtime_error ("device description not set"));
  SWITCH_ASSERT_THROW (0x0 != m_pDataContainer, std::runtime_error ("device description set, but data container not allocated"));
  if (0x0 == m_pDescription)
  {
    throw std::runtime_error ("device description not set");
  }

  // return refernce
  return *m_pDataContainer;
}

/*!
  \brief Gets a reference to the device's data container.

  \return Reference to the device's data container.

  \throw std::runtime_error if the device description was not set.

  \see SetDescription
 */
Switch::DataContainer& Switch::Device::GetDataContainer ()
{
  // ensure the the device description is set
  SWITCH_ASSERT_THROW (0x0 != m_pDescription, std::runtime_error ("device description not set"));
  SWITCH_ASSERT_THROW (0x0 != m_pDataContainer, std::runtime_error ("device description set, but data container not allocated"));
  if (0x0 == m_pDescription)
  {
    throw std::runtime_error ("device description not set");
  }

  // return refernce
  return *m_pDataContainer;
}
