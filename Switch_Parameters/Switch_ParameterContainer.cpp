/*?*************************************************************************
*                           Switch_ParameterContainer.cpp
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

#include "Switch_ParameterContainer.h"
#include "Switch_Parameter.h"

// switch includes
#include <Switch_Base/Switch_Debug.h>

// third-party includes
#include <json/value.h>


/*!
  \brief Default constructor.
 */
Switch::ParameterContainer::ParameterContainer ()
{
}

/*!
  \brief Constructor.

  \param [in] i_name        Name of the container.
  \param [in] i_description Description of the container.
 */
Switch::ParameterContainer::ParameterContainer (const std::string& i_name, const std::string& i_description)
{
  m_name        = i_name;
  m_description = i_description;
}

/*!
  \brief Destructor.
 */
Switch::ParameterContainer::~ParameterContainer ()
{
  _Deallocate ();
}

/*!
  \brief Copy constructor.

  \param [in] i_other Object to copy.
 */
Switch::ParameterContainer::ParameterContainer (const ParameterContainer& i_other)
{
  m_name        = i_other.m_name;
  m_description = i_other.m_description;

  // copy all parameter objects behind the pointers
  ParameterList::const_iterator itParameter;
  for (itParameter = i_other.m_parameters.begin (); i_other.m_parameters.end () != itParameter; ++itParameter)
  {
    m_parameters.push_back ((*itParameter)->Clone ());
  }

  // copy all sub-containers
  m_subContainers = i_other.m_subContainers;
}

/*!
  \brief Assignment operator.

  \param [in] i_other Object to assign.

  \return Reference to this object.
 */
Switch::ParameterContainer& Switch::ParameterContainer::operator= (const Switch::ParameterContainer& i_other)
{
  if (this != &i_other)
  {
    // clear all content
    _Deallocate ();

    // copy name and description
    m_name        = i_other.m_name;
    m_description = i_other.m_description;

    // copy all parameter objects behind the pointers
    ParameterList::const_iterator itParameter;
    for (itParameter = i_other.m_parameters.begin (); i_other.m_parameters.end () != itParameter; ++itParameter)
    {
      m_parameters.push_back ((*itParameter)->Clone ());
    }

    // copy all sub-containers
    m_subContainers = i_other.m_subContainers;
  }

  return *this;
}

/*!
  \brief Deallocates all resources.
 */
void Switch::ParameterContainer::_Deallocate ()
{
  // delete all parameter objects behind the pointers
  ParameterList::iterator itParameter;
  for (itParameter = m_parameters.begin (); m_parameters.end () != itParameter; ++itParameter)
  {
    delete *itParameter;
    *itParameter = 0x0;
  }

  // clear all content
  m_parameters.clear ();
  m_subContainers.clear ();
}

/*!
  \brief Gets the name of the parameter container.

  \return The name of the parameter container.
 */
const std::string& Switch::ParameterContainer::GetName () const
{
  return m_name;
}

/*!
  \brief Gets the description of the parameter container.

  \return The description of the parameter container.
 */
const std::string& Switch::ParameterContainer::GetDescription () const
{
  return m_description;
}

/*!
  \brief Gets a list of all parameters in this container.

  \return Const reference to a list of all parameters in this container.
 */
const Switch::ParameterContainer::ParameterList& Switch::ParameterContainer::GetParameters () const
{
  return m_parameters;
}

/*!
  \brief Gets a list of all parameters in this container.

  \return Reference to a list of all parameters in this container.
 */
Switch::ParameterContainer::ParameterList& Switch::ParameterContainer::GetParameters ()
{
  return m_parameters;
}

/*!
  \brief Gets a list of all sub-containers in this container.

  \return Const reference to a list of all sub-containers in this container.
 */
const Switch::ParameterContainer::ContainerList& Switch::ParameterContainer::GetSubContainers () const
{
  return m_subContainers;
}

/*!
  \brief Gets a list of all sub-containers in this container.

  \return Reference to a list of all sub-containers in this container.
 */
Switch::ParameterContainer::ContainerList& Switch::ParameterContainer::GetSubContainers ()
{
  return m_subContainers;
}

/*!
  \brief Sets the name of the container.

  \param [in] i_name The name of the container.
 */
void Switch::ParameterContainer::SetName (const std::string& i_name)
{
  m_name = i_name;
}

/*!
  \brief Sets the description of the container.

  \param [in] i_description The description of the container.
 */
void Switch::ParameterContainer::SetDescription (const std::string& i_description)
{
  m_description = i_description;
}

/*!
  \brief Serializes the object to a JSON value.

  \param [out] o_root Root value of the object as JSON.
 */
void Switch::ParameterContainer::Serialize (Json::Value& o_root) const
{
  o_root ["name"]         = m_name;
  o_root ["description"]  = m_description;

  // serialize the container's parameters
  {
    Json::Value parameterArray (Json::arrayValue);
    Switch::ParameterContainer::ParameterList::const_iterator itParameter;
    for (itParameter = m_parameters.begin (); m_parameters.end () != itParameter; ++itParameter)
    {
      Json::Value parameterValue;
      (*itParameter)->Serialize (parameterValue);
      parameterArray.append (parameterValue);
    }
    o_root ["parameters"] = parameterArray;
  }

  // serialize the container's sub-containers
  {
    Json::Value containerArray (Json::arrayValue);
    Switch::ParameterContainer::ContainerList::const_iterator itContainer;
    for (itContainer = m_subContainers.begin (); m_subContainers.end () != itContainer; ++itContainer)
    {
      Json::Value containerValue;
      itContainer->Serialize (containerValue);
      containerArray.append (containerValue);
    }
    o_root ["subContainers"] = containerArray;
  }
}

/*!
  \brief De-serializes the object from a JSON value.

  \param [in] i_root Root value of the object as JSON.
 */
void Switch::ParameterContainer::Deserialize (const Json::Value& i_root)
{
  // free used resources
  _Deallocate ();

  // de-serialize members
  m_name        = i_root ["name"].       asString ();
  m_description = i_root ["description"].asString ();

  // de-serialize the parameters
  m_parameters.clear ();
  const Json::Value& parameterArray = i_root ["parameters"];
  for (size_t i=0; i<parameterArray.size (); ++i)
  {
    const Json::Value& parameterValue = parameterArray [i];
    Switch::Parameter::eParameterType parameterType
      = static_cast <Switch::Parameter::eParameterType> (parameterValue ["type"].asInt ());

    Switch::Parameter* pParameter = 0x0;
    if (Switch::Parameter::PT_STRING == parameterType)
    {
      pParameter = new Switch::TextParameter <std::string> ();
    }
    else if (Switch::Parameter::PT_INT8 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <int8_t> ();
    }
    else if (Switch::Parameter::PT_UINT8 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <uint8_t> ();
    }
    else if (Switch::Parameter::PT_INT16 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <int16_t> ();
    }
    else if (Switch::Parameter::PT_UINT16 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <uint16_t> ();
    }
    else if (Switch::Parameter::PT_INT32 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <int32_t> ();
    }
    else if (Switch::Parameter::PT_UINT32 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <uint32_t> ();
    }
    else if (Switch::Parameter::PT_INT64 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <int64_t> ();
    }
    else if (Switch::Parameter::PT_UINT64 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <uint64_t> ();
    }
    else if (Switch::Parameter::PT_FLOAT32 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <float> ();
    }
    else if (Switch::Parameter::PT_FLOAT64 == parameterType)
    {
      pParameter = new Switch::ScalarParameter <double> ();
    }

    if (0x0 != pParameter)
    {
      pParameter->Deserialize (parameterValue);
      m_parameters.push_back (pParameter);
    }
  }

  // de-serialize the container's sub-containers
  m_subContainers.clear ();
  const Json::Value& containerArray = i_root ["subContainers"];
  for (size_t i=0; i<containerArray.size (); ++i)
  {
    const Json::Value& containerValue = containerArray [i];
    Switch::ParameterContainer container;
    m_subContainers.push_back (container);
    m_subContainers.back ().Deserialize (containerValue);
  }
}

