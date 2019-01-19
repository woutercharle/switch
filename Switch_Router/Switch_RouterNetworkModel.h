/*?*************************************************************************
*                           Switch_RouterNetworkModel.h
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

#ifndef _SWITCH_ROUTERNETWORKMODEL
#define _SWITCH_ROUTERNETWORKMODEL

// switch router includes
#include "Switch_RouterNodeModel.h"

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"
#include "../Switch_Base/Switch_Types.h"

// std includes
#include <list>
#include <map>

// forward declarations


namespace Switch
{
  /*!
    \brief Network broadcast
   */
  class RouterNetworkModel
  {
  public:

    // default constructor disabled
    RouterNetworkModel () = delete;
    /*!
      \brief Constructor
     */
    RouterNetworkModel (const Switch::RouterNodeModel& i_routerNode);
    /*!
      \brief Destructor
     */
    ~RouterNetworkModel ();

    // copy constructor and assignment operator are disabled
    RouterNetworkModel (const RouterNetworkModel& i_other) = delete;
    RouterNetworkModel& operator= (const RouterNetworkModel& i_other) = delete;

    /*!
      \brief Adds a node model to the network model.

      If the node is later updated with communication information, the node becomes routable.

      \param [in] i_deviceAddress Device address of the node.
     */
    void AddNode (const switch_device_address_type& i_deviceAddress);

    /*!
      \brief Updates a node model with communication information.

      If a node with given device address was previously added, the communication information is set for this node.
      If the node was not added, no action is taken. After setting the communication information the node becomes
      routable.

      \param [in] i_deviceAddress Device address of the node.
      \param [in] i_deviceRxAddress The address of the device's rx pipe on which it receives messages.

      \warning The communication information of a node can be set only once and hence cannot be changed.
     */
    void UpdateNode (const switch_device_address_type& i_deviceAddress, const switch_pipe_address_type& i_deviceRxAddress);

    /*!
      \brief Gets a pointer to the node model with given device address

      \param [in] i_deviceAddress Device address of the node to get

      \return Const pointer to the node model with given device address or null
     */
    const Switch::RouterNodeModel* GetNode (const switch_device_address_type& i_deviceAddress) const;
    const Switch::RouterNodeModel* GetNode (const Switch::NetworkAddress& i_networkAddress) const;

    /*!
      \brief Unassigns the node with the given device address

      Unassigns the node and adds it to the list of unassigned nodes with a reset counter.

      \param [out] List with the device addresses of all nodes that were unassigned
      \param [in] i_deviceAddress The device address of the node
     */
    void UnAssignNode (std::list <switch_device_address_type>& o_unassignedNodes, const switch_device_address_type& i_deviceAddress);

    /*!
      \brief Unassigns the whole branch containing the node with the given device address

      Unassigns every node in the branch and adds the nodes to the list of unassigned nodes with a reset counter.

      \param [out] List with the device addresses of all nodes that were unassigned
      \param [in] i_deviceAddress The device address of the node in the branch
     */
    void UnAssignBranch (std::list <switch_device_address_type>& o_unassignedNodes, const switch_device_address_type& i_deviceAddress);

    /*!
      \brief Assigns a node as child to a parent

      The given child node is added to the given parent node as a child. A network address is computed and returned.

      \param [in] i_parentAddress The device address of the parent node
      \param [in] i_childAddress The device address of the child node

      \return The network address of the new child node
     */
    Switch::NetworkAddress AssignNode (const switch_device_address_type& i_parentAddress, const switch_device_address_type& i_childAddress);

    /*!
      \brief Sets a hearing relationship between two nodes

      \param [in] i_nodeAddress The node who hears the other node
      \param [in] i_otherNodeAddress The other node who is heared by the node

      \return The number of times the node has already heard the other node
     */
    uint32_t SetNodeHearsOtherNode (const switch_device_address_type& i_nodeAddress, const switch_device_address_type& i_otherNodeAddress);

    void FillListUnassignedNodes (std::list <const Switch::RouterNodeModel*>& o_unassignedNodes, const uint8_t& i_minHearingCount=1) const;

  private:

    // members
    Switch::RouterNodeModel m_routerNode;                                             ///< Keeps track of the router node
    std::map <switch_device_address_type, Switch::RouterNodeModel*> m_knownNodesMap;  ///< Keeps track of all known node addresses mapped to a node model
    std::map <Switch::RouterNodeModel*, uint32_t> m_unassignedNodesCountMap;          ///< Keeps track of the unassigned nodes in the network mapped to the number of times they are heared

  private:

    /*!
      \brief Gets a pointer to the node model with given device address

      \param [in] i_deviceAddress Device address of the node to get

      \return Const pointer to the node model with given device address or null
     */
    Switch::RouterNodeModel* _GetNode (const switch_device_address_type& i_deviceAddress);
    Switch::RouterNodeModel* _GetNode (const Switch::NetworkAddress& i_networkAddress);

  };
}

#endif // _SWITCH_ROUTERNETWORKMODEL
