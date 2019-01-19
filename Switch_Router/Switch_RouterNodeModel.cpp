/*?*************************************************************************
*                           Switch_RouterNodeModel.cpp
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

#include "Switch_RouterNodeModel.h"

// Switch includes
#include "../Switch_Base/Switch_Debug.h"

// std includes
#include <algorithm>

/*!
  \brief Constructor
 */
Switch::RouterNodeModel::RouterNodeModel ()
: pParentNode (0x0),
  deviceAddress (0x0),
  networkAddress (0x0),
  rxPipeAddress (0x0),
  m_isRouter (false)
{
  for (uint8_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
  {
    pChildNodes [i] = 0x0;
  }
}

/*!
  \brief Constructor

  \param[in] i_deviceAddress The node's device address
  \param[in] i_rxPipeAddress The node's rx pipe address
 */
Switch::RouterNodeModel::RouterNodeModel (const switch_device_address_type& i_deviceAddress, const switch_pipe_address_type& i_rxPipeAddress, const bool& i_isRouter)
: pParentNode (0x0),
  deviceAddress (i_deviceAddress),
  networkAddress (0x0),
  rxPipeAddress (i_rxPipeAddress),
  m_isRouter (i_isRouter)
{
  for (uint8_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
  {
    pChildNodes [i] = 0x0;
  }
}

/*!
  \brief Destructor
 */
Switch::RouterNodeModel::~RouterNodeModel ()
{
}

// copy constructor and assignment operator
Switch::RouterNodeModel::RouterNodeModel (const Switch::RouterNodeModel& i_other)
{
  m_isRouter              = i_other.m_isRouter;
  deviceAddress           = i_other.deviceAddress;
  networkAddress          = i_other.networkAddress;
  rxPipeAddress           = i_other.rxPipeAddress;
  receiverNodes           = i_other.receiverNodes;
  receivingNodesCountMap  = i_other.receivingNodesCountMap;
  pParentNode             = i_other.pParentNode;
  for (uint8_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
  {
    pChildNodes [i] = i_other.pChildNodes [i];
  }
}

Switch::RouterNodeModel& Switch::RouterNodeModel::operator= (const Switch::RouterNodeModel& i_other)
{
  // avoid self-assignment
  if (this != &i_other)
  {
    m_isRouter              = i_other.m_isRouter;
    deviceAddress           = i_other.deviceAddress;
    networkAddress          = i_other.networkAddress;
    rxPipeAddress           = i_other.rxPipeAddress;
    receiverNodes           = i_other.receiverNodes;
    receivingNodesCountMap  = i_other.receivingNodesCountMap;
    pParentNode             = i_other.pParentNode;
    for (uint8_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
    {
      pChildNodes [i] = i_other.pChildNodes [i];
    }
  }

  return *this;
}

/*!
  \brief Increments the number of times this node has heared the given node

  Adds the node if the node hasn't been heard previously.

  \param[in] i_pNode Node to add
 */
uint32_t Switch::RouterNodeModel::SetHears (Switch::RouterNodeModel* i_pNode)
{
  // fetch the counter for the node
  uint32_t& count = receivingNodesCountMap [i_pNode];
  // increment the counter
  ++count;
  // return the counter
  return count;
}

/*!
  \brief Removes a given node from the list of nodes heared by this node

  \param[in] i_pNode Node to remove
 */
void Switch::RouterNodeModel::RemoveHearedNode (Switch::RouterNodeModel* i_pNode)
{
  SWITCH_ASSERT (0x0 != i_pNode);
  SWITCH_ASSERT (receivingNodesCountMap.end () != receivingNodesCountMap.find (i_pNode));

  // erase the node and its hearing counter from the list
  receivingNodesCountMap.erase (i_pNode);
}

void Switch::RouterNodeModel::SetIsHearedBy (Switch::RouterNodeModel* i_pNode)
{
  // check if the node is already in the list of receivers
  std::list <RouterNodeModel*>::const_iterator nodeIterator = std::find (receiverNodes.begin(), receiverNodes.end(), i_pNode);
  if (receiverNodes.end() != nodeIterator)
  {
    return;
  }
  // add the node to the list of receivers
  receiverNodes.push_back (i_pNode);
}

bool Switch::RouterNodeModel::GetIsAssigned () const
{
  if (m_isRouter)
  {
    // the router is always assigned
    return true;
  }

  // assert the consistency of the member variables
  SWITCH_ASSERT (((0x0 == pParentNode) && (networkAddress == 0x0)) || ((0x0 != pParentNode) && (networkAddress != 0x0)));

  // the node is assigned if the network address is non-zero
  return (networkAddress != 0x0);
}

/*!
  \brief Unassign the node
 */
void Switch::RouterNodeModel::UnAssign ()
{
  SWITCH_DEBUG_MSG_0 ("reset network address, forget parent and children ... ");
  // reset network address
  networkAddress   = 0x0;
  if (0x0 != pParentNode)
  {
    pParentNode->_ForgetChild (deviceAddress);
    pParentNode      = 0x0;
  }
  for (uint8_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
  {
    pChildNodes [i]  = 0x0;
  }
  std::list <RouterNodeModel*>::iterator receiverIterator;
  for (receiverIterator=receiverNodes.begin (); receiverIterator!=receiverNodes.end (); receiverIterator=receiverNodes.erase (receiverIterator))
  {
    RouterNodeModel* pReceiver = *receiverIterator;
    SWITCH_ASSERT (0x0 != pReceiver);
    pReceiver->RemoveHearedNode (this);
  }
}

/*!
  \brief Unassign a child node

  \param[in] i_childDeviceAddress Device address of the child to unassign
 */
void Switch::RouterNodeModel::UnAssignChild (const switch_device_address_type& i_childDeviceAddress)
{
  for (uint8_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
  {
    if ((0x0 != pChildNodes [i]) && (i_childDeviceAddress == pChildNodes [i]->deviceAddress))
    {
      // forget about the child
      Switch::RouterNodeModel* pFormerChild = pChildNodes [i];
      pChildNodes [i] = 0x0;

      // tell the child it is an orphan
      pFormerChild->UnAssign ();

      // done
      break;
    }
  }
}

/*!
  \brief Assigns a child to a node at a given index

  \param[in] i_pChildNode Pointer to the child node
  \param[in] i_childIndex Index to assign the child to

  \return The network address of the child
 */
Switch::NetworkAddress Switch::RouterNodeModel::AssignChild (Switch::RouterNodeModel* i_pChildNode)
{
  SWITCH_ASSERT_RETURN_1 (0x0 != i_pChildNode, 0x0);
  SWITCH_DEBUG_MSG_0 ("assigning child ... ");

  // find a vacant child index
  uint8_t childIndex;
  for (childIndex=0; childIndex<NODE_MAX_NR_CHILD_NODES; ++childIndex)
  {
    if (0x0 == pChildNodes [childIndex])
    {
      break;
    }
  }
  SWITCH_ASSERT_RETURN_1 (NODE_MAX_NR_CHILD_NODES > childIndex, 0x0);
  SWITCH_DEBUG_MSG_1 ("at index %u ... ", childIndex);

  // compose the child's network address
  Switch::NetworkAddress childAddress = networkAddress;
  uint8_t branchIndex = networkAddress.GetBranchIndex ();
  childAddress.SetBranchIndex (branchIndex + 1);
  childAddress.SetChildIndex (branchIndex, childIndex);

  SWITCH_DEBUG_MSG_1 ("child network address 0x%x ... ", childAddress.value);
  SWITCH_ASSERT (7 > branchIndex);

  // assign the node
  i_pChildNode->_Assign (this, childAddress);
  pChildNodes [childIndex] = i_pChildNode;

  // return the network address
  return childAddress;
}

/*!
  \brief Sets the parent node and network address
 */
void Switch::RouterNodeModel::_Assign (Switch::RouterNodeModel* i_pParentNode, const Switch::NetworkAddress& i_networkAddress)
{
  SWITCH_ASSERT (0x0 == pParentNode);
  SWITCH_ASSERT (0x0 != rxPipeAddress);
  SWITCH_ASSERT (networkAddress == 0x0);
# ifdef _DEBUG
  for (uint8_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
  {
    SWITCH_ASSERT (0x0 == pChildNodes [i]);
  }
# endif

  pParentNode    = i_pParentNode;
  networkAddress = i_networkAddress;
}

/*!
  \brief Forget the parent
 */
void Switch::RouterNodeModel::_ForgetParent ()
{
  // forget about the child
  pParentNode = 0x0;
}

/*!
  \brief Forget a child node
 */
void Switch::RouterNodeModel::_ForgetChild (const switch_device_address_type& i_childDeviceAddress)
{
  for (uint8_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
  {
    if ((0x0 != pChildNodes [i]) && (i_childDeviceAddress == pChildNodes [i]->deviceAddress))
    {
      // forget about the child
      pChildNodes [i] = 0x0;

      // done
      break;
    }
  }
}

/*!
  \brief Gets a pointer to the child node model with given network address

  \param[in] i_networkAddress Network address of the child node to get

  \return Const pointer to the child node model with given network address or null
 */
const Switch::RouterNodeModel* Switch::RouterNodeModel::GetChild (const Switch::NetworkAddress& i_networkAddress) const
{
  if (networkAddress == i_networkAddress)
  {
    // this node is the seeked child
    return this;
  }
  else
  {
    Switch::RouterNodeModel* pNextNode = 0x0;

    uint8_t branchIndex = networkAddress.GetBranchIndex ();
    uint16_t mask = !(0xFFFF << 2*branchIndex);

    if ((i_networkAddress.GetBranchIndex () > branchIndex) &&
        (i_networkAddress.value & mask) == (networkAddress.value & mask))
    // get in direction of children
    {
      // get the child index
      uint8_t childIndex = i_networkAddress.GetChildIndex (branchIndex);

      // forward call to the corresponding node
      pNextNode = pChildNodes [childIndex];
    }
    else
    // send in direction of router
    {
      pNextNode = pParentNode;
    }

    if (0x0 != pNextNode)
    {
      // if the child exists forward the call
      return pNextNode->GetChild (i_networkAddress);
    }
    else
    {
      // if the child does not exist, return 0x0
      return 0x0;
    }
  }
}

/*!
  \brief Gets a pointer to the child node model with given network address

  \param[in] i_networkAddress Network address of the child node to get

  \return Pointer to the child node model with given network address or null
 */
Switch::RouterNodeModel* Switch::RouterNodeModel::GetChild (const Switch::NetworkAddress& i_networkAddress)
{
  if (networkAddress == i_networkAddress)
  {
    // this node is the seeked child
    return this;
  }
  else
  {
    Switch::RouterNodeModel* pNextNode = 0x0;

    uint8_t branchIndex = networkAddress.GetBranchIndex ();
    uint16_t mask = !(0xFFFF << 2*branchIndex);

    if ((i_networkAddress.GetBranchIndex () > branchIndex) &&
        (i_networkAddress.value & mask) == (networkAddress.value & mask))
    // get in direction of children
    {
      // get the child index
      uint8_t childIndex = i_networkAddress.GetChildIndex (branchIndex);

      // forward call to the corresponding node
      pNextNode = pChildNodes [childIndex];
    }
    else
    // send in direction of router
    {
      pNextNode = pParentNode;
    }

    if (0x0 != pNextNode)
    {
      // if the child exists forward the call
      return pNextNode->GetChild (i_networkAddress);
    }
    else
    {
      // if the child does not exist, return 0x0
      return 0x0;
    }
  }
}

/*!
  \brief Gets this node's distance to the router node

  \return The distance to the router node or 0 if this is the router node
 */
uint8_t Switch::RouterNodeModel::GetDistanceToRouterNode () const
{
  if (0x0 != pParentNode)
  {
    return 1 + pParentNode->GetDistanceToRouterNode ();
  }
  else return 0;
}

uint8_t Switch::RouterNodeModel::GetNrChildPositionsAvailable () const
{
  size_t count = 0;
  for (size_t i=0; i<NODE_MAX_NR_CHILD_NODES; ++i)
  {
    if (0x0 == pChildNodes [i])
    {
      ++count;
    }
  }
  return count;
}

