//##############################################################################
// File Information
//##############################################################################



//##############################################################################
// Multiple Inclusion Guard
//##############################################################################

#ifndef AST_UAVCAN_H
#define AST_UAVCAN_H

//##############################################################################
// Arduino Support Library
//##############################################################################

#include <Arduino.h>

//##############################################################################
// AST CAN Driver Library
//##############################################################################

#include <ASTCanLib.h>

//##############################################################################
// Canard Source File
//##############################################################################

#include "canard.h"

//##############################################################################
// UAVCAN Data Type Structures & Constants
//##############################################################################

#include "uavcan_data_types.h"

//##############################################################################
// UAVCAN Node Structure
//##############################################################################

typedef GetNodeInfo
/*{
    NodeStatus status
    {
      uint32_t uptime_sec;
      uint8_t health;
      uint8_t mode;
      uint8_t sub_mode;
      uint16_t vendor_specific_status_code;
    };
    SoftwareVersion software_version
    {
      uint8_t major;
      uint8_t minor;
      uint8_t optional_field_flags;
      uint32_t vcs_commit;
      uint64_t image_crc;
    };
    HardwareVersion hardware_version
    {
      uint8_t major;
      uint8_t minor;
      uint8_t unique_id[16];
      uint8_t certificate_of_authenticity[255];
    };
    uint8_t name[80];
}*/
UAVCAN_Node;

//##############################################################################
// UAVCAN Memory Size
//##############################################################################

// #define CANARD_MEMORY_POOL_SIZE 128   //  4 blocks
#define CANARD_MEMORY_POOL_SIZE 256   //  8 blocks
// #define CANARD_MEMORY_POOL_SIZE 512   // 16 blocks
// #define CANARD_MEMORY_POOL_SIZE 1024  // 32 blocks

//##############################################################################
// UAVCAN Driver Class
//##############################################################################

class UAVCAN_Driver
{
public:

  // Constructor
  UAVCAN_Driver();

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
  int clean(uint64_t timestamp_us);

  // Returns the stats of the Canard pool
  int stats(CanardPoolAllocatorStatistics* stats);

  // Polls CAN module for single frame
  int poll();

  // Polls CAN module for single frame
  int poll(CanardCANFrame* rxf);

  // Generic broadcast transfer
  int broadcast(
    uint64_t data_type_signature,
    uint16_t data_type_id,
    uint8_t* inout_transfer_id,
    uint8_t priority,
    const void* payload,
    uint16_t payload_len
  );

  // Sends a NodeStatus broadcast transfer
  int broadcast(NodeStatus* node_status);

  // Sends an AngularCommand broadcast transfer
  int broadcast(AngularCommand* angular_command);

  // Generic request/response transfer
  int service(
    uint8_t destination_node_id,
    uint64_t data_type_signature,
    uint8_t data_type_id,
    uint8_t* inout_transfer_id,
    uint8_t priority,
    CanardRequestResponse kind,
    const void* payload,
    uint16_t payload_len
  );

  // Sends a GetNodeInfo request/response transfer
  int service(GetNodeInfo* get_node_info, CanardRequestResponse kind, uint8_t remote_node_id);

private:

  // Node id
  uint8_t _id;

  // CAN bitrate
  long _bitrate;

  // Canard library instance
  CanardInstance _canard;

  // Canard memory pool
  uint8_t _memory_pool[CANARD_MEMORY_POOL_SIZE];

  // Sends a single Canard CAN frame
  int _sendFrame(const CanardCANFrame* txf);

  // Reads a single Canard CAN frame
  int _readFrame(CanardCANFrame* rxf);

  // Sends all frames in the Canard queue
  int _sendQueue();
};

//##############################################################################
// Multiple Inclusion Guard
//##############################################################################

#endif // AST_UAVCAN_H
