/*?*************************************************************************
*                           Switch_NetworkConfiguration.h
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

#ifndef _SWITCH_NETWORKCONFIGURATION
#define _SWITCH_NETWORKCONFIGURATION

/*
  The channel on which the network operates
  Frequency = 2400 + channel
  Must be a value in [0x00, 0x7D]
 */
#define NC_NETWORK_CHANNEL 0x36

/*
  
 */
#define NC_PAYLOAD_SIZE 32

/*
  The number of TX retries in case of failure
 */
#define NC_NR_RETRIES 15

/*
  The delay between two TX retries in milliseconds
 */
#define NC_RETRY_DELAY_MS 20

/*
  The power amplifier level of the rf transmissions
 */
#define NC_POWER_AMPLIER_LEVEL RF24_PA_MAX

/*
  The data rate of the rf transmissions
 */
#define NC_DATA_RATE RF24_2MBPS

/*
  The cyclic redundancy check length
 */
#define NC_CRC_LENGTH RF24_CRC_16

/*
  Broadcast pipe address:
    When the node is not part of a network, it broadcasts its information
    over the broadcast pipe. This information is used by other nodes to
    integrate the node in the network. When the node is part of a network
    it listens to other node's broadcasts over the broadcast pipe on RX P0.
 */
#define NC_BROADCAST_PIPE 0xFF29A45D3cULL

/*
  Node receive pipes:
    Every node receives broadcasts on RX pipe0.
    The other pipes are for directed traffic.
    RX P1    => traffic from the parent node
    RX P2-P5 => traffic from the child nodes
    The full RX pipe address is the suffix in byte 0, followed by the device address 
 */
#define NC_RX_P1_SUFFIX 0x5a
#define NC_RX_P2_SUFFIX 0x69
#define NC_RX_P3_SUFFIX 0x96
#define NC_RX_P4_SUFFIX 0xa5
#define NC_RX_P5_SUFFIX 0xc3



#endif // _SWITCH_NETWORKCONFIGURATION
