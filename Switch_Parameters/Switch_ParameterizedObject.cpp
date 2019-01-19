/*?*************************************************************************
 *                           Switch_ParameterizedObject.cpp
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

#include "Switch_ParameterizedObject.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"
#include "../Switch_Base/Switch_StdLibExtras.h"


/*!
 \brief Default constructor.
 */
Switch::ParameterizedObject::Parameters::Parameters ()
{

}

/*!
 \brief Destructor.
 */
Switch::ParameterizedObject::Parameters::~Parameters ()
{

}

/*!
  \brief Default constructor
  Initializes variables to defaults, use the following methods in the constructor of a derived class
  to correctly configure this object's parameter list:
  - _SetParameterListName         => Sets this object's parameter list name
  - _SetParameterListDescription  => Sets this object's parameter list description
  - _AddParameter                 => Adds a parameter from the object's internal Parameters object to the parameter list
  - _AddParameterList             => Adds a parameterlist from an object owned by this object to the parameter list
  - _OverrideParameter            => Removes a parameter from an object owned by this object
 */
Switch::ParameterizedObject::ParameterizedObject ()
: m_containerName         ("N.A."),
  m_containerDescription  ("N.A."),
  m_vftableAddress        (0x0),
  m_byteSize              (sizeof (m_vftableAddress))
{
}

/*!
  \brief Switch::ParameterizedObject Destructor
 */
Switch::ParameterizedObject::~ParameterizedObject ()
{
  // iterate over the maps and delete all pointers
  // > the parameter links list
  std::list <ParameterLinkBase*>::iterator itParameter;
  for (itParameter = m_parameterLinks.begin (); m_parameterLinks.end () != itParameter; ++itParameter)
  {
    delete (*itParameter);
    (*itParameter) = 0x0;
  }

  m_parameterLinks.clear ();
  m_parameterMap.clear ();

  // > the parameters list map
  std::list <ParameterizedObjectLink*>::iterator itParameterlist;
  for (itParameterlist = m_containerLinks.begin (); m_containerLinks.end () != itParameterlist; ++itParameterlist)
  {
    delete (*itParameterlist);
    (*itParameterlist) = 0x0;
  }

  m_containerLinks.clear ();
  m_containerMap.clear ();
}

/*!
  \brief Checks if the parameterized object contains a link to a parameter with given byte offset.

  \param[in] i_parameterByteOffset Byte offset of the parameter to check if it is contained by the parameterized object.
 */
bool Switch::ParameterizedObject::_ContainsParameterLink (const size_t& i_parameterByteOffset) const
{
  // find the parameter with this byte offset
  std::list <ParameterLinkBase*>::const_iterator itParameter;
  for (itParameter = m_parameterLinks.begin (); m_parameterLinks.end () != itParameter; ++itParameter)
  {
    // check if the iterator's pointer to ParameterLinkBase is not null
    // and if the ParameterLinkBase links the given object (compare by parameter offset)
    if ((0x0 != *itParameter) &&
        ((*itParameter)->GetByteOffset () == i_parameterByteOffset))
    {
      return true;
    }
  }
  return false;
}

/*!
  \brief Checks if the parameterized object contains a link to a container from the given parameterized object.

  \param[in] i_other Parameterized object check if its container is contained by this parameterized object.
 */
bool Switch::ParameterizedObject::_ContainsParameterContainerLink (const Switch::ParameterizedObject& i_other) const
{
  // find the container for the object equal to i_other
  std::list <ParameterizedObjectLink*>::const_iterator itParameterList;
  for (itParameterList = m_containerLinks.begin (); m_containerLinks.end () != itParameterList; ++itParameterList)
  {
    // check if the iterator's pointer to ParameterizedObjectLink is not null
    // and if the ParameterizedObjectLink links the given object (compare by object pointer)
    if ((0x0 != *itParameterList) &&
        (&((*itParameterList)->GetParameterizedObject ()) == &i_other))
    {
      return true;
    }
  }
  return false;
}

/*!
  \brief Constructor.

  \param[in] i_object Parameterized object.
  \param[in] i_listName Unique name for the parameter list of the parameterized object.
 */
Switch::ParameterizedObject::ParameterizedObjectLink::ParameterizedObjectLink (Switch::ParameterizedObject& i_object, const std::string& i_containerName)
: m_parameterizedObject (i_object),
  m_containerName (i_containerName)
{
}

/*!
  \brief Destructor
 */
Switch::ParameterizedObject::ParameterizedObjectLink::~ParameterizedObjectLink ()
{
}

/*!
  \brief Gets the parameters for this link.

  Tells the parameterized object in this link to get its parameters and sets the name of the resulting parameter list.
  \param[out] o_parameters Parameters of this link
 */
void Switch::ParameterizedObject::ParameterizedObjectLink::GetParameters (Switch::ParameterContainer& o_parameters) const
{
  // let the parameterized object save its parameters to a parameter list
  m_parameterizedObject.GetParameters (o_parameters);

  // set the name of the list to some unique value
  o_parameters.SetName (m_containerName);
}

/*!
  \brief Sets the parameters of this link.

  Tells the parameterized object in this link to set the given parameters.
 */
void Switch::ParameterizedObject::ParameterizedObjectLink::SetParameters (const Switch::ParameterContainer& i_parameters) const
{
  // let the parameterized object set its parameters from the parameter container
  m_parameterizedObject.SetParameters (i_parameters);
}

/*!
  \brief Gets a const reference to the parameterized object in the parameter container link.

  \return A const reference to the parameterized object in the parameter container link.
 */
const Switch::ParameterizedObject& Switch::ParameterizedObject::ParameterizedObjectLink::GetParameterizedObject () const
{
  return m_parameterizedObject;
}

/*!
  \brief Gets a reference to the parameterized object in the parameter container link.

  \return A reference to the parameterized object in the parameter container link.
 */
Switch::ParameterizedObject& Switch::ParameterizedObject::ParameterizedObjectLink::GetParameterizedObject ()
{
  return m_parameterizedObject;
}

/*!
  \brief Constructor.

  \param [in] i_byteOffset Amount of bytes between the parameter and the parameters container object (position of the parameter in the parameters object).
  \param [in] i_byteSize Size of the parameter in bytes (the space consumed by the parameter in the parameters object).
 */
Switch::ParameterizedObject::ParameterLinkBase::ParameterLinkBase (const size_t& i_byteOffset, const size_t& i_byteSize)
: m_hidden (false),
  m_byteOffset (i_byteOffset),
  m_byteSize (i_byteSize)
{
}

/*!
  \brief Destructor.
 */
Switch::ParameterizedObject::ParameterLinkBase::~ParameterLinkBase ()
{
}

/*!
  \brief Gets the hidden flag of the parameter link.

  \return The hidden flag of the parameter link. Flag is true if hidden, false if visible.
 */
bool Switch::ParameterizedObject::ParameterLinkBase::GetIsHidden () const
{
  return m_hidden;
}

/*!
  \brief Sets the hidden flag of the parameter link.

  \param [in] i_hide The hidden flag of the parameter link. Flag is true if hidden, false if visible.
 */
void Switch::ParameterizedObject::ParameterLinkBase::SetHidden (const bool& i_hide)
{
  m_hidden = i_hide;
}

/*!
  \brief Gets the byte offset of the parameter linked by this object.

  \return The byte offset of the parameter linked by this object.
 */
size_t Switch::ParameterizedObject::ParameterLinkBase::GetByteOffset () const
{
  return m_byteOffset;
}

/*!
 *  \brief Initializes the native parameter in the parameters container object.
 *
 *  Specialization for std::string parameters.
 *
 *  \param [in, out] i_pData Pointer to the parameters container object.
 *
 *  \remark {Exception safety: no-throw guarantee if io_pData is allocated correctly, undefined behavior otherwise.}
 */
template <>
void Switch::ParameterizedObject::ParameterLink <Switch::TextParameter, std::string>::InitializeData (unsigned char* io_pData)
{
  // initialize the memory as if it were a real string
  std::string initialValue;
  memcpy (io_pData + m_byteOffset, &initialValue, sizeof (std::string));

  // note: the pointer of a default-constructed string does not yet point to memory allocated on the heap
  //       for this reason, there is no need to force the clone to allocate its own memory
}

/*!
  \brief Resets the parameter object in the parameters container object.

  Specialization for string_t parameters.

  \param [in, out] i_pData Pointer to the parameters container object.

  \remark {Exception safety: no-throw guarantee if io_pData is allocated correctly, undefined behavior otherwise.}
 */
template <>
void Switch::ParameterizedObject::ParameterLink <Switch::TextParameter, std::string>::CleanData (unsigned char* io_pData)
{
  // get a reference to the string value
  std::string& string = *reinterpret_cast_ptr <std::string*> (io_pData + m_byteOffset);

  // reset the string and force reallocation for proper cleanup of memory
  string = "";
  string.reserve (0);
}

/*!
  \brief Sets this object's parameter list name
  \param[in] i_listName Name for this object's parameter list
 */
void Switch::ParameterizedObject::_SetContainerName (const std::string& i_name)
{
  m_containerName = i_name;
}

/*!
  \brief Sets this object's parameter list description
  \param[in] i_listDescription Description for this object's parameter list
 */
void Switch::ParameterizedObject::_SetContainerDescription (const std::string& i_description)
{
  m_containerDescription = i_description;
}

/*!
  \brief Adds a parameter list to this object
  \param[in] i_object Object whose parameter list must be nested in this object's parameter list
  \param[in] i_listName Name of the nested parameter list
 */
void Switch::ParameterizedObject::_LinkParameterizedObject (Switch::ParameterizedObject& i_object, const std::string& i_containerName)
{
  SWITCH_ASSERT_RETURN_0 (!_ContainsParameterContainerLink (i_object));
  if (_ContainsParameterContainerLink (i_object))
  {
    return;
  }

  // create the container link
  ParameterizedObjectLink* pParameterizedObjectLink = new ParameterizedObjectLink (i_object, i_containerName);

  // insert it at the end of the list of parameter list links
  std::list <ParameterizedObjectLink*>::iterator itParameterList = m_containerLinks.insert (m_containerLinks.end (), pParameterizedObjectLink);

  // add the iterator to the map
  std::string key = i_containerName;
  m_containerMap [key] = itParameterList;
}

/*!
  \brief Hides a parameter in this object's parameter list.

  \param[in] i_byteOffset Position of the parameter in the parameters container object in bytes.
 */
void Switch::ParameterizedObject::_HideParameter (const size_t& i_byteOffset)
{
  // find the parameter with this byte offset
  std::map <std::string, std::list <ParameterLinkBase*>::iterator>::iterator itParameterMap;
  for (itParameterMap = m_parameterMap.begin (); itParameterMap != m_parameterMap.end (); ++itParameterMap)
  {
    if ((0x0 != *(itParameterMap->second)) &&
        ((*(itParameterMap->second))->GetByteOffset () == i_byteOffset))
    {
      // hide the parameter link
      (*(itParameterMap->second))->SetHidden (true);
      break;
    }
  }
}

/*!
  \brief Hides a parameter in this object's parameter list.

  \param[in] i_name The name of the parameter to hide.
  \param[in] i_group The group of the parameter to hide.
 */
void Switch::ParameterizedObject::_HideParameter (const std::string& i_name, const std::string& i_group)
{
  // create the key
  std::string key = i_name + i_group;

  // find the list with given key
  std::map <std::string, std::list <ParameterLinkBase*>::iterator>::iterator itParameterMap = m_parameterMap.find (key);

  // check if a list link was found
  if (m_parameterMap.end () == itParameterMap)
  {
    return;
  }

  // hide the parameter link
  if (0x0 != *(itParameterMap->second))
  {
    (*(itParameterMap->second))->SetHidden (true);
  }
}


/*!
  \brief Un-hides a parameter in this object's parameter list.

  \param[in] i_byteOffset Position of the parameter in the parameters container object in bytes.
 */
void Switch::ParameterizedObject::_UnhideParameter (const size_t& i_byteOffset)
{
  // find the parameter with this byte offset
  std::map <std::string, std::list <ParameterLinkBase*>::iterator>::iterator itParameterMap;
  for (itParameterMap = m_parameterMap.begin (); itParameterMap != m_parameterMap.end (); ++itParameterMap)
  {
    if ((0x0 != *(itParameterMap->second)) &&
        ((*(itParameterMap->second))->GetByteOffset () == i_byteOffset))
    {
      // un-hide the parameter link
      (*(itParameterMap->second))->SetHidden (false);
      break;
    }
  }
}

/*!
  \brief Un-hides a parameter from this object's parameter list.

  \param[in] i_name The name of the parameter to un-hide.
  \param[in] i_group The group of the parameter to un-hide.
 */
void Switch::ParameterizedObject::_UnhideParameter (const std::string& i_name, const std::string& i_group)
{
  // create the key
  std::string key = i_name + i_group;

  // find the list with given key
  std::map <std::string, std::list <ParameterLinkBase*>::iterator>::iterator itParameterMap = m_parameterMap.find (key);

  // check if a list link was found
  if (m_parameterMap.end () == itParameterMap)
  {
    return;
  }

  // un-hide the parameterlink
  if (0x0 != *(itParameterMap->second))
  {
    (*(itParameterMap->second))->SetHidden (false);
  }
}

/*!
  \brief Removes a nested parameter list from this object's parameter container.

  \param[in] i_object The object whose parameter list to remove.
 */
void Switch::ParameterizedObject::_UnlinkParamterizedObject (Switch::ParameterizedObject& i_object)
{
  // find the parameter container link for the given object
  std::map <std::string, std::list <ParameterizedObjectLink*>::iterator>::iterator itContainerLink;
  for (itContainerLink = m_containerMap.begin (); m_containerMap.end () != itContainerLink; ++itContainerLink)
  {
    // check if the iterator's pointer to ParameterizedObjectLink is not null
    // and if the ParameterizedObjectLink links the given object (compare by object pointer)
    if ((0x0 != (*itContainerLink->second)) &&
        (&((*(itContainerLink->second))->GetParameterizedObject ()) == &i_object))
    {
      // delete the parameter list link
      delete *(itContainerLink->second);
      *(itContainerLink->second) = 0x0;

      // remove the parameter link from the list
      m_containerLinks.erase (itContainerLink->second);

      // erase the mapping
      m_containerMap.erase (itContainerLink);
      break;
    }
  }
}

/*!
  \brief Removes a nested parameter list from this object's parameter container.

  \param[in] i_listName The name of the parameter container to remove.
 */
void Switch::ParameterizedObject::_UnlinkParamterizedObject (const std::string& i_listName)
{
  // find the list with given key
  std::map <std::string, std::list <ParameterizedObjectLink*>::iterator>::iterator itContainerLink = m_containerMap.find (i_listName);

  // check if a list link was found
  if (m_containerMap.end () == itContainerLink)
  {
    return;
  }

  // delete the parameter list link
  if (0x0 != *(itContainerLink->second))
  {
    delete *(itContainerLink->second);
    *(itContainerLink->second) = 0x0;
  }

  // remove the parameter link from the list
  m_containerLinks.erase (itContainerLink->second);

  // erase the mapping
  m_containerMap.erase (itContainerLink);
}

/*!
  \brief Public set parameters method.

  Sets the current parameters as specified in the given Parameters object. Thread-safe.
  Forwards the call to the internal _SetParameters method.

  \param [in] i_parameters Parameters to set.

  \note Never call a public method on self from a private or protected method!
 */
void Switch::ParameterizedObject::SetParameters (const Parameters& i_parameters)
{
  std::lock_guard <std::mutex> parameterLock (m_parameterMutex);

  // go to the safe side
  _SetParameters (i_parameters);
}

/*!
  \brief Public get parameters method.

  Gets the current parameters. Thread-safe. Forwards the call to the internal
  _GetParameters method.

  \param [out] o_parameters Current parameters.

  \note Never call a public method on self from a private or protected method!
 */
void Switch::ParameterizedObject::GetParameters (Parameters& o_parameters) const
{
  std::lock_guard <std::mutex> parameterLock (m_parameterMutex);

  // go to the safe side
  _GetParameters (o_parameters);
}

/*!
  \brief Public get parameters method.

  Gets parameters into a parameter container. Thread-safe. Forwards the call to the internal
  _SaveParamters method.

  \param [out] o_parameters Parameter container with current parameters.

  \note Never call a public method on self from a private or protected method!
 */
void Switch::ParameterizedObject::GetParameters (Switch::ParameterContainer& o_parameters) const
{
  std::lock_guard <std::mutex> parameterLock (m_parameterMutex);

  // save the parameters
  _GetParameters (o_parameters);
}

/*!
  \brief Public set parameters method.

  Sets parameters from a parameter container. Threadsafe. Forwards the call to the internal
  _LoadParamters method.

  \param[in] i_parameterList List with parameters to load.

  \note Never call a public method on self from a private or protected method!
 */
void Switch::ParameterizedObject::SetParameters (const Switch::ParameterContainer& i_parameters)
{
  std::lock_guard <std::mutex> parameterLock (m_parameterMutex);

  // load the given parameters
  _SetParameters (i_parameters);
}

/*!
  \brief Internal save parameters implementation.

  Automatically generates a parameter list, gets the parameters and nested parameter lists
  of owned objects. Overload this method to specify or extend the save parameters functionality.
  Always forward the call to this method or explicitly mention this is not intended!

  Steps taken:\n
  1.) _GetParameters\n
  2.) Generate list from Parameters object\n
  3.) call SaveParameters on other objects

  \param [out] o_parameters Parameter container with current parameters.
 */
void Switch::ParameterizedObject::_GetParameters (Switch::ParameterContainer& o_parameters) const
{
  // set the name of the parameter list
  o_parameters.SetName         (m_containerName);
  o_parameters.SetDescription  (m_containerDescription);

  // check if parameters were added
  // note: if this step is omitted, the fake parameters object will not have a valid vftable and fail to cast
  if (m_parameterLinks.size () > 0)
  {
    Switch::ParameterContainer::ParameterList& outParameterList = o_parameters.GetParameters ();

    // create new array of the correct size
    std::list <ParameterLinkBase*>::const_iterator itParameter;
    unsigned char* pParameterArray = new unsigned char [m_byteSize];
    memset (pParameterArray, 0, m_byteSize);
    for (itParameter = m_parameterLinks.begin (); m_parameterLinks.end () != itParameter; ++itParameter)
    {
      (*itParameter)->InitializeData (pParameterArray);
    }

    // set first bytes equal to pointer to virtual function table
    reinterpret_cast_ptr <uintptr_t*> (pParameterArray) [0] = m_vftableAddress;

    // cast to parameters object and get the parameters
    _GetParameters (*reinterpret_cast_ptr <Parameters*> (pParameterArray));

    // loop over all parameter links
    for (itParameter = m_parameterLinks.begin (); m_parameterLinks.end () != itParameter; ++itParameter)
    {
      if (!(*itParameter)->GetIsHidden ())
      {
        (*itParameter)->UpdateValue (pParameterArray);
        outParameterList.push_back ((*itParameter)->ToParameter ().Clone ());
      }
    }

    // cleanup
    for (itParameter = m_parameterLinks.begin (); m_parameterLinks.end () != itParameter; ++itParameter)
    {
      (*itParameter)->CleanData (pParameterArray);
    }
    delete [] pParameterArray;
    pParameterArray = 0x0;
  }

  // check if parameter lists were added
  if (m_containerLinks.size () > 0)
  {
    Switch::ParameterContainer::ContainerList& outContainerList = o_parameters.GetSubContainers ();

    // next add the parameter lists of all the objects
    std::list <ParameterizedObjectLink*>::const_iterator itParameterizedObject;
    for (itParameterizedObject = m_containerLinks.begin (); m_containerLinks.end () != itParameterizedObject; ++itParameterizedObject)
    {
      Switch::ParameterContainer nestedContainer;
      (*itParameterizedObject)->GetParameters (nestedContainer);
      outContainerList.push_back (nestedContainer);
    }
  }
}

/*!
  \brief Internal set parameters implementation.

  Automatically parses parameters from a parameter container, sets the parameters and forwards
  nested parameter containers to other objects. Overload this method to specify or extend the
  set parameters functionality. Always forward the call to this method or explicitly mention
  this is not intended!

  Steps taken:
  1.) _GetParameters
  2.) Parse list into Parameters object
  3.) _SetParameters
  4.) call SetParameters on other objects

  \param [in] i_parameters Container with parameters
 */
void Switch::ParameterizedObject::_SetParameters (const Switch::ParameterContainer& i_parameters)
{
  unsigned char* pParameterArray = 0x0;

  try
  {
    // note: validate all parameters

    // check if parameters were added
    // note: if this step is omitted, the fake parameters object will not have a valid vftable and fail to cast
    if (m_parameterMap.size () > 0)
    {
      // create new array of the correct size
      std::list <ParameterLinkBase*>::const_iterator itParameter;
      pParameterArray = new unsigned char [m_byteSize];
      memset (pParameterArray, 0, m_byteSize);
      for (itParameter = m_parameterLinks.begin (); m_parameterLinks.end () != itParameter; ++itParameter)
      {
        (*itParameter)->InitializeData (pParameterArray);
      }

      // set first bytes equal to pointer to virtual function table
      reinterpret_cast_ptr <uintptr_t*> (pParameterArray) [0] = m_vftableAddress;

      // cast to parameters object and get the parameters
      _GetParameters (*reinterpret_cast_ptr <Parameters*> (pParameterArray));

      // prepare to iterate through the input parameters
      const Switch::ParameterContainer::ParameterList& inParameterList = i_parameters.GetParameters ();
      Switch::ParameterContainer::ParameterList::const_iterator inParameterIterator;

      // prepare to iterate through the parameter map
      std::string key;
      std::map <std::string, std::list <ParameterLinkBase*>::iterator>::iterator itParameterLink;

      for (inParameterIterator=inParameterList.begin (); inParameterList.end ()!=inParameterIterator; ++inParameterIterator)
      {
        // dereference the iterator
        const Switch::Parameter* pInParameter = *inParameterIterator;

        // construct the key of the parameter used to search for its corresponding parameter
        key = pInParameter->GetName () + pInParameter->GetGroup ();

        // find the corresponding parameter in the map
        itParameterLink = m_parameterMap.find (key);

        if (itParameterLink != m_parameterMap.end ())
        {
          if (!(*(itParameterLink->second))->GetIsHidden ())
          {
            // retrieve the parameter value
            (*(itParameterLink->second))->ExtractValue (pParameterArray, *pInParameter);
          }
        }
      }

      // cast to parameters object and set the parameters
      _SetParameters (*reinterpret_cast_ptr <Parameters*> (pParameterArray));

      // cleanup
      for (itParameter = m_parameterLinks.begin (); m_parameterLinks.end () != itParameter; ++itParameter)
      {
        (*itParameter)->CleanData (pParameterArray);
      }
      delete [] pParameterArray;
      pParameterArray = 0x0;
    }

    // check if parameterlists were added
    if (m_containerMap.size () > 0)
    {
      // prepare to iterate through the input containers
      const Switch::ParameterContainer::ContainerList& inContainerList = i_parameters.GetSubContainers ();
      Switch::ParameterContainer::ContainerList::const_iterator inContainerIterator;

      // prepare to iterate through the parameterlist map
      std::map <std::string, std::list <ParameterizedObjectLink*>::iterator>::iterator itParameterlist;

      for (inContainerIterator=inContainerList.begin (); inContainerList.end ()!=inContainerIterator; ++inContainerIterator)
      {
        // dereference the container
        const Switch::ParameterContainer& inContainer = *inContainerIterator;

        // construct the key of the parameter list used to search for its corresponding object
        std::string key = inContainer.GetName ();

        // find the corresponding object in the map
        itParameterlist = m_containerMap.find (key);

        if (itParameterlist != m_containerMap.end ())
        {
          // load the parameter list for this object
          (*(itParameterlist->second))->SetParameters (inContainer);
        }
      }
    }
  }
  catch (...)
  {
    // cleanup
    delete [] pParameterArray;
    pParameterArray = 0x0;

    // re-throw
    throw;
  }
}
