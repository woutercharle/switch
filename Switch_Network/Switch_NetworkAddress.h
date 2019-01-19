/*?*************************************************************************
*                           Switch_NetworkAddress.h
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

#ifndef _SWITCH_NETWORKADDRESS
#define _SWITCH_NETWORKADDRESS

#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"

#pragma pack (push)
#pragma pack (1)

namespace Switch
{
  /*!
    \brief Network address container with utility methods
   */
  class NetworkAddress
  {
  public:
  
    /*!
      \brief Constructor
      
      \param[in] i_networkAddress Network address to initialize the object with
     */
    NetworkAddress (const switch_network_address_type& i_value=0x0);

    /*!
      \brief Destructor
     */
    ~NetworkAddress ();

    /*!
      \brief Copy constructor

      \param[in] i_other Object to copy
     */
    NetworkAddress (const NetworkAddress& i_other);

    /*!
      \brief Assignment operator

      \param[in] i_other Object to assign to this object

      \return Reference to this object
     */
    NetworkAddress& operator= (const NetworkAddress& i_other);
    
    /*!
      \brief Equality operator

      \param[in] i_other Object to check if equal to this object

      \return True if equal to this object, false otherwise
     */
    bool operator== (const NetworkAddress& i_other) const;
    bool operator== (const switch_network_address_type& i_value) const;
    
    /*!
      \brief In-equality operator

      \param[in] i_other Object to check if not equal to this object

      \return True if not equal to this object, false otherwise
     */
    bool operator!= (const NetworkAddress& i_other) const;
    bool operator!= (const switch_network_address_type& i_value) const;

    // accessors
    uint8_t GetBranchIndex () const;
    uint8_t GetChildIndex (const uint8_t& i_branchIndex) const;
    
    // modifiers
    void SetBranchIndex (const uint8_t& i_branchIndex);
    void SetChildIndex (const uint8_t& i_branchIndex, const uint8_t& i_childIndex);
    
    // members
    switch_network_address_type value;  ///< The network address value (4 bits branchIndex; 6x2 bits child index) branchIndex 0 is router
  };
}

#pragma pack (pop)

#endif // _SWITCH_NETWORKADDRESS
