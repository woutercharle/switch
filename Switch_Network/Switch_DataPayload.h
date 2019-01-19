/*?*************************************************************************
*                           Switch_DataPayload.h
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

#ifndef _SWITCH_DATAPAYLOAD
#define _SWITCH_DATAPAYLOAD

#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"

#pragma pack (push)
#pragma pack (1)

namespace Switch
{
  /*!
    \brief Broadcast network message payload
   */
  class DataPayload
  {
  public:
  
    /*
      Maximum size in bytes of the data in the payload
      Must be smaller or equal to NM_MAX_PAYLOAD_SIZE
     */
#   define DP_MAX_DATA_SIZE 24

    /*!
      \brief Constructor
     */
    DataPayload ();

    /*!
      \brief Destructor
     */
    ~DataPayload ();

    /*!
      \brief Copy constructor

      \param[in] i_other Object to copy
     */
    DataPayload (const DataPayload& i_other);

    /*!
      \brief Assignment operator

      \param[in] i_other Object to assign to this object

      \return Reference to this object
     */
    DataPayload& operator= (const DataPayload& i_other);

    // members
    uint8_t data [DP_MAX_DATA_SIZE];  ///< The actual data
  };
}

#pragma pack (pop)

#endif // _SWITCH_DATAPAYLOAD
