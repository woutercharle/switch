/*?*************************************************************************
*                           Switch_DataAdapter.cpp
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

#include "Switch_DataAdapter.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"

// thirdparty includes
#include <cmath>
#include <json/json.h>


/*!
  \brief Default constructor.
 */
Switch::DataAdapter::DataFormat::DataFormat ()
: m_bitOffset ( 0),
  m_bitSize   ( 0),
  m_minValue  ( 1),
  m_maxValue  (-1)
{
  m_name        = "N.A.";
  m_description = "N.A.";
}

/*!
  \brief Constructor with minimal set of arguments

  \param [in] i_name        Name of the data object
  \param [in] i_description Description of the data object
  \param [in] i_bitOffset   The index of the first bit. Ordered from right (small indices) to left (large indices). Values 0 to 7.
  \param [in] i_bitSize     The data length in bits, starting from the bit at m_bitOffset in m_pData.
 */
Switch::DataAdapter::DataFormat::DataFormat (const std::string& i_name,  const std::string& i_description,
                                             const uint8_t& i_bitOffset, const uint32_t& i_bitSize)
: m_bitOffset (i_bitOffset),
  m_bitSize   (i_bitSize)
{
  // set descriptors
  m_name        = i_name;
  m_description = i_description;
  m_group       = "";

  // compute min and max value from bit size
  m_minValue = static_cast <int32_t> (-1*::pow (2, (i_bitSize - 1)));
  m_maxValue = static_cast <int32_t> (::pow (2, (i_bitSize - 1)) - 1);

  // assert other values
  SWITCH_ASSERT (m_bitOffset < 8);
}

/*!
  \brief Constructor with maximal set of arguments

  \param [in] i_name        Name of the data object
  \param [in] i_description Description of the data object
  \param [in] i_group       The name of the group this type of adapter belongs to.
  \param [in] i_bitOffset   The index of the first bit. Ordered from right (small indices) to left (large indices). Values 0 to 7.
  \param [in] i_bitSize     The data length in bits, starting from the bit at m_bitOffset in m_pData.
  \param [in] i_maxValue    The minimum data value.
  \param [in] i_minValue    The maximum data value.

  \note The difference between max and min value must fit in the bit size. All data is stored unsigned with reference to the min value.
 */
Switch::DataAdapter::DataFormat::DataFormat (const std::string& i_name,  const std::string& i_description, const std::string& i_group,
                                             const uint8_t& i_bitOffset, const uint32_t& i_bitSize,        const int32_t& i_minValue,
                                             const int32_t& i_maxValue)
: m_bitOffset (i_bitOffset),
  m_bitSize   (i_bitSize),
  m_minValue  (i_minValue),
  m_maxValue  (i_maxValue)
{
  // set descriptors
  m_name        = i_name;
  m_description = i_description;
  m_group       = i_group;

  // assert other values
  SWITCH_ASSERT (m_bitOffset < 8);
  SWITCH_ASSERT ((m_maxValue - m_minValue) <= static_cast <int32_t> (::pow (2, m_bitSize) - 1));
}

/*!
  \brief Destructor
 */
Switch::DataAdapter::DataFormat::~DataFormat ()
{
}

/*!
 \brief Copy constructor.

 \param [in] i_other Object to copy.
 */
Switch::DataAdapter::DataFormat::DataFormat (const Switch::DataAdapter::DataFormat& i_other)
{
  m_name        = i_other.m_name;
  m_description = i_other.m_description;
  m_group       = i_other.m_group;
  m_bitOffset   = i_other.m_bitOffset;
  m_bitSize     = i_other.m_bitSize;
  m_minValue    = i_other.m_minValue;
  m_maxValue    = i_other.m_maxValue;
}

/*!
 \brief Assignment operator.

 \param [in] i_other Object to assign.

 \return Reference to this object.
 */
Switch::DataAdapter::DataFormat& Switch::DataAdapter::DataFormat::operator= (const Switch::DataAdapter::DataFormat& i_other)
{
  if (this != &i_other)
  {
    m_name        = i_other.m_name;
    m_description = i_other.m_description;
    m_group       = i_other.m_group;
    m_bitOffset   = i_other.m_bitOffset;
    m_bitSize     = i_other.m_bitSize;
    m_minValue    = i_other.m_minValue;
    m_maxValue    = i_other.m_maxValue;
  }

  return *this;
}

/*!
 * \brief Equality comparator.
 *
 * \param [in] i_other Object to compare to.
 *
 * \return True if the compared object is equal, false otherwise.
 */
bool Switch::DataAdapter::DataFormat::operator== (const Switch::DataAdapter::DataFormat& i_other) const
{
  if (this == &i_other)
  {
    return true;
  }

  if (m_name != i_other.m_name)
  {
    return false;
  }
  if (m_description != i_other.m_description)
  {
    return false;
  }
  if (m_group != i_other.m_group)
  {
    return false;
  }
  if (m_bitOffset != i_other.m_bitOffset)
  {
    return false;
  }
  if (m_bitSize != i_other.m_bitSize)
  {
    return false;
  }
  if (m_minValue != i_other.m_minValue)
  {
    return false;
  }
  if (m_maxValue != i_other.m_maxValue)
  {
    return false;
  }

  return true;
}

/*!
 * \brief Inequality comparator.
 *
 * \param [in] i_other Object to compare to.
 *
 * \return True if the compared object is inequal, false otherwise.
 */
bool Switch::DataAdapter::DataFormat::operator!= (const Switch::DataAdapter::DataFormat& i_other) const
{
  return !(this->operator== (i_other));
}

/*!
  \brief Validates a value against the data format's description.

  \param [in] i_value The value to validate against the data format.

  \return True if the value satisfies the data format's description, false otherwise.
 */
bool Switch::DataAdapter::DataFormat::ValidateValue (const int32_t& i_value) const
{
  // check boundaries
  if (i_value < m_minValue)
  {
    return false;
  }
  if (i_value > m_maxValue)
  {
    return false;
  }

  // all test succeeded
  return true;
}

/*!
  \brief Computes the magic number for this object.

  The magic number can be used to verify e.g., the address of data dapters.

  \param [in] i_baseNumber Base number used to compute the magic number.

  \return Magic number.
 */
uint32_t Switch::DataAdapter::DataFormat::ComputeMagicNumber (const uint32_t& i_baseNumber) const
{
  return (*reinterpret_cast <uint32_t*> (&m_maxValue) + *reinterpret_cast <uint32_t*> (&m_minValue) + m_bitOffset) << (32 - m_bitSize);
}

/*!
  \brief Serializes the object to a JSON value.

  \param [out] o_root Root value of the object as JSON.
 */
void Switch::DataAdapter::DataFormat::Serialize (Json::Value& o_root) const
{
  // add members
  o_root ["name"]         = m_name;
  o_root ["description"]  = m_description;
  o_root ["group"]        = m_group;
  o_root ["bitOffset"]    = m_bitOffset;
  o_root ["bitSize"]      = m_bitSize;
  o_root ["minValue"]     = m_minValue;
  o_root ["maxValue"]     = m_maxValue;

  // add comments
# ifdef SWITCH_SERIALIZE_WITH_COMMENTS
  o_root ["name"].        setComment ("// The name of the data object.", Json::commentAfterOnSameLine);
  o_root ["description"]. setComment ("// A description of the data object.", Json::commentAfterOnSameLine);
  o_root ["group"].       setComment ("// The name of the group this type of adapter belongs to.", Json::commentAfterOnSameLine);
  o_root ["bitOffset"].   setComment ("// The index of the first bit. Ordered from right (small indices) to left (large indices). Values 0 to 7.", Json::commentAfterOnSameLine);
  o_root ["bitSize"].     setComment ("// The data length in bits, starting from the bit at m_bitOffset in m_pData.", Json::commentAfterOnSameLine);
  o_root ["minValue"].    setComment ("// The minimum data value.", Json::commentAfterOnSameLine);
  o_root ["maxValue"].    setComment ("// The maximum data value.", Json::commentAfterOnSameLine);
# endif
}

/*!
  \brief De-serializes the object from a JSON value.

  \param [in] i_root Root value of the object as JSON.
 */
void Switch::DataAdapter::DataFormat::Deserialize (const Json::Value& i_root)
{
  // deserialize members
  m_name        = i_root ["name"].        asString ();
  m_description = i_root ["description"]. asString ();
  m_group       = i_root ["group"].       asString ();
  m_bitOffset   = i_root ["bitOffset"].   asUInt ();
  m_bitSize     = i_root ["bitSize"].     asUInt ();
  m_minValue    = i_root ["minValue"].    asInt ();
  m_maxValue    = i_root ["maxValue"].    asInt ();
}

/*!
  \brief Constructor.

  \param [in] i_dataFormat The data format of the data adapter.

  \note Allocates memory for the data and initializes to the minimum value.

  \throw Strong guarantee. Throws when allocation fails.
 */
Switch::DataAdapter::DataAdapter (const Switch::DataAdapter::DataFormat& i_dataFormat)
: mr_dataFormat (i_dataFormat),
  m_pData       (0x0),
  m_ownsData    (true)
{
  try
  {
    uint8_t boundingBitSize = (mr_dataFormat.m_bitOffset + mr_dataFormat.m_bitSize + 7) / 8;
    m_pData = new uint8_t [boundingBitSize];
    memset (m_pData, 0, boundingBitSize);

    _CreateMask ();
  }
  catch (...)
  {
    delete m_pData;
    m_pData = 0x0;

    throw;
  }
}

/*!
  \brief Constructor

  \param [in] i_pData Access to the underlying data buffer, shared with this data adapter
  \param [in] i_dataFormat The data format of the data adapter

  \throw No-throw guarantee.
 */
Switch::DataAdapter::DataAdapter (uint8_t* i_pData, const DataFormat& i_dataFormat)
: mr_dataFormat (i_dataFormat),
  m_pData       (i_pData),
  m_ownsData    (false)
{
  _CreateMask ();
}

/*!
  \brief Destructor.
 */
Switch::DataAdapter::~DataAdapter ()
{
  if (m_ownsData)
  {
    delete m_pData;
    m_pData = 0x0;
  }
}

/*!
 \brief Copy constructor.

 \param [in] i_other Object to copy.
 */
Switch::DataAdapter::DataAdapter (const DataAdapter& i_other)
: mr_dataFormat (i_other.mr_dataFormat),
  m_pData       (0x0),
  m_ownsData    (false)
{
  try
  {
    if (i_other.m_ownsData)
    {
      m_ownsData = true;
      uint8_t boundingBitSize = (mr_dataFormat.m_bitOffset + mr_dataFormat.m_bitSize + 7) / 8;
      m_pData = new uint8_t [boundingBitSize];
      memcpy (m_pData, i_other.m_pData, boundingBitSize);
    }
    else
    {
      m_ownsData = false;
      m_pData = i_other.m_pData;
    }

    _CreateMask ();
  }
  catch (...)
  {
    if (m_ownsData)
    {
      delete m_pData;
      m_pData = 0x0;
    }

    throw;
  }
}

/*!
  \brief Assignment operator.

  \param [in] i_other Object to assign.

  \return Reference to this object.

  \note Only objects with same data format can be assigned to each-other.
 */
Switch::DataAdapter& Switch::DataAdapter::operator= (const DataAdapter& i_other)
{
  if (this != &i_other)
  {
    SWITCH_ASSERT (mr_dataFormat == i_other.mr_dataFormat);

    if (m_ownsData)
    {
      delete m_pData;
      m_pData = 0x0;
      m_ownsData = false;
    }

    try
    {
      if (i_other.m_ownsData)
      {
        m_ownsData = true;
        uint8_t boundingBitSize = (mr_dataFormat.m_bitOffset + mr_dataFormat.m_bitSize + 7) / 8;
        m_pData = new uint8_t [boundingBitSize];
        memcpy (m_pData, i_other.m_pData, boundingBitSize);
      }
      else
      {
        m_ownsData = false;
        m_pData = i_other.m_pData;
      }

      _CreateMask ();
    }
    catch (...)
    {
      delete m_pData;
      m_pData = 0x0;

      throw;
    }
  }

  return *this;
}

void Switch::DataAdapter::_CreateMask ()
{
  // reset the mask
  uint32_t mask = 0;
  // invert the mask
  mask = ~mask;
  // shift the leading bits off the mask
  mask <<= mr_dataFormat.m_bitOffset;
  // shift the trailing bits off the mask
  mask >>= (32 - mr_dataFormat.m_bitSize);
  // shift the mask into position
  mask <<= (32 - mr_dataFormat.m_bitOffset - mr_dataFormat.m_bitSize);

  // flip the byte order in the mask to make the mask a big-endian array
  // note: the architecture is small-endian; most significant bit has smallest index
  uint8_t* pMask = reinterpret_cast <uint8_t*> (&mask);
  m_mask [0] = pMask [3];
  m_mask [1] = pMask [2];
  m_mask [2] = pMask [1];
  m_mask [3] = pMask [0];
}

/*!
  \brief Gets the data format.

  \return Const reference to the data format.
 */
const Switch::DataAdapter::DataFormat& Switch::DataAdapter::GetDataFormat () const
{
  return mr_dataFormat;
}

/*!
  \brief Sets the data value.

  Extracts the data value from the given data array and sets it in this object's data array.

  \return True if the value has changed, false otherwise.
 */
bool Switch::DataAdapter::SetDataValue (const uint8_t* const i_pData)
{
  // check data validity
  SWITCH_ASSERT_RETURN_1 (0x0 != m_pData, false);
  SWITCH_ASSERT_RETURN_1 (0x0 != i_pData, false);
  if (0x0 == i_pData)
  {
    return false;
  }

  // extract the value into a big endian array
  int32_t inValue = 0;
  uint8_t* pInValue = reinterpret_cast <uint8_t*> (&inValue);
  pInValue [0] = i_pData [3] & m_mask [3];
  pInValue [1] = i_pData [2] & m_mask [2];
  pInValue [2] = i_pData [1] & m_mask [1];
  pInValue [3] = i_pData [0] & m_mask [0];
  inValue >>= (32 - mr_dataFormat.m_bitOffset - mr_dataFormat.m_bitSize);
  inValue += mr_dataFormat.m_minValue;

  // validate the value
  if (!mr_dataFormat.ValidateValue (inValue))
  {
    SWITCH_DEBUG_MSG_2 ("new value for %s not valid %d\n\r", mr_dataFormat.m_name.c_str (), inValue);
    return false;
  }

  // get the initial value
  int32_t initialValue = 0;
  uint8_t* pInitialValue = reinterpret_cast <uint8_t*> (&initialValue);
  pInitialValue [0] = m_pData [3] & m_mask [3];
  pInitialValue [1] = m_pData [2] & m_mask [2];
  pInitialValue [2] = m_pData [1] & m_mask [1];
  pInitialValue [3] = m_pData [0] & m_mask [0];
  initialValue >>= (32 - mr_dataFormat.m_bitOffset - mr_dataFormat.m_bitSize);
  initialValue += mr_dataFormat.m_minValue;
  SWITCH_DEBUG_MSG_2 ("overwriting initial value %d with new value %d\n\r", initialValue, inValue);

  // set the value in the data
  m_pData [0] &= ~m_mask [0];
  m_pData [0] |= (i_pData [0] & m_mask [0]);
  m_pData [1] &= ~m_mask [1];
  m_pData [1] |= (i_pData [1] & m_mask [1]);
  m_pData [2] &= ~m_mask [2];
  m_pData [2] |= (i_pData [2] & m_mask [2]);
  m_pData [3] &= ~m_mask [3];
  m_pData [3] |= (i_pData [3] & m_mask [3]);

  // return whether or not the value has changed
  return (inValue != initialValue);
}

/*!
  \brief Sets the data value.

  Sets the given value in this object's data array.

  \return True if the value has changed, false otherwise.
 */
bool Switch::DataAdapter::SetDataValue (const int32_t& i_value)
{
  // check data validity
  SWITCH_ASSERT_RETURN_1 (0x0 != m_pData, false);

  // validate the value
  if (!mr_dataFormat.ValidateValue (i_value))
  {
    SWITCH_DEBUG_MSG_2 ("new value for %s not valid %d\n\r", mr_dataFormat.m_name.c_str (), i_value);
    return false;
  }

  // extract the value into a big endian char array
  uint32_t scaledValue = static_cast <uint32_t> (i_value - mr_dataFormat.m_minValue);
  scaledValue <<= (32 - mr_dataFormat.m_bitOffset - mr_dataFormat.m_bitSize);
  uint8_t* pInValue = reinterpret_cast <uint8_t*> (&scaledValue);

  // get the initial value from the data array
  uint32_t initialValue = 0;
  uint8_t* pInitialValue = reinterpret_cast <uint8_t*> (&initialValue);
  pInitialValue [0] = m_pData [3] & m_mask [3];
  pInitialValue [1] = m_pData [2] & m_mask [2];
  pInitialValue [2] = m_pData [1] & m_mask [1];
  pInitialValue [3] = m_pData [0] & m_mask [0];
  initialValue >>= (32 - mr_dataFormat.m_bitOffset - mr_dataFormat.m_bitSize);
  SWITCH_DEBUG_MSG_2 ("overwriting initial value %d with new value %d\n\r", (static_cast <int32_t> (initialValue) + mr_dataFormat.m_minValue), i_value);

  // set the value in the data array
  m_pData [0] &= ~m_mask [0];
  m_pData [0] |= (pInValue [3] & m_mask [0]);
  m_pData [1] &= ~m_mask [1];
  m_pData [1] |= (pInValue [2] & m_mask [1]);
  m_pData [2] &= ~m_mask [2];
  m_pData [2] |= (pInValue [1] & m_mask [2]);
  m_pData [3] &= ~m_mask [3];
  m_pData [3] |= (pInValue [0] & m_mask [3]);

  // return whether or not the value has changed
  return (scaledValue != initialValue);
}

/*!
  \brief Gets the data value.

  Gets the given value from this object's data array.
 */
void Switch::DataAdapter::GetDataValue (int32_t& o_value) const
{
  // check data validity
  SWITCH_ASSERT_RETURN_0 (0x0 != m_pData);

  // extract the value from the data array
  uint8_t* pOutValue = reinterpret_cast <uint8_t*> (&o_value);
  pOutValue [0] = m_pData [3] & m_mask [3];
  pOutValue [1] = m_pData [2] & m_mask [2];
  pOutValue [2] = m_pData [1] & m_mask [1];
  pOutValue [3] = m_pData [0] & m_mask [0];

  // allign the bits
  o_value >>= (32 - mr_dataFormat.m_bitOffset - mr_dataFormat.m_bitSize);
  // re-scale to signed integer
  o_value += mr_dataFormat.m_minValue;
}
