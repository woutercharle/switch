/*?*************************************************************************
*                           Switch_Parameter.cpp
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

#include "Switch_Parameter.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"

// thirdparty includes


/*!
  \brief Default constructor.
 */
Switch::Parameter::Parameter ()
: m_type (PT_NOT_A_TYPE)
{
}

/*!
  \brief Constructor.

  \param [in] i_name        Name of the parameter.
  \param [in] i_description Description of the parameter.
  \param [in] i_group       Parameter group or subset this parameter belongs to.
 */
Switch::Parameter::Parameter (const std::string& i_name, const std::string& i_description, const std::string& i_group)
: m_type (PT_NOT_A_TYPE)
{
  // set descriptors
  m_name        = i_name;
  m_description = i_description;
  m_group       = i_group;
}

/*!
  \brief Destructor
 */
Switch::Parameter::~Parameter ()
{
}

/*!
  \brief Gets the name of the parameter.

  \return The name of the parameter.
 */
const std::string& Switch::Parameter::GetName () const
{
  return m_name;
}

/*!
  \brief Gets the group of the parameter.

  \return The group of the parameter.
 */
const std::string& Switch::Parameter::GetGroup () const
{
  return m_group;
}

/*!
  \brief Gets the description of the parameter.

  \return The description of the parameter.
 */
const std::string& Switch::Parameter::GetDescription () const
{
  return m_description;
}

/*!
  \brief Gets the type of the parameter.

  \return The type of the parameter.
 */
const Switch::Parameter::eParameterType& Switch::Parameter::GetType () const
{
  return m_type;
}

//************************ Scalar Parameters ************************//

#define DEFINE_SCALAR_PARAMETER(scalar_type,scalar_type_enum)   \
namespace Switch                                                \
{                                                               \
  template <>                                                   \
  ScalarParameter <scalar_type>::ScalarParameter                \
  (                                                             \
    const std::string& i_name,                                  \
    const std::string& i_description,                           \
    const std::string& i_group                                  \
  )                                                             \
  : Switch::Parameter (i_name, i_description, i_group),         \
    m_value ()                                                  \
  {                                                             \
    m_type = scalar_type_enum;                                  \
    m_validationCriteria.push_back                              \
    (                                                           \
      std::make_pair                                            \
      (                                                         \
        std::numeric_limits <scalar_type>::lowest (),           \
        std::numeric_limits <scalar_type>::max ()               \
      )                                                         \
    );                                                          \
  }                                                             \
}

DEFINE_SCALAR_PARAMETER (int8_t,    PT_INT8);
DEFINE_SCALAR_PARAMETER (uint8_t,   PT_UINT8);
DEFINE_SCALAR_PARAMETER (int16_t,   PT_INT8);
DEFINE_SCALAR_PARAMETER (uint16_t,  PT_UINT16);
DEFINE_SCALAR_PARAMETER (int32_t,   PT_INT32);
DEFINE_SCALAR_PARAMETER (uint32_t,  PT_UINT32);
DEFINE_SCALAR_PARAMETER (int64_t,   PT_INT64);
DEFINE_SCALAR_PARAMETER (uint64_t,  PT_UINT64);
DEFINE_SCALAR_PARAMETER (float,     PT_FLOAT32);
DEFINE_SCALAR_PARAMETER (double,    PT_FLOAT64);


//************************ Text Parameters ************************//

#define DEFINE_TEXT_PARAMETER(text_type,text_type_enum)         \
namespace Switch                                                \
{                                                               \
  template <>                                                   \
  TextParameter <text_type>::TextParameter                      \
  (                                                             \
    const std::string& i_name,                                  \
    const std::string& i_description,                           \
    const std::string& i_group                                  \
  )                                                             \
  : Switch::Parameter (i_name, i_description, i_group),         \
    m_value ()                                                  \
  {                                                             \
    m_type = text_type_enum;                                    \
  }                                                             \
}

DEFINE_TEXT_PARAMETER (std::string, PT_STRING);
