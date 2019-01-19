/*?*************************************************************************
*                           Switch_NodeAssignmentPayload.cpp
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

#include "Switch_NodeAssignmentPayload.h"


/*!
  \brief Constructor
 */
Switch::NodeAssignmentPayload::NodeAssignmentPayload ()
: nodeNetworkAddress (0x0),
  nodeDeviceAddress (0x0),
  communicationPipeAddress (0x0)
{
}

/*!
  \brief Destructor
 */
Switch::NodeAssignmentPayload::~NodeAssignmentPayload ()
{
}

/*!
  \brief Copy constructor

  \param[in] i_other Object to copy
 */
Switch::NodeAssignmentPayload::NodeAssignmentPayload (const Switch::NodeAssignmentPayload& i_other)
{
  nodeNetworkAddress        = i_other.nodeNetworkAddress;
  nodeDeviceAddress         = i_other.nodeDeviceAddress;
  communicationPipeAddress  = i_other.communicationPipeAddress;
}

/*!
  \brief Assignment operator

  \param[in] i_other Object to assign to this object
  
  \return Reference to this object
 */
Switch::NodeAssignmentPayload& Switch::NodeAssignmentPayload::operator= (const Switch::NodeAssignmentPayload& i_other)
{
  // avoid self-assignment
  if (this != &i_other)
  {
    nodeNetworkAddress        = i_other.nodeNetworkAddress;
    nodeDeviceAddress         = i_other.nodeDeviceAddress;
    communicationPipeAddress  = i_other.communicationPipeAddress;
  }
  
  // return reference to this object
  return *this;
}
