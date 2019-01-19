/*?*************************************************************************
*                           Switch_JsonSerializer.h
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

#ifndef _SWITCH_JSONSERIALIZER
#define _SWITCH_JSONSERIALIZER

// project includes

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"

// third party includes
#include <iostream>


namespace Switch
{
  // forward declarations
  class JsonSerializableInterface;
  
  
  /*!
    \brief Base class for objects with parameters
   */
  class JsonSerializer
  {
  public:
      
    // construction of this object is disabled
    explicit JsonSerializer () = delete;
    
    /*!
      \brief Serializes an object implementing the JsonSerializableInterface to a stream.
      
      \param [in,out] io_outStream Stream to which to serialize the object.
      \param [in] i_object Object to serialize.
     */
    static void Serialize (std::ostream& io_outStream, const Switch::JsonSerializableInterface& i_object);
    
    /*!
      \brief De-serializes an object implementing the JsonSerializableInterface from a stream.
      
      \param [in,out] io_object Object to de-serialize.
      \param [in,out] io_outStream Stream from which to de-serialize the object.
     */
    static void Deserialize (Switch::JsonSerializableInterface& io_object, std::istream& io_inStream);
    
  protected:
    
  private:
    
  };
}

#endif // _SWITCH_JSONSERIALIZER

