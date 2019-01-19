/*?*************************************************************************
*                           Switch_NodeConfiguration.h
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

#ifndef _SWITCH_ROUTERCONFIGURATION
#define _SWITCH_ROUTERCONFIGURATION

#include "../Switch_Node/Switch_NodeConfiguration.h"

/*
  The pin to which the chip enable input of the radio is connected
 */
#define ROUTER_RADIO_CHIP_ENABLE_PIN 9

/*
  The pin to which the chip select input of the radio is connected
 */
#define ROUTER_RADIO_CHIP_SELECT_PIN 10

/*
  The size of the rx network message queue
 */
#define ROUTER_RX_MESSAGE_QUEUE_SIZE 32

/*
  The maximum number of consecutive reads from the radio.
  The fifo can contain 3 x 32 byte messages
 */
#define ROUTER_MAX_NR_CONSECUTIVE_RX_READS 4

/*
  The maximum number of child nodes
  => Set equal to the node's maximum nr child nodes for compatibility!
 */
#define ROUTER_MAX_NR_CHILD_NODES NODE_MAX_NR_CHILD_NODES

/*
  The maximum number of consecutive unsuccessful communication attempts to a child node
  If this maximum is exceeded, the connection to the node is reset
 */
#define ROUTER_MAX_NR_COMMUNICATION_FAULTS 5

#endif // _SWITCH_NODECONFIGURATION
