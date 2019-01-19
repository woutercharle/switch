/*?*************************************************************************
*                           Switch_PingPongPayload.h
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

#ifndef _SWITCH_PINGPONGPAYLOAD
#define _SWITCH_PINGPONGPAYLOAD

#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"

#pragma pack (push)
#pragma pack (1)

namespace Switch
{
  /*!
    \brief Broadcast network message payload
   */
  class PingPongPayload
  {
  public:
    /*!
      \brief Constructor
     */
    PingPongPayload ();

    /*!
      \brief Destructor
     */
    ~PingPongPayload ();

    /*!
      \brief Copy constructor

      \param[in] i_other Object to copy
     */
    PingPongPayload (const PingPongPayload& i_other);

    /*!
      \brief Assignment operator

      \param[in] i_other Object to assign to this object

      \return Reference to this object
     */
    PingPongPayload& operator= (const PingPongPayload& i_other);

    // members
    uint64_t transmissionStartTimeMs;  ///< Time of the start of the
  };
}

#pragma pack (pop)

#endif // _SWITCH_PINGPONGPAYLOAD
