/*?*************************************************************************
*                           Switch_ApplicationModule.cpp
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

#include "Switch_ApplicationModule.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"


/*!
  \brief Default constructor.
 */
Switch::ApplicationModule::Parameters::Parameters ()
{
}

/*!
  \brief Destructor.
 */
Switch::ApplicationModule::Parameters::~Parameters ()
{
}

/*!
  \brief Default constructor.
 */
Switch::ApplicationModule::ApplicationModule ()
: Switch::StatedObject (),
  Switch::ParameterizedObject ()
{
}

/*!
  \brief Destructor.
 */
Switch::ApplicationModule::~ApplicationModule ()
{
}

/*!
  \brief Sets the parameters of the parameterized object.
  
  \param [in] i_parameters Parameters to set in the parameterized object.
  
  \note The object must be STOPPED.
 */
void Switch::ApplicationModule::SetParameters (const Switch::ParameterizedObject::Parameters& i_parameters)
{
  std::lock_guard <std::mutex> stateLock (m_stateMutex);
  
  SWITCH_ASSERT (OS_STOPPED == m_objectState);
  if (OS_STOPPED != m_objectState)
  {
    throw std::runtime_error ("object must be STOPPED to set parameters");
  }
  
  Switch::ParameterizedObject::SetParameters (i_parameters);
}

/*!
  \brief Sets the parameters of the parameterized object.
  
  \param [in] i_parameters Parameters to set in the parameterized object.
  
  \note The object must be STOPPED.
 */
void Switch::ApplicationModule::SetParameters (const Switch::ParameterContainer& i_parameters)
{
  std::lock_guard <std::mutex> stateLock (m_stateMutex);
  
  SWITCH_ASSERT (OS_STOPPED == m_objectState);
  if (OS_STOPPED != m_objectState)
  {
    throw std::runtime_error ("object must be STOPPED to set parameters");
  }
  
  Switch::ParameterizedObject::SetParameters (i_parameters);
}
