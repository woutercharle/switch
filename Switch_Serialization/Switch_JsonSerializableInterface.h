/*?*************************************************************************
*                           Switch_JsonSerializableInterface.h
*                           -----------------------
*    copyright            : (C) 2013 by Wouter Charle
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

#ifndef _SWITCH_JSONSERIALIZABLEINTERFACE
#define _SWITCH_JSONSERIALIZABLEINTERFACE

// Switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"

// third-party includes

// forward declarations
namespace Json
{
  class Value;
}

namespace Switch
{
  /*!
    \brief Interface class for JSON serializable objects.
   */
  class JsonSerializableInterface
  {
  public:
  
    /*!
      \brief Destructor.
     */
    virtual ~JsonSerializableInterface () {};
    
    /*!
      \brief Serializes the object to a JSON value.
      
      \param [out] o_root Root value of the object as JSON.
     */
    virtual void Serialize   (Json::Value& o_root) const = 0;
    
    /*!
      \brief De-serializes the object from a JSON value.
      
      \param [in] i_root Root value of the object as JSON.
     */
    virtual void Deserialize (const Json::Value& i_root) = 0;
  };
}

#endif // _SWITCH_JSONSERIALIZABLEINTERFACE
