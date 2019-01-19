/*?*************************************************************************
*                           Switch_RouterNodeModel.h
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

#ifndef _SWITCH_ROUTERNODEMODEL
#define _SWITCH_ROUTERNODEMODEL

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"
#include "../Switch_Node/Switch_NodeConfiguration.h"
#include "../Switch_Network/Switch_NetworkAddress.h"

// std includes
#include <map>
#include <list>

// forward declarations

namespace Switch
{
  /*!
    \brief Network broadcast
   */
  class RouterNodeModel
  {
  public:

    /*!
      \brief Default constructor
     */
    RouterNodeModel ();

    /*!
      \brief Constructor

      \param[in] i_deviceAddress The node's device address
      \param[in] i_rxPipeAddress The node's rx pipe address
     */
    RouterNodeModel (const switch_device_address_type& i_deviceAddress, const switch_pipe_address_type& i_rxPipeAddress=0x0, const bool& i_isRouter=false);

    /*!
      \brief Destructor
     */
    ~RouterNodeModel ();

    // copy constructor and assignment operator
    RouterNodeModel (const RouterNodeModel& i_other);
    RouterNodeModel& operator= (const RouterNodeModel& i_other);

    uint32_t SetHears (Switch::RouterNodeModel* i_pNode);
    void SetIsHearedBy (Switch::RouterNodeModel* i_pNode);
    void RemoveHearedNode (Switch::RouterNodeModel* i_pNode);

    bool GetIsAssigned () const;
    void UnAssign ();
    void UnAssignChild (const switch_device_address_type& i_nodeAddress);
    Switch::NetworkAddress AssignChild (Switch::RouterNodeModel* i_pChildNode);

    const RouterNodeModel* GetChild (const Switch::NetworkAddress& i_networkAddress) const;
    RouterNodeModel*       GetChild (const Switch::NetworkAddress& i_networkAddress);
    uint8_t GetDistanceToRouterNode () const;
    uint8_t GetNrChildPositionsAvailable () const;

    // members
    std::list <RouterNodeModel*> 		      receiverNodes;			                    ///< List of pointers to the nodes that can hear this node
    std::map <RouterNodeModel*, uint32_t> receivingNodesCountMap;                 ///< Pointers to the nodes that this node is able to receive, mapped to the nr of times this node has heard each node
    RouterNodeModel* 					            pParentNode;                            ///< Pointer to the node's parent node
    RouterNodeModel* 					            pChildNodes [NODE_MAX_NR_CHILD_NODES];  ///< Pointers to the node's child nodes
    switch_device_address_type 			      deviceAddress;                          ///< This node's device address
    Switch::NetworkAddress                networkAddress;                         ///< This node's network address
    switch_pipe_address_type 		          rxPipeAddress;                          ///< This node's rx pipe address

  private:

    void _ForgetParent ();
    void _ForgetChild (const switch_device_address_type& i_nodeAddress);
    void _Assign (Switch::RouterNodeModel* i_pParentNode, const Switch::NetworkAddress& i_networkAddress);

    bool m_isRouter; ///< Indicates if this node is the router or not
  };
}

#endif // _SWITCH_ROUTERNODEMODEL
