/*?*************************************************************************
*                           Switch_RouterNetworkModel.cpp
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

#include "Switch_RouterNetworkModel.h"

// Switch router includes
#include "Switch_RouterNodeModel.h"

// Switch includes
#include "../Switch_Base/Switch_Debug.h"


/*!
  \brief Constructor
 */
Switch::RouterNetworkModel::RouterNetworkModel (const RouterNodeModel& i_routerNode)
{
  m_routerNode = i_routerNode;
}

/*!
  \brief Destructor
 */
Switch::RouterNetworkModel::~RouterNetworkModel ()
{
  // clear all unassigned nodes
  m_unassignedNodesCountMap.clear ();

  // delete all known nodes
  std::map <switch_device_address_type, Switch::RouterNodeModel*>::iterator nodeIterator;
  for (nodeIterator=m_knownNodesMap.begin (); nodeIterator!=m_knownNodesMap.end (); ++nodeIterator)
  {
    delete nodeIterator->second;
    nodeIterator->second = 0x0;
  }
  m_knownNodesMap.clear ();
}

/*!
  \brief Adds a node model to the network model.

  If the node is later updated with communication information, the node becomes routable.

  \param [in] i_deviceAddress Device address of the node.
 */
void Switch::RouterNetworkModel::AddNode (const switch_device_address_type& i_deviceAddress)
{
  // make sure this is not the router node
  SWITCH_ASSERT_RETURN_0 (m_routerNode.deviceAddress != i_deviceAddress);

  // fetch an entry in the known nodes map for the node
  Switch::RouterNodeModel*& pNode = m_knownNodesMap [i_deviceAddress];
  if (0x0 != pNode)
  {
    return;
  }

  // create the node
  pNode = new Switch::RouterNodeModel (i_deviceAddress);

  // add node to unassigned nodes list with counter at zero
  m_unassignedNodesCountMap [pNode] = 0;
}

/*!
  \brief Updates a node model with communication information.

  If a node with given device address was previously added, the communication information is set for this node.
  If the node was not added, no action is taken. After setting the communication information the node becomes
  routable.

  \param [in] i_deviceAddress Device address of the node.
  \param [in] i_deviceRxAddress The address of the device's rx pipe on which it receives messages.

  \warning The communication information of a node can be set only once and hence cannot be changed.
 */
void Switch::RouterNetworkModel::UpdateNode (const switch_device_address_type& i_deviceAddress, const switch_pipe_address_type& i_deviceRxAddress)
{
  SWITCH_ASSERT (0x0 != i_deviceRxAddress);

  // find the node with given device address in the known nodes map
  std::map <switch_device_address_type, Switch::RouterNodeModel*>::iterator itKnownNodes;
  itKnownNodes = m_knownNodesMap.find (i_deviceAddress);
  if (m_knownNodesMap.end () == itKnownNodes)
  {
    // not found
    return;
  }

  // set the node's rx address
  Switch::RouterNodeModel* pNode = itKnownNodes->second;
  SWITCH_ASSERT_RETURN_0 (0x0 == pNode->rxPipeAddress);
  pNode->rxPipeAddress = i_deviceRxAddress;
}

/*!
  \brief Gets a pointer to the node model with given device address.

  \param[in] i_deviceAddress Device address of the node to get.

  \note A node model must have a rx pipe address and must be routable before it will be returned.

  \return Const pointer to the node model with given device address or null.
 */
const Switch::RouterNodeModel* Switch::RouterNetworkModel::GetNode (const switch_device_address_type& i_deviceAddress) const
{
  if (m_routerNode.deviceAddress == i_deviceAddress)
  {
    // device address equals the router node
    return &m_routerNode;
  }
  std::map <switch_device_address_type, RouterNodeModel*>::const_iterator nodeModelIterator = m_knownNodesMap.find (i_deviceAddress);
  if (m_knownNodesMap.end () != nodeModelIterator)
  {
    // device found
    RouterNodeModel* pNodeModel = nodeModelIterator->second;
    if (0x0 != pNodeModel->rxPipeAddress)
    {
      return pNodeModel;
    }
  }

  // device not found or incomplete
  return 0x0;
}

/*!
  \brief Gets a pointer to the node model with given device address.

  \param[in] i_deviceAddress Device address of the node to get.

  \note A node model must have a rx pipe address and must be routable before it will be returned.

  \return Pointer to the node model with given device address or null.
 */
Switch::RouterNodeModel* Switch::RouterNetworkModel::_GetNode (const switch_device_address_type& i_deviceAddress)
{
  if (m_routerNode.deviceAddress == i_deviceAddress)
  {
    // device address equals the router node
    return &m_routerNode;
  }
  std::map <switch_device_address_type, RouterNodeModel*>::iterator nodeModelIterator = m_knownNodesMap.find (i_deviceAddress);
  if (m_knownNodesMap.end () != nodeModelIterator)
  {
    // device found
    RouterNodeModel* pNodeModel = nodeModelIterator->second;
    if (0x0 != pNodeModel->rxPipeAddress)
    {
      return pNodeModel;
    }
  }

  // device not found or incomplete
  return 0x0;
}

/*!
  \brief Gets a pointer to the node model with given network address

  \param[in] i_networkAddress Network address of the node to get

  \note A node model must have a rx pipe address and must be routable before it will be returned.

  \return Const pointer to the node model with given network address or null
 */
const Switch::RouterNodeModel* Switch::RouterNetworkModel::GetNode (const Switch::NetworkAddress& i_networkAddress) const
{
  // get the node model
  return m_routerNode.GetChild (i_networkAddress);
}

/*!
  \brief Gets a pointer to the node model with given network address

  \param[in] i_networkAddress Network address of the node to get

  \note A node model must have a rx pipe address and must be routable before it will be returned.

  \return Pointer to the node model with given network address or null
 */
Switch::RouterNodeModel* Switch::RouterNetworkModel::_GetNode (const Switch::NetworkAddress& i_networkAddress)
{
  // get the node model
  return m_routerNode.GetChild (i_networkAddress);
}

/*!
  \brief Unassigns the whole branch containing the node with the given device address

  Unassigns every node in the branch and adds the nodes to the list of unassigned nodes with a reset counter.

  \param [in] i_deviceAddress The device address of the node in the branch

  \param [out] List with the device addresses of all nodes that were unassigned
  \return List with the device addresses of all nodes that were unassigned
 */
void Switch::RouterNetworkModel::UnAssignBranch (std::list <switch_device_address_type>& o_unassignedNodes, const switch_device_address_type& i_deviceAddress)
{
  SWITCH_DEBUG_MSG_0 ("unassigning branch ... ");
  o_unassignedNodes.clear ();

  // make sure this is not the router node
  SWITCH_ASSERT_RETURN_0 (m_routerNode.deviceAddress != i_deviceAddress);

  // fetch the node from the known nodes map
  Switch::RouterNodeModel* pNode = m_knownNodesMap [i_deviceAddress];
  SWITCH_ASSERT (0x0 != pNode);

  // trace back to the node connected to the router node
  while (&m_routerNode != pNode->pParentNode)
  {
    SWITCH_ASSERT (0x0 != pNode->pParentNode);
    pNode = pNode->pParentNode;
  }

  // un-assing the node and all its child nodes
  UnAssignNode (o_unassignedNodes, pNode->deviceAddress);
}

/*!
  \brief Unassigns the node with the given device address

  Unassigns the node and adds it to the list of unassigned nodes with a reset counter.

  \param [out] List with the device addresses of all nodes that were unassigned
  \param [in] i_deviceAddress The device address of the node
 */
void Switch::RouterNetworkModel::UnAssignNode (std::list <switch_device_address_type>& o_unassignedNodes, const switch_device_address_type& i_deviceAddress)
{
  // make sure this is not the router node
  SWITCH_ASSERT_RETURN_0 (m_routerNode.deviceAddress != i_deviceAddress);

  // fetch the node from the known nodes map
  Switch::RouterNodeModel* pNode = m_knownNodesMap [i_deviceAddress];

  // initialize list of nodes to un-assign
  std::list <Switch::RouterNodeModel*> unAssignList;
  unAssignList.push_back (pNode);

  // unassign all nodes in the list
  while (!unAssignList.empty ())
  {
    // get the node at the back of the list
    pNode = unAssignList.back ();

    // ensure the node exists and that it is currently assigned
    SWITCH_ASSERT_RETURN_0 (0x0 != pNode);
    SWITCH_ASSERT (pNode->GetIsAssigned ());

    // check if the node has children to unassign
    bool childLess = true;
    for (size_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
    {
      Switch::RouterNodeModel* pChildNode = pNode->pChildNodes [i];
      if (0x0 != pChildNode)
      {
        unAssignList.push_back (pChildNode);
        pNode->pChildNodes [i] = 0x0;
        childLess = false;
      }
    }

    // check if no children must be unassinged first
    if (childLess)
    {
      // unassign the node
      SWITCH_DEBUG_MSG_1 ("unassigning node 0x%04x ... ", pNode->deviceAddress);
      pNode->UnAssign ();
      o_unassignedNodes.push_back (pNode->deviceAddress);

      // add the node to the map of unassigned nodes
      m_unassignedNodesCountMap [pNode] = 0;

      // remove the node from the list
      unAssignList.pop_back ();
    }
  }
}

/*!
  \brief Assigns a node as child to a parent

  The given child node is added to the given parent node as a child. A network address is computed and returned.

  \param [in] i_parentAddress The device address of the parent node
  \param [in] i_childAddress The device address of the child node

  \return The network address of the new child node
 */
Switch::NetworkAddress Switch::RouterNetworkModel::AssignNode (const switch_device_address_type& i_parentAddress, const switch_device_address_type& i_childAddress)
{
  // make sure the child does not equal the parent
  SWITCH_ASSERT_RETURN_1 (i_parentAddress != i_childAddress, 0x0);
  // make sure the child is not the router node
  SWITCH_ASSERT_RETURN_1 (m_routerNode.deviceAddress != i_childAddress, 0x0);

  // get the parent node
  Switch::RouterNodeModel* pParentNode;
  if (m_routerNode.deviceAddress == i_parentAddress)
  {
    SWITCH_DEBUG_MSG_0 ("parent is router node ... ");
    pParentNode = &m_routerNode;
  }
  else
  {
    SWITCH_DEBUG_MSG_0 ("parent is not router node ... ");
    pParentNode = m_knownNodesMap [i_parentAddress];
  }

  // ensure the parent node exists and that it is assigned
  SWITCH_ASSERT_RETURN_1 (0x0 != pParentNode, 0x0);
  SWITCH_ASSERT (pParentNode->GetIsAssigned ());

  // get the child node
  Switch::RouterNodeModel* pChildNode = m_knownNodesMap [i_childAddress];

  // ensure the child node exists and that it is not assigned
  SWITCH_ASSERT_RETURN_1 (0x0 != pChildNode, 0x0);
  SWITCH_ASSERT (!pChildNode->GetIsAssigned ());

  // assign the child node to the parent
  Switch::NetworkAddress childNetworkAddress = pParentNode->AssignChild (pChildNode);
  SWITCH_ASSERT (childNetworkAddress == pChildNode->networkAddress);

  // remove the child from the unassigned nodes list
  m_unassignedNodesCountMap.erase (pChildNode);

  // return the child's network address
  return childNetworkAddress;
}

/*!
  \brief Sets a hearing relationship between two nodes

  \param [in] i_nodeAddress The node who hears the other node

  \param [in] i_otherNodeAddress The other node who is heared by the node

  \return The number of times the node has already heard the other node
 */
uint32_t Switch::RouterNetworkModel::SetNodeHearsOtherNode (const switch_device_address_type& i_nodeAddress, const switch_device_address_type& i_otherNodeAddress)
{
  // fetch the node and the other node
  Switch::RouterNodeModel* pNode = _GetNode (i_nodeAddress);
  SWITCH_ASSERT_RETURN_1 (0x0 != pNode, 0);
  Switch::RouterNodeModel* pOtherNode = _GetNode (i_otherNodeAddress);
  SWITCH_ASSERT_RETURN_1 (0x0 != pOtherNode, 0);
  SWITCH_ASSERT (!(pOtherNode->GetIsAssigned ()));

  // add the relationship between the nodes
  uint32_t count = pNode->SetHears (pOtherNode);
  pOtherNode->SetIsHearedBy (pNode);

  SWITCH_DEBUG_MSG_2 ("node %x now hears %u nodes ... ", pNode->deviceAddress, pNode->receivingNodesCountMap.size ());
  SWITCH_DEBUG_MSG_2 ("node %x is now heared by %u nodes ... ", pOtherNode->deviceAddress, pOtherNode->receiverNodes.size ());

  // check if the other node is unassigned
  std::map <Switch::RouterNodeModel*, uint32_t>::iterator nodeCountIterator = m_unassignedNodesCountMap.find (pOtherNode);
  if (m_unassignedNodesCountMap.end () != nodeCountIterator)
  {
    // increment the global hearing counter for the other node
    ++(nodeCountIterator->second);
  }

  // return the nr of times the node has heard the other node
  return count;
}

void Switch::RouterNetworkModel::FillListUnassignedNodes (std::list <const Switch::RouterNodeModel*>& o_unassignedNodes, const uint8_t& i_minHearingCount) const
{
  // clear output arguments
  o_unassignedNodes.clear ();

  // iterate over the map of unassigned nodes
  std::map <Switch::RouterNodeModel*, uint32_t>::const_iterator mapIterator;
  for (mapIterator = m_unassignedNodesCountMap.begin (); mapIterator != m_unassignedNodesCountMap.end (); ++mapIterator)
  {
    const Switch::RouterNodeModel* pNodeModel = mapIterator->first;

    // check if the node can be routed
    if (0x0 != pNodeModel->rxPipeAddress)
    {
      // check if the node has been heard a suffienct amount of time
      if (i_minHearingCount <= mapIterator->second)
      {
        // add node to list
        o_unassignedNodes.push_back (mapIterator->first);
      }
    }
  }
}

