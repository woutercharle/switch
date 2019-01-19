/*?*************************************************************************
*                           Switch_ApplicationModule.h
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

#ifndef _SWITCH_APPLICATIONMODULE
#define _SWITCH_APPLICATIONMODULE

// project includes
#include "Switch_StatedObject.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Parameters/Switch_ParameterizedObject.h"

// third party includes


namespace Switch
{  
  /*!
    \brief Base class for objects with parameters
   */
  class ApplicationModule : public Switch::StatedObject, public Switch::ParameterizedObject
  {
  public:
  
    /*!
      \brief Parameters container object.
     */
    class Parameters : public Switch::ParameterizedObject::Parameters
    {
    public:
      /*!
        \brief Default constructor.
       */
      explicit Parameters ();
      /*!
        \brief Destructor.
       */
      virtual ~Parameters ();
    };
    
    /*!
      \brief Default constructor.
     */
    explicit ApplicationModule ();
    /*!
      \brief Destructor.
     */
    virtual ~ApplicationModule ();
    
    // parameter modifiers
    /*!
      \brief Sets the parameters of the parameterized object.
      
      \param [in] i_parameters Parameters to set in the parameterized object.
      
      \note The object must be STOPPED.
     */
    virtual void SetParameters (const Switch::ParameterizedObject::Parameters& i_parameters);
    /*!
      \brief Sets the parameters of the parameterized object.
      
      \param [in] i_parameters Parameters to set in the parameterized object.
      
      \note The object must be STOPPED.
     */
    virtual void SetParameters (const Switch::ParameterContainer& i_parameters);
    
  protected:
  
    // accessors
    /*!
      \brief Gets the parameters of the parameterized object.
      
      \param [out] o_parameters Parameters of the parameterized object.
     */
    virtual void _GetParameters (Switch::ParameterizedObject::Parameters& o_parameters) const = 0;
    
    // modifiers
    /*!
      \brief Sets the parameters of the parameterized object.
      
      \param [in] i_parameters Parameters to set in the parameterized object.
     */
    virtual void _SetParameters (const Switch::ParameterizedObject::Parameters& i_parameters) = 0;
    /*!
      \brief Prepares the object.
      
      Moves the object from the STOPPED state to the READY state.
      
      \note The object's state must be STOPPED.
     */
    void _Prepare () = 0;
    /*!
      \brief Starts the object.
      
      Moves the object from the READY state to the STARTED state.
      
      \note The object's state must be READY.
     */
    void _Start () = 0;
    /*!
      \brief Pauses the object.
      
      Moves the object from the STARTED state to the READY state.
      
      \note The object's state must be STARTED.
     */
    void _Pause () = 0;
    /*!
      \brief Stops the object.
      
      Moves the object from the STARTED or the READY state to the STOPPED state.
      
      \note The object's state must be STARTED or READY.
     */
    void _Stop () = 0;
    
    // state variables
    mutable std::mutex m_operationsMutex;
    
  private:
    
  };
}

#endif // _SWITCH_PARAMETERIZEDOBJECT

