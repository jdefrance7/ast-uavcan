//------------------------------------------------------------------------------
// File Information



//------------------------------------------------------------------------------
// Multiple Inclusion Guard

#ifndef AST_UAVCAN_H
#define AST_UAVCAN_H

//------------------------------------------------------------------------------
// AST CAN Driver Library

#include <ASTCanLib.h>

//------------------------------------------------------------------------------
// Canard Source File

#include "canard.h"

//------------------------------------------------------------------------------
// UAVCAN Data Type Structures & Constants

#include "uavcan_data_types.h"

//------------------------------------------------------------------------------
// UAVCAN Node Structure

typedef GetNodeInfo
/*
  NodeStatus status;
  SoftwareVersion software_version;
  HardwareVersion hardware_version;
  uint8_t name[80];
*/
UAVCAN_NODE;

//------------------------------------------------------------------------------
// UAVCAN Driver Class & Configs

#define CANARD_MEMORY_POOL_SIZE 1024

class UAVCAN_DRIVER
{
public:

  // Constructor
  UAVCAN_DRIVER();

  // ID setter/getter
  int setId(uint8_t id);
  uint8_t getId();

  // Bitrate setter/getter
  int setBitrate(long bitrate);
  long getBitrate();

  // Callback setter
  int setCallbacks(CanardOnTransferReception on_reception, CanardShouldAcceptTransfer should_accept);

  // Starts the CAN module
  int begin();

  // Frees the memory held by stale transfers
  int clean(uint64_t timestamp);

  // Polls CAN module for single frame
  int poll();

  // Sends a NodeStatus broadcast transfer
  int broadcast(NodeStatus* node_status);

  // Sends an AngularCommand broadcast transfer
  int broadcast(AngularCommand* angular_command);

  // Sends a GetNodeInfo request/response transfer
  int service(GetNodeInfo* get_node_info, CanardRequestResponse kind, uint8_t remote_node_id);

private:

  // Node id
  uint8_t _id;

  // CAN bitrate
  long _bitrate;

  // Callback flag
  bool _callbacks;

  // Ready flag
  bool _ready;

  // Canard library instance
  CanardInstance _canard;

  // Canard memory pool
  uint8_t _memory_pool[CANARD_MEMORY_POOL_SIZE];

  // Sends a single Canard CAN frame
  int _sendFrame(CanardCANFrame* txf);

  // Reads a single Canard CAN frame
  int _readFrame(CanardCANFrame* rxf);

  // Sends all frames in the Canard queue
  int _sendQueue();
};

#endif // AST_UAVCAN_H
