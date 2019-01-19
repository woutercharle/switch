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

#ifndef _SWITCH_NODECONFIGURATION
#define _SWITCH_NODECONFIGURATION

// switch includes
#include "../Switch_Base/Switch_CompilerConfiguration.h"

/*
  The pin to which the chip enable input of the radio is connected
 */
#define NODE_RADIO_CHIP_ENABLE_PIN 8

/*
  The pin to which the chip select input of the radio is connected
 */
#define NODE_RADIO_CHIP_SELECT_PIN 10

/*
  The time in milliseconds between two broadcast messages
 */
#define NODE_BROADCAST_INTERVAL_MS 5000

/*
  The size of the rx network message queue
 */
#define NODE_RX_MESSAGE_QUEUE_SIZE 10

/*
  The maximum number of consecutive reads from the radio.
  The fifo can contain 3 x 32 byte messages
 */
#define NODE_MAX_NR_CONSECUTIVE_RX_READS 4

/*
  The maximum number of child nodes per node
 */
//#define NODE_MAX_NR_CHILD_NODES 4
#define NODE_MAX_NR_CHILD_NODES 1

/*
  The maximum number of consecutive unsuccessful communication attempts to the parent node
  If this maximum is exceeded, the node is reset
 */
#define NODE_MAX_NR_COMMUNICATION_FAULTS 3

/*
  The maximum duration of communication silence between a node and it's parent in seconds
 */
#define NODE_MAX_PARENT_COMMUNICATION_SILENCE_MS 30000

/*
  The maximum duration of communication silence between a node and it's parent in seconds
 */
#define NODE_MAX_TXRX_COMMUNICATION_SILENCE_MS 2500

#endif // _SWITCH_NODECONFIGURATION
