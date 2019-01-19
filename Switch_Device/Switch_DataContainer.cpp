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

#include "Switch_DataContainer.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"

// third-party includes
#include <json/json.h>


/*!
  \brief Default constructor.
 */
Switch::DataContainer::DataFormat::DataFormat ()
{
  m_name        = "N.A.";
  m_description = "N.A.";
}

/*!
  \brief Constructor.

  \param [in] i_name        Name of the data object.
  \param [in] i_description Description of the data object.
 */
Switch::DataContainer::DataFormat::DataFormat (const std::string& i_name, const std::string& i_description)
{
  // set descriptors
  m_name        = i_name;
  m_description = i_description;
}

/*!
  \brief Destructor.
 */
Switch::DataContainer::DataFormat::~DataFormat ()
{
}

/*!
 * \brief Copy constructor.
 *
 * \param [in] i_other Object to copy.
 */
Switch::DataContainer::DataFormat::DataFormat (const Switch::DataContainer::DataFormat& i_other)
{
  m_name              = i_other.m_name;
  m_description       = i_other.m_description;
  m_nestedAdapters    = i_other.m_nestedAdapters;
  m_nestedContainers  = i_other.m_nestedContainers;
}

/*!
 * \brief Assignment operator.
 *
 * \param [in] i_other Object to assign.
 *
 * \return Reference to this object.
 */
Switch::DataContainer::DataFormat& Switch::DataContainer::DataFormat::operator= (const Switch::DataContainer::DataFormat& i_other)
{
  if (this != &i_other)
  {
    m_name              = i_other.m_name;
    m_description       = i_other.m_description;
    m_nestedAdapters    = i_other.m_nestedAdapters;
    m_nestedContainers  = i_other.m_nestedContainers;
  }

  return *this;
}

/*!
 \brief Equality comparator.

 \param [in] i_other Object to compare to.

 \return True if the compared object is equal, false otherwise.
 */
bool Switch::DataContainer::DataFormat::operator== (const Switch::DataContainer::DataFormat& i_other) const
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
  if (m_nestedAdapters != i_other.m_nestedAdapters)
  {
    return false;
  }
  if (m_nestedContainers != i_other.m_nestedContainers)
  {
    return false;
  }

  return true;
}

/*!
 \brief Inequality comparator.

 \param [in] i_other Object to compare to.

 \return True if the compared object is inequal, false otherwise.
 */
bool Switch::DataContainer::DataFormat::operator!= (const Switch::DataContainer::DataFormat& i_other) const
{
  return !(this->operator== (i_other));
}

/*!
  \brief Gets the total size of the data in the container in bits.

  \return The total size of the data in the container in bits.
 */
uint32_t Switch::DataContainer::DataFormat::GetTotalBitSize () const
{
  // initialize counter
  uint32_t totalNrBits = 0;

  // add size of each adapter
  std::list <Switch::DataAdapter::DataFormat>::const_iterator adapterIterator;
  for (adapterIterator=m_nestedAdapters.begin (); m_nestedAdapters.end ()!=adapterIterator; ++adapterIterator)
  {
    totalNrBits += adapterIterator->m_bitSize;
  }

  // add size of each container
  std::list <DataFormat>::const_iterator containerIterator;
  for (containerIterator=m_nestedContainers.begin (); m_nestedContainers.end ()!=containerIterator; ++containerIterator)
  {
    totalNrBits += containerIterator->GetTotalBitSize ();
  }

  return totalNrBits;
}

/*!
  \brief Serializes the object to a JSON value.

  \param [out] o_root Root value of the object as JSON.
 */
void Switch::DataContainer::DataFormat::Serialize (Json::Value& o_root) const
{
  o_root ["name"]         = m_name;
  o_root ["description"]  = m_description;

  // serialize the container's adapters
  Json::Value adapterArray (Json::arrayValue);
  std::list <Switch::DataAdapter::DataFormat>::const_iterator itAdapter;
  for (itAdapter = m_nestedAdapters.begin (); m_nestedAdapters.end () != itAdapter; ++itAdapter)
  {
    Json::Value adapterValue;
    itAdapter->Serialize (adapterValue);
    adapterArray.append (adapterValue);
  }
  o_root ["adapters"] = adapterArray;

  // serialize the container's sub-containers
  Json::Value containerArray (Json::arrayValue);
  std::list <Switch::DataContainer::DataFormat>::const_iterator itContainer;
  for (itContainer = m_nestedContainers.begin (); m_nestedContainers.end () != itContainer; ++itContainer)
  {
    Json::Value containerValue;
    itContainer->Serialize (containerValue);
    containerArray.append (containerValue);
  }
  o_root ["subContainers"] = containerArray;

# ifdef SWITCH_SERIALIZE_WITH_COMMENTS
  // add comments
  o_root ["name"].          setComment ("// The name of the container object.", Json::commentAfterOnSameLine);
  o_root ["description"].   setComment ("// A description of the content of the container object.", Json::commentAfterOnSameLine);
  o_root ["adapters"].      setComment ("// Array of data formats of data adapters contained by this container.", Json::commentAfterOnSameLine);
  o_root ["subContainers"]. setComment ("// Array of data formats of nested data containers.", Json::commentAfterOnSameLine);
# endif
}

/*!
  \brief De-serializes the object from a JSON value.

  \param [in] i_root Root value of the object as JSON.
 */
void Switch::DataContainer::DataFormat::Deserialize (const Json::Value& i_root)
{
  // deserialize members
  m_name        = i_root ["name"].        asString ();
  m_description = i_root ["description"]. asString ();

  // de-serialize the container's adapters
  m_nestedAdapters.clear ();
  const Json::Value& adapterArray = i_root ["adapters"];
  for (size_t i=0; i<adapterArray.size (); ++i)
  {
    const Json::Value& adapterValue = adapterArray [i];
    Switch::DataAdapter::DataFormat dataFormat;
    m_nestedAdapters.push_back (dataFormat);
    m_nestedAdapters.back ().Deserialize (adapterValue);
  }

  // de-serialize the container's sub-containers
  m_nestedContainers.clear ();
  const Json::Value& containerArray = i_root ["subContainers"];
  for (size_t i=0; i<containerArray.size (); ++i)
  {
    const Json::Value& containerValue = containerArray [i];
    Switch::DataContainer::DataFormat dataFormat;
    m_nestedContainers.push_back (dataFormat);
    m_nestedContainers.back ().Deserialize (containerValue);
  }
}

/*!
  \brief Constructor

  \param [in] i_dataFormat The data format of the data adapter
 */
Switch::DataContainer::DataContainer (const Switch::DataContainer::DataFormat& i_dataFormat)
: mr_dataFormat   (i_dataFormat),
  m_ownsData      (true),
  m_pData         (0x0),
  m_startAddress  (0)
{
  try
  {
    // compute the total data size
    uint32_t totalByteSize = (mr_dataFormat.GetTotalBitSize () + 7) / 8;

    // allocate data
    m_pData = new uint8_t [totalByteSize];

    // create new data adapters and nested data containers
    _GenerateNestedObjectsFromDataFormat ();
  }
  catch (...)
  {
    // cleanup
    m_nestedDataAdapters.clear ();
    m_nestedDataContainers.clear ();

    delete [] m_pData;
    m_pData = 0x0;

    // rethrow
    throw;
  }
}

/*!
  \brief Constructor

  \param [in] i_pData         Access to the underlying data buffer, shared with this data adapter
  \param [in] i_startAddress  Address of the first element of the container.
  \param [in] i_dataFormat    The data format of the data adapter
 */
Switch::DataContainer::DataContainer (uint8_t* i_pData, const uint32_t& i_startAddress, const DataFormat& i_dataFormat)
: mr_dataFormat   (i_dataFormat),
  m_ownsData      (false),
  m_pData         (i_pData),
  m_startAddress  (i_startAddress)
{
  // create new data adapters and nested data containers
  _GenerateNestedObjectsFromDataFormat ();
}

/*!
  \brief Destructor
 */
Switch::DataContainer::~DataContainer ()
{
  m_nestedDataAdapters.clear ();
  m_nestedDataContainers.clear ();

  if (m_ownsData)
  {
    delete [] m_pData;
    m_pData = 0x0;
  }
}

/*!
 \brief Copy constructor.

 \param [in] i_other Object to copy.
 */
Switch::DataContainer::DataContainer (const DataContainer& i_other)
: mr_dataFormat   (i_other.mr_dataFormat),
  m_ownsData      (false),
  m_pData         (0x0),
  m_startAddress  (0)
{
  try
  {
    if (i_other.m_ownsData)
    {
      // compute the total data size and allocate data
      m_ownsData              = true;
      uint32_t totalByteSize  = (mr_dataFormat.GetTotalBitSize () + 7) / 8;
      m_pData                 = new uint8_t [totalByteSize];
      memcpy (m_pData, i_other.m_pData, totalByteSize);
    }
    else
    {
      m_ownsData      = false;
      m_pData         = i_other.m_pData;
      m_startAddress  = i_other.m_startAddress;
    }

    // create new data adapters and nested data containers
    _GenerateNestedObjectsFromDataFormat ();
  }
  catch (...)
  {
    // cleanup
    m_nestedDataAdapters.clear ();
    m_nestedDataContainers.clear ();

    if (m_ownsData)
    {
      delete [] m_pData;
      m_pData = 0x0;
    }

    // rethrow
    throw;
  }
}

/*!
  \brief Assignment operator.

  \param [in] i_other Object to assign.

  \return Reference to this object.

  \note Only objects with same data format can be assigned to each-other.
 */
Switch::DataContainer& Switch::DataContainer::operator= (const DataContainer& i_other)
{
  if (this != &i_other)
  {
    SWITCH_ASSERT (mr_dataFormat == i_other.mr_dataFormat);

    // cleanup
    m_nestedDataAdapters.clear ();
    m_nestedDataContainers.clear ();

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
        // compute the total data size and allocate data
        m_ownsData = true;
        uint32_t totalByteSize = (mr_dataFormat.GetTotalBitSize () + 7) / 8;
        m_pData = new uint8_t [totalByteSize];
        memcpy (m_pData, i_other.m_pData, totalByteSize);
        m_startAddress = 0x0;
      }
      else
      {
        m_ownsData = false;
        m_pData         = i_other.m_pData;
        m_startAddress  = i_other.m_startAddress;
      }

      // create new data adapters and nested data containers
      _GenerateNestedObjectsFromDataFormat ();
    }
    catch (...)
    {
      // cleanup
      m_nestedDataAdapters.clear ();
      m_nestedDataContainers.clear ();

      if (m_ownsData)
      {
        delete [] m_pData;
        m_pData = 0x0;
      }

      // rethrow
      throw;
    }
  }

  return *this;
}

/*!
  \brief Parses the data format and generates nested objects.

  \note If the nested object lists are non-empty, they are cleared first.
 */
void Switch::DataContainer::_GenerateNestedObjectsFromDataFormat ()
{
  SWITCH_ASSERT_RETURN_0 (0x0 != m_pData);

  // clear the nested object lists
  m_nestedDataAdapters.clear ();
  m_nestedDataContainers.clear ();

  // initialization of helper variables
  uint32_t totalBits = 0;
  uint32_t bitOffset = 0;
  if (!mr_dataFormat.m_nestedAdapters.empty ())
  {
    bitOffset = mr_dataFormat.m_nestedAdapters.begin ()->m_bitOffset;
  }

  // iterate over all nested data adapters
  std::list <Switch::DataAdapter::DataFormat>::const_iterator adapterIterator;
  for (adapterIterator=mr_dataFormat.m_nestedAdapters.begin (); mr_dataFormat.m_nestedAdapters.end ()!=adapterIterator; ++adapterIterator)
  {
    m_nestedDataAdapters.push_back (Switch::DataAdapter (&m_pData [(totalBits + bitOffset)/8], *adapterIterator));
    totalBits += adapterIterator->m_bitSize;
  }

  // iterate over all nested data containers
  std::list <Switch::DataContainer::DataFormat>::const_iterator containerIterator;
  for (containerIterator=mr_dataFormat.m_nestedContainers.begin (); mr_dataFormat.m_nestedContainers.end ()!=containerIterator; ++containerIterator)
  {
    m_nestedDataContainers.push_back (Switch::DataContainer (&m_pData [(totalBits + bitOffset)/8], m_startAddress + totalBits, *containerIterator));
    totalBits += containerIterator->GetTotalBitSize ();
  }
}

/*!
  \brief Gets the data format.

  \return Const reference to the data format.
 */
const Switch::DataContainer::DataFormat& Switch::DataContainer::GetDataFormat () const
{
  return mr_dataFormat;
}

/*!
  \brief Sets the value of a selection of elements.

  \param [in,out] io_changedElements List of elements whose value has changed.
  \param [in] i_elements List of elements for which to set the value.

  \note The list must be sorted on element address.

  \return True if the value of one or more elements has changed.
 */
bool Switch::DataContainer::SetElements (std::list <Switch::DataContainer::Element>& io_changedElements, const std::list <Switch::DataContainer::Element>& i_elements)
{
  // ensure that the list of elements is non-empty
  if (0 == i_elements.size ())
  {
    return false;
  }

  // prepare to iterator over the elements
  bool adapterValueChanged;
  bool valueChanged = false;
  std::list <Switch::DataContainer::Element>::const_iterator elementIterator = i_elements.begin ();

  // iterate over all nested adapters
  uint32_t currentAddress = m_startAddress;
  std::list <Switch::DataAdapter>::iterator adapterIterator;
  for (adapterIterator=m_nestedDataAdapters.begin ();
       (m_nestedDataAdapters.end () !=adapterIterator) && (i_elements.end ()!=elementIterator);
       ++adapterIterator)
  {
    // ensure that the element's address is beyond the address of the current element
    SWITCH_ASSERT (currentAddress <= elementIterator->m_address);
    if (elementIterator->m_address < currentAddress)
    {
      break;
    }

    // check if the element address maches the current address
    if (elementIterator->m_address == currentAddress)
    {
      if (elementIterator->m_magicNumber != adapterIterator->GetDataFormat ().ComputeMagicNumber (elementIterator->m_address))
      {
        SWITCH_DEBUG_MSG_0 ("invalid magic number\n");
        continue;
      }

      // set the value in the data adapter
      adapterValueChanged = adapterIterator->SetDataValue (elementIterator->m_value);
      if (adapterValueChanged)
      {
        io_changedElements.push_back (*elementIterator);
      }
      valueChanged |= adapterValueChanged;
      ++elementIterator;
    }

    // update to the next adapter address
    currentAddress += adapterIterator->GetDataFormat ().m_bitSize;
  }

  // stop if all elements have been handled
  if (i_elements.end ()==elementIterator)
  {
    return valueChanged;
  }

  // iterate over all nested containers
  std::list <Switch::DataContainer>::iterator containerIterator;
  uint32_t addressUpperBoundary;
  for (containerIterator=m_nestedDataContainers.begin ();
       (m_nestedDataContainers.end () !=containerIterator) && (i_elements.end ()!=elementIterator);
       ++containerIterator)
  {
    // ensure that the element's address is beyond the address of the current container
    SWITCH_ASSERT (m_startAddress <= elementIterator->m_address);
    if (elementIterator->m_address < currentAddress)
    {
      break;
    }

    // get address boundaries of the current container
    addressUpperBoundary = currentAddress + containerIterator->GetDataFormat ().GetTotalBitSize ();

    // check if the element belongs to the current container
    if (elementIterator->m_address<addressUpperBoundary)
    {
      // create a new list of all elements belonging to the current container
      std::list <Switch::DataContainer::Element> containerElements;
      for (; (i_elements.end ()!=elementIterator) && (elementIterator->m_address<addressUpperBoundary); ++elementIterator)
      {
        containerElements.push_back (*elementIterator);
      }

      // set the elements in the current container
      valueChanged |= containerIterator->SetElements (io_changedElements, containerElements);
    }

    // update to the next container address
    currentAddress = addressUpperBoundary;
  }

  // return if the value of the data in this container changed
  return valueChanged;
}

/*!
  \brief Gets all elements.

  \param [in, out] i_elements List of all elements.
 */
void Switch::DataContainer::GetElements (std::list <Switch::DataContainer::Element>& io_elements) const
{
  // iterate over all nested adapters
  uint32_t currentAddress = m_startAddress;
  std::list <Switch::DataAdapter>::const_iterator adapterIterator;
  for (adapterIterator=m_nestedDataAdapters.begin (); m_nestedDataAdapters.end () !=adapterIterator; ++adapterIterator)
  {
    // get the data format of the current adapter
    const Switch::DataAdapter::DataFormat& adapterDataFormat = adapterIterator->GetDataFormat ();

    // translate the adapter to an element
    Element adapterElement;
    adapterElement.m_address      = currentAddress;
    adapterElement.m_magicNumber  = adapterDataFormat.ComputeMagicNumber (currentAddress);
    adapterIterator->GetDataValue (adapterElement.m_value);
    io_elements.push_back (adapterElement);

    // update the address for the next element
    currentAddress += adapterDataFormat.m_bitSize;
  }

  // iterate over all nested containers
  std::list <Switch::DataContainer>::const_iterator containerIterator;
  for (containerIterator=m_nestedDataContainers.begin (); m_nestedDataContainers.end () !=containerIterator; ++containerIterator)
  {
    // let the current container add its elements
    containerIterator->GetElements (io_elements);
  }
}

/*!
  \brief Sets the content of the container.

  \param [in,out] io_changedElements List of elements whose value has changed.
  \param [in] i_pData Data buffer containing the content to set.

  \return True if the content has changed, false otherwise.
 */
bool Switch::DataContainer::SetContent (std::list <Element>& io_changedElements, const uint8_t* const i_pData)
{
  // initialization of helper variables
  uint32_t totalBits = 0;
  if (!mr_dataFormat.m_nestedAdapters.empty ())
  {
    totalBits = mr_dataFormat.m_nestedAdapters.begin ()->m_bitOffset;
  }
  bool adapterDataChanged;
  bool dataChanged = false;

  // iterate over all nested data adapters
  std::list <Switch::DataAdapter>::iterator adapterIterator;
  for (adapterIterator=m_nestedDataAdapters.begin (); m_nestedDataAdapters.end ()!=adapterIterator; ++adapterIterator)
  {
    const Switch::DataAdapter::DataFormat& adapterDataFormat = adapterIterator->GetDataFormat ();

    adapterDataChanged = adapterIterator->SetDataValue (&i_pData [totalBits/8]);
    if (adapterDataChanged)
    {
      // add an element for the adapter to the list of changed elements
      Element adapterElement;
      adapterElement.m_address      = m_startAddress - m_startAddress%8  + totalBits;
      adapterElement.m_magicNumber  = adapterDataFormat.ComputeMagicNumber (adapterElement.m_address);
      adapterIterator->GetDataValue (adapterElement.m_value);
      io_changedElements.push_back (adapterElement);

      // note: the address of the element is computed against the beginning of the up-most parent of the container
      //       while i_pData starts on the same byte as this data container
    }
    dataChanged |= adapterDataChanged;
    totalBits += adapterDataFormat.m_bitSize;
  }

  // iterate over all nested data containers
  std::list <Switch::DataContainer>::iterator containerIterator;
  for (containerIterator=m_nestedDataContainers.begin (); m_nestedDataContainers.end ()!=containerIterator; ++containerIterator)
  {
    dataChanged |= containerIterator->SetContent (io_changedElements, &i_pData [totalBits/8]);
    totalBits += containerIterator->GetDataFormat ().GetTotalBitSize ();
  }

  return dataChanged;
}

/*!
  \brief Gets the content of the container.

  \param [in] o_pData Data buffer that must be filled with the content of the container.

  \note Can only be called on the data container that owns its own data buffer.
 */
void Switch::DataContainer::GetContent (uint8_t* const o_pData) const
{
  // ensure that the method is called on the container owning the data
  SWITCH_ASSERT (m_ownsData);
  if (!m_ownsData)
  {
    return;
  }

  // get the total size in bits
  uint32_t totalBitSize = mr_dataFormat.GetTotalBitSize ();

  // copy the data buffer
  memcpy (o_pData, m_pData, static_cast <uint32_t> ((totalBitSize + 7)/8));
}
