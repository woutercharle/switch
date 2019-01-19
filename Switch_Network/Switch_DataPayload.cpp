/*?*************************************************************************
*                           Switch_DataPayload.cpp
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

#include "Switch_DataPayload.h"

/*!
  \brief Constructor
 */
Switch::DataPayload::DataPayload ()
{
  // set all bytes to 0
  memset (&data [0], 0, DP_MAX_DATA_SIZE);
}

/*!
  \brief Destructor
 */
Switch::DataPayload::~DataPayload ()
{
}

/*!
  \brief Copy constructor

  \param[in] i_other Object to copy
 */
Switch::DataPayload::DataPayload (const Switch::DataPayload& i_other)
{
  memcpy (&data [0], &(i_other.data) [0], DP_MAX_DATA_SIZE);
}

/*!
  \brief Assignment operator

  \param[in] i_other Object to assign to this object
  
  \return Reference to this object
 */
Switch::DataPayload& Switch::DataPayload::operator= (const Switch::DataPayload& i_other)
{
  // avoid self-assignment
  if (this != &i_other)
  {
    memcpy (&data [0], &(i_other.data) [0], DP_MAX_DATA_SIZE);
  }
  
  // return reference to this object
  return *this;
}
