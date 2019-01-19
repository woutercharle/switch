/*?*************************************************************************
*                           Switch_NodeAssignmentPayload.h
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

#ifndef _SWITCH_NODEASSIGNMENTPAYLOAD
#define _SWITCH_NODEASSIGNMENTPAYLOAD

#include "Switch_NetworkAddress.h"

#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"

#pragma pack (push)
#pragma pack (1)

namespace Switch
{
  /*!
    \brief Network broadcast
   */
  class NodeAssignmentPayload
  {
  public:
    /*!
      \brief Constructor
     */
    NodeAssignmentPayload ();
    
    /*!
      \brief Destructor
     */
    ~NodeAssignmentPayload ();

    /*!
      \brief Copy constructor

      \param[in] i_other Object to copy
     */
    NodeAssignmentPayload (const NodeAssignmentPayload& i_other);
    
    /*!
      \brief Assignment operator

      \param[in] i_other Object to assign to this object
      
      \return Rereference to this object
     */
    NodeAssignmentPayload& operator= (const NodeAssignmentPayload& i_other);

    // members
    Switch::NetworkAddress      nodeNetworkAddress;         ///< Network address to assign to the node
    switch_device_address_type  nodeDeviceAddress;          ///< Device address of the node
    switch_pipe_address_type    communicationPipeAddress;   ///< Address of the communication pipes. Initially the pipe address to the broadcasting node, replaced by the parent node to the final pipe address
  };
}

#pragma pack (pop)

#endif // _SWITCH_NODEASSIGNMENTPAYLOAD
