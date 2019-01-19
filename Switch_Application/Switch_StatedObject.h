/*?*************************************************************************
*                           Switch_StatedObject.h
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

#ifndef _SWITCH_STATEDOBJECT
#define _SWITCH_STATEDOBJECT

// project includes

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"

// third party includes
#include <mutex>
#include <atomic>


namespace Switch
{
  /*!
    \brief Base class for objects with parameters
   */
  class StatedObject
  {
  public:

    enum eObjectState
    {
      OS_NOT_A_STATE = -1,
      OS_STOPPED     =  0,
      OS_READY       =  1,
      OS_STARTED     =  2
    };

    /*!
      \brief Default constructor.
     */
    explicit StatedObject ();
    /*!
      \brief Destructor.
     */
    virtual ~StatedObject ();

    // accessors
    /*!
      \brief Gets the object's state.

      \return The object's state.
     */
    eObjectState GetState () const;

    // modifiers
    /*!
      \brief Prepares the object.

      Moves the object from the STOPPED state to the READY state.

      \note The object's state must be STOPPED.
     */
    virtual void Prepare ();
    /*!
      \brief Starts the object.

      Moves the object from the READY state to the STARTED state.

      \note The object's state must be READY.
     */
    virtual void Start ();
    /*!
      \brief Pauses the object.

      Moves the object from the STARTED state to the READY state.

      \note The object's state must be STARTED.
     */
    virtual void Pause () noexcept;
    /*!
      \brief Stops the object.

      Moves the object from the STARTED or the READY state to the STOPPED state.

      \note The object's state must be STARTED or READY.
     */
    virtual void Stop () noexcept;

  protected:

    // modifiers
    /*!
      \brief Prepares the object.

      Moves the object from the STOPPED state to the READY state.

      \note The object's state must be STOPPED.
     */
    virtual void _Prepare () = 0;
    /*!
      \brief Starts the object.

      Moves the object from the READY state to the STARTED state.

      \note The object's state must be READY.
     */
    virtual void _Start () = 0;
    /*!
      \brief Pauses the object.

      Moves the object from the STARTED state to the READY state.

      \note The object's state must be STARTED.
     */
    virtual void _Pause () = 0;
    /*!
      \brief Stops the object.

      Moves the object from the STARTED or the READY state to the STOPPED state.

      \note The object's state must be STARTED or READY.
     */
    virtual void _Stop () = 0;

    // state variables
    mutable std::mutex          m_stateMutex;
    std::atomic <eObjectState>  m_objectState;

  private:

  };
}

#endif // _SWITCH_PARAMETERIZEDOBJECT

