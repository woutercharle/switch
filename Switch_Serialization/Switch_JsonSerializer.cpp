/*?*************************************************************************
*                           Switch_JsonSerializer.cpp
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

// project includes
#include "Switch_JsonSerializer.h"
#include "Switch_JsonSerializableInterface.h"

// switch includes
#include "../Switch_Base/Switch_Debug.h"

// third-party includes
#include <json/value.h>
#include <json/writer.h>
#include <json/reader.h>


/*!
  \brief Serializes an object implementing the JsonSerializableInterface to a stream.
  
  \param [in,out] io_outStream Stream to which to serialize the object.
  \param [in] i_object Object to serialize.
 */
void Switch::JsonSerializer::Serialize (std::ostream& io_outStream, const Switch::JsonSerializableInterface& i_object)
{
  // serialize the object
  Json::Value serializationRoot;
  i_object.Serialize (serializationRoot);
 
  // write JSON to the stream
  Json::StyledStreamWriter streamWriter;
  streamWriter.write (io_outStream, serializationRoot);
}

/*!
  \brief De-serializes an object implementing the JsonSerializableInterface from a stream.
  
  \param [in,out] io_object Object to de-serialize.
  \param [in,out] io_outStream Stream from which to de-serialize the object.
  
  \throw An exception of type std::ios_base::failure is thrown when parsing the input stream fails.
 */
void Switch::JsonSerializer::Deserialize (Switch::JsonSerializableInterface& io_object, std::istream& io_inStream)
{
  // read JSON from the stream
  Json::Value deserializationRoot;
  Json::Reader reader;
 
  bool result = reader.parse (io_inStream, deserializationRoot, false);
  if (!result)
  {
    throw std::ios_base::failure ("failed to parse input stream to JSON");
  }
 
  // de-serialize the object
  io_object.Deserialize (deserializationRoot);
}
