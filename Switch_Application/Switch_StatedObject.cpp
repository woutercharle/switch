/*?*************************************************************************
*                           Switch_StatedObject.cpp
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

#include "Switch_StatedObject.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"

/*!
  \brief Default constructor.
 */
Switch::StatedObject::StatedObject ()
: m_objectState (OS_STOPPED)
{
}

/*!
  \brief Destructor.
 */
Switch::StatedObject::~StatedObject ()
{
}

/*!
  \brief Gets the object's state.

  \return The object's state.
 */
Switch::StatedObject::eObjectState Switch::StatedObject::GetState () const
{
  return m_objectState;
}

/*!
  \brief Prepares the object.

  Moves the object from the STOPPED state to the READY state.

  \note The object's state must be STOPPED.
 */
void Switch::StatedObject::Prepare ()
{
  std::lock_guard <std::mutex> stateLock (m_stateMutex);

  SWITCH_ASSERT (OS_STOPPED == m_objectState);
  if (OS_STOPPED != m_objectState)
  {
    if (OS_READY == m_objectState)
    {
      return;
    }
    else
    {
      throw std::runtime_error ("object is STARTED");
    }
  }

  _Prepare ();

  m_objectState = OS_READY;
}

/*!
  \brief Starts the object.

  Moves the object from the READY state to the STARTED state.

  \note The object's state must be READY.
 */
void Switch::StatedObject::Start ()
{
  std::lock_guard <std::mutex> stateLock (m_stateMutex);

  SWITCH_ASSERT (OS_READY == m_objectState);
  if (OS_READY != m_objectState)
  {
    if (OS_STARTED == m_objectState)
    {
      return;
    }
    else
    {
      throw std::runtime_error ("object is STOPPED");
    }
  }

  _Start ();

  m_objectState = OS_STARTED;
}

/*!
  \brief Pauses the object.

  Moves the object from the STARTED state to the READY state.

  \note The object's state must be STARTED.
 */
void Switch::StatedObject::Pause () noexcept
{
  std::lock_guard <std::mutex> stateLock (m_stateMutex);

  if (OS_STARTED != m_objectState)
  {
    return;
  }

  _Pause ();

  m_objectState = OS_READY;
}

/*!
  \brief Stops the object.

  Moves the object from the STARTED or the READY state to the STOPPED state.

  \note The object's state must be STARTED or READY.
 */
void Switch::StatedObject::Stop () noexcept
{
  std::lock_guard <std::mutex> stateLock (m_stateMutex);

  if (OS_STOPPED == m_objectState)
  {
    return;
  }
  else if (OS_STARTED == m_objectState)
  {
    _Pause ();

    m_objectState = OS_READY;
  }

  _Stop ();

  m_objectState = OS_STOPPED;
}



