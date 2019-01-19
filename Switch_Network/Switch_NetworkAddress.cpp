/*?*************************************************************************
*                           Switch_NetworkAddress.cpp
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

#include "Switch_NetworkAddress.h"

/*!
  \brief Constructor
 */
Switch::NetworkAddress::NetworkAddress (const switch_network_address_type& i_value)
: value (i_value)
{
}

/*!
  \brief Destructor
 */
Switch::NetworkAddress::~NetworkAddress ()
{
}

/*!
  \brief Copy constructor

  \param[in] i_other Object to copy
 */
Switch::NetworkAddress::NetworkAddress (const Switch::NetworkAddress& i_other)
{
  value = i_other.value;
}

/*!
  \brief Assignment operator

  \param[in] i_other Object to assign to this object
  
  \return Reference to this object
 */
Switch::NetworkAddress& Switch::NetworkAddress::operator= (const Switch::NetworkAddress& i_other)
{
  // avoid self-assignment
  if (this != &i_other)
  {
    value = i_other.value;
  }
  
  // return reference to this object
  return *this;
}

/*!
  \brief Equality operator

  \param[in] i_other Object to check if equal to this object

  \return True if equal to this object, false otherwise
 */
bool Switch::NetworkAddress::operator== (const NetworkAddress& i_other) const
{
  return (i_other.value == value);
}

/*!
  \brief Equality operator

  \param[in] i_value Value to check if equal to this object

  \return True if equal to this object, false otherwise
 */
bool Switch::NetworkAddress::operator== (const switch_network_address_type& i_value) const
{
  return (i_value == value);
}

/*!
  \brief In-equality operator

  \param[in] i_other Object to check if not equal to this object

  \return True if not equal to this object, false otherwise
 */
bool Switch::NetworkAddress::operator!= (const NetworkAddress& i_other) const
{
  return (i_other.value != value);
}

/*!
  \brief In-equality operator

  \param[in] i_value Value to check if not equal to this object

  \return True if not equal to this object, false otherwise
 */
bool Switch::NetworkAddress::operator!= (const switch_network_address_type& i_value) const
{
  return (i_value != value);
}

uint8_t Switch::NetworkAddress::GetBranchIndex () const
{
  return value >> 12;
}

uint8_t Switch::NetworkAddress::GetChildIndex (const uint8_t& i_branchIndex) const
{
  return (value >> (2*i_branchIndex)) & 0x3;
}

void Switch::NetworkAddress::SetBranchIndex (const uint8_t& i_branchIndex)
{
  // reset the branch bits
  value = value & 0x0FFF;
  // set the branch bits
  value = value | (static_cast <switch_network_address_type> (i_branchIndex) << 12);
}

void Switch::NetworkAddress::SetChildIndex (const uint8_t& i_branchIndex, const uint8_t& i_childIndex)
{
  // reset the child bits
  value = value & ~(static_cast <switch_network_address_type> (0x3) << (2*i_branchIndex));
  // set the child bits
  value = value | (static_cast <switch_network_address_type> (i_childIndex) << (2*i_branchIndex));
}
