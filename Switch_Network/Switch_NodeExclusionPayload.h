/*?*************************************************************************
*                           Switch_NodeExclusionPayload.h
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

#ifndef _SWITCH_NODEEXCLUSIONPAYLOAD
#define _SWITCH_NODEEXCLUSIONPAYLOAD

#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"

#pragma pack (push)
#pragma pack (1)

namespace Switch
{
  /*!
    \brief Network broadcast
   */
  class NodeExclusionPayload
  {
  public:
  
	/*
      Enumeration of all possible node exclusions
     */
    typedef uint8_t node_selection_type;
#   define NE_NONE          0
#   define NE_ALL_CHILDREN  1
#   define NE_CHILD_0       2
#   define NE_CHILD_1 	    3
#   define NE_CHILD_2 	    4
#   define NE_CHILD_3 	    5
  
    /*!
      \brief Constructor
     */
    NodeExclusionPayload ();
    
    /*!
      \brief Destructor
     */
    ~NodeExclusionPayload ();

    /*!
      \brief Copy constructor

      \param[in] i_other Object to copy
     */
    NodeExclusionPayload (const NodeExclusionPayload& i_other);
    
    /*!
      \brief Assignment operator

      \param[in] i_other Object to assign to this object
      
      \return Rereference to this object
     */
    NodeExclusionPayload& operator= (const NodeExclusionPayload& i_other);

    // members
    node_selection_type nodeToExclude;	///< Node selected to exclude
    
  };
}

#pragma pack (pop)

#endif // _SWITCH_NODEEXCLUSIONPAYLOAD
