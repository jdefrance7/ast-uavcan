//##############################################################################
// File Information
//##############################################################################




//##############################################################################
// Header Files
//##############################################################################


#include "ast-uavcan.h"

//##############################################################################
// Function Definitions
//##############################################################################


UAVCAN_Driver::UAVCAN_Driver()
{
  _id = 0;
  _bitrate = 0;
}

int UAVCAN_Driver::setId(uint8_t id)
{
  _id = id;
  return 0;
}

uint8_t UAVCAN_Driver::getId()
{
  return _id;
}

int UAVCAN_Driver::setBitrate(long bitrate)
{
  if(bitrate <= 0){return -1;}
  _bitrate = bitrate;
  return 0;
}

long UAVCAN_Driver::getBitrate()
{
  return _bitrate;
}

int UAVCAN_Driver::setCallbacks(CanardOnTransferReception on_reception, CanardShouldAcceptTransfer should_accept)
{
  canardInit(
      &_canard,
      &_memory_pool,
      sizeof(_memory_pool),
      on_reception,
      should_accept,
      NULL
  );
  return 0;
}

int UAVCAN_Driver::begin()
{
  // Bitrate must be positive
  if(_bitrate == 0)
  {
    return -1;
  }
  // Allow for dynamic node allocation
  if(_id != 0)
  {
    canardSetLocalNodeID(&_canard, _id);
  }
  // Start the CAN module
  canInit(_bitrate);

  return 0;
}

int UAVCAN_Driver::clean(uint64_t timestamp_us)
{
  canardCleanupStaleTransfers(&_canard, timestamp_us);
  return 0;
}

int UAVCAN_Driver::stats(CanardPoolAllocatorStatistics* stats)
{
  *stats = canardGetPoolAllocatorStatistics(&_canard);
  return 0;
}

int UAVCAN_Driver::poll()
{
  CanardCANFrame rxf;
  return _readFrame(&rxf);
}

int UAVCAN_Driver::poll(CanardCANFrame* rxf)
{
  return _readFrame(rxf);
}

int UAVCAN_Driver::broadcast(
  uint64_t data_type_signature,
  uint16_t data_type_id,
  uint8_t* inout_transfer_id,
  uint8_t priority,
  const void* payload,
  uint16_t payload_len
)
{
  // Format and push message frame(s) onto Canard queue
  int16_t broadcast = canardBroadcast(
    &_canard,
    data_type_signature,
    data_type_id,
    inout_transfer_id,
    priority,
    payload,
    payload_len
  );

  // Return code if error
  if(broadcast < 0){return broadcast;}

  // Transmit Canard queue
  return _sendQueue();
}

int UAVCAN_Driver::broadcast(NodeStatus* node_status)
{
  // Unique ID for NodeStatus transfers
  static uint8_t node_status_transfer_id = 0;

  // Create and clear data buffer
  uint8_t data[((NODE_STATUS_DATA_TYPE_MAX_SIZE+7)/8)];
  memset(data, 0, sizeof(data));

  // Set bit offset at start
  uint32_t bit_offset = 0;

  // Encode values in data buffer
  canardEncodeScalar(data, bit_offset, 32, &(node_status->uptime_sec));
  bit_offset += 32;
  canardEncodeScalar(data, bit_offset, 2,  &(node_status->health));
  bit_offset += 2;
  canardEncodeScalar(data, bit_offset, 3,  &(node_status->mode));
  bit_offset += 3;
  canardEncodeScalar(data, bit_offset, 3,  &(node_status->sub_mode));
  bit_offset += 3;
  canardEncodeScalar(data, bit_offset, 16, &(node_status->vendor_specific_status_code));
  bit_offset += 16;

  // Format and push message frame(s) onto Canard queue
  int16_t broadcast = canardBroadcast(
    &_canard,
    NODE_STATUS_DATA_TYPE_SIGNATURE,
    NODE_STATUS_DATA_TYPE_ID,
    &node_status_transfer_id,
    CANARD_TRANSFER_PRIORITY_MEDIUM,
    data,
    sizeof(data)
  );

  // Return code if error
  if(broadcast < 0){return broadcast;}

  // Transmit Canard queue
  return _sendQueue();
}

int UAVCAN_Driver::broadcast(AngularCommand* angular_command)
{
  // Unique ID for AngularCommand transfers
  static uint8_t angular_command_transfer_id = 0;

  // Create and clear data buffer
  uint8_t data[((ANGULAR_COMMAND_DATA_TYPE_MAX_SIZE+7)/8)];
  memset(data, 0, sizeof(data));

  // Convert float32 to float16
  uint16_t quaternion_xyzw[4] = {
      canardConvertNativeFloatToFloat16(angular_command->quaternion_xyzw[0]),
      canardConvertNativeFloatToFloat16(angular_command->quaternion_xyzw[1]),
      canardConvertNativeFloatToFloat16(angular_command->quaternion_xyzw[2]),
      canardConvertNativeFloatToFloat16(angular_command->quaternion_xyzw[3]),
  };

  // Set bit offset at start
  uint32_t bit_offset = 0;

  // Encode data values
  canardEncodeScalar(data, bit_offset, 8,  &(angular_command->gimbal_id));
  bit_offset += 8;
  canardEncodeScalar(data, bit_offset, 8,  &(angular_command->mode));
  bit_offset += 8;
  canardEncodeScalar(data, bit_offset, 16, &quaternion_xyzw[0]);
  bit_offset += 16;
  canardEncodeScalar(data, bit_offset, 16, &quaternion_xyzw[1]);
  bit_offset += 16;
  canardEncodeScalar(data, bit_offset, 16, &quaternion_xyzw[2]);
  bit_offset += 16;
  canardEncodeScalar(data, bit_offset, 16, &quaternion_xyzw[3]);
  bit_offset += 16;

  // Format and push message frame(s) onto Canard queue
  int16_t broadcast = canardBroadcast(
    &_canard,
    ANGULAR_COMMAND_DATA_TYPE_SIGNATURE,
    ANGULAR_COMMAND_DATA_TYPE_ID,
    &angular_command_transfer_id,
    CANARD_TRANSFER_PRIORITY_MEDIUM,
    data,
    sizeof(data)
  );

  // Return code if error
  if(broadcast < 0){return broadcast;}

  // Transmit Canard queue
  return _sendQueue();
}

int UAVCAN_Driver::service(
  uint8_t destination_node_id,
  uint64_t data_type_signature,
  uint8_t data_type_id,
  uint8_t* inout_transfer_id,
  uint8_t priority,
  CanardRequestResponse kind,
  const void* payload,
  uint16_t payload_len
)
{
  // Format and push message frame(s) onto Canard queue
  int val = canardRequestOrRespond(
    &_canard,
    destination_node_id,
    data_type_signature,
    data_type_id,
    inout_transfer_id,
    priority,
    kind,
    payload,
    payload_len
  );

  // Return code if error
  if(val < 0){return val;}

  // Transmit Canard queue
  return _sendQueue();
}

int UAVCAN_Driver::service(GetNodeInfo* get_node_info, CanardRequestResponse kind, uint8_t remote_node_id)
{
  // Unique ID for GetNodeInfo transfers
  static uint8_t get_node_info_transfer_id = 0;

  if(kind == CanardRequest)
  {
    // Format and push message frame(s) onto Canard queue
    int request = canardRequestOrRespond(
      &_canard,
      remote_node_id,
      GET_NODE_INFO_DATA_TYPE_SIGNATURE,
      GET_NODE_INFO_DATA_TYPE_ID,
      &get_node_info_transfer_id,
      CANARD_TRANSFER_PRIORITY_MEDIUM,
      CanardRequest,
      NULL,
      0
    );

    // Return code if error
    if(request < 0){return request;}

    // Transmit Canard queue
    return _sendQueue();
  }
  else if(kind == CanardResponse)
  {
    // Create and clear data buffer
    uint8_t data[((GET_NODE_INFO_DATA_TYPE_MAX_SIZE+7)/8)];
    memset(data, 0, sizeof(data));

    // Set bit offset at start
    uint32_t bit_offset = 0;

    // Encode data values
    canardEncodeScalar(data, bit_offset, 32, &(get_node_info->status.uptime_sec));
    bit_offset += 32;
    canardEncodeScalar(data, bit_offset, 2,  &(get_node_info->status.health));
    bit_offset += 2;
    canardEncodeScalar(data, bit_offset, 3,  &(get_node_info->status.mode));
    bit_offset += 3;
    canardEncodeScalar(data, bit_offset, 3,  &(get_node_info->status.sub_mode));
    bit_offset += 3;
    canardEncodeScalar(data, bit_offset, 16, &(get_node_info->status.vendor_specific_status_code));
    bit_offset += 16;
    canardEncodeScalar(data, bit_offset, 8,  &(get_node_info->software_version.major));
    bit_offset += 8;
    canardEncodeScalar(data, bit_offset, 8,  &(get_node_info->software_version.minor));
    bit_offset += 8;
    canardEncodeScalar(data, bit_offset, 8,  &(get_node_info->software_version.optional_field_flags));
    bit_offset += 8;
    canardEncodeScalar(data, bit_offset, 32, &(get_node_info->software_version.vcs_commit));
    bit_offset += 32;
    canardEncodeScalar(data, bit_offset, 64, &(get_node_info->software_version.image_crc));
    bit_offset += 64;
    canardEncodeScalar(data, bit_offset, 8,  &(get_node_info->hardware_version.major));
    bit_offset += 8;
    canardEncodeScalar(data, bit_offset, 8,  &(get_node_info->hardware_version.minor));
    bit_offset += 8;
    for(uint8_t i = 0; i < 16; i++)
    {
      canardEncodeScalar(data, bit_offset, 8, &(get_node_info->hardware_version.unique_id[i]));
      bit_offset += 8;
    }
    for(uint16_t i = 0; i < 255; i++)
    {
      canardEncodeScalar(data, bit_offset, 8, &(get_node_info->hardware_version.certificate_of_authenticity[i]));
      bit_offset += 8;

      if(get_node_info->hardware_version.certificate_of_authenticity == 0){break;}
    }
    for(uint8_t i = 0; i < 80; i++)
    {
      canardEncodeScalar(data, bit_offset, 8, &(get_node_info->name[i]));
      bit_offset += 8;

      if(get_node_info->name[i] == 0){break;}
    }

    // Dynamic payload length in bytes (rounded up)
    uint16_t payload_length = (bit_offset + 7) / 8;

    // Format and push message frame(s) onto Canard queue
    int response = canardRequestOrRespond(
      &_canard,
      remote_node_id,
      GET_NODE_INFO_DATA_TYPE_SIGNATURE,
      GET_NODE_INFO_DATA_TYPE_ID,
      &get_node_info_transfer_id,
      CANARD_TRANSFER_PRIORITY_MEDIUM,
      CanardResponse,
      data,
      payload_length
    );

    // Return code if error
    if(response < 0){return response;}

    // Transmit Canard queue
    return _sendQueue();
  }

  // Not a request or response
  return -1;
}

int UAVCAN_Driver::_sendFrame(const CanardCANFrame* txf)
{
  // CAN message object
  st_cmd_t txMsg;

  // Create data field
  uint8_t txBuffer[8] = {};

  // Clear data field
  memset(txBuffer, 0, sizeof(txBuffer));

  // Assign data field
  txMsg.pt_data = &txBuffer[0];

  // Write data field
  memcpy(txBuffer, txf->data, txf->data_len);

  // Identifier Extension Bit = 1 (CAN 2.0B)
  txMsg.ctrl.ide = 1;

  // Extended Message ID
  txMsg.id.ext = txf->id & CANARD_CAN_EXT_ID_MASK;

  // Data Lenght Code (DLC)
  txMsg.dlc = txf->data_len;

  // Remote Transfer Request (RTR) = 0 (Message)
  txMsg.ctrl.rtr = 0;

  // Send transfer command to the CAN controller
  txMsg.cmd = CMD_TX_DATA;

  // Wait for the command to be accepted by the controller
  while(can_cmd(&txMsg) != CAN_CMD_ACCEPTED);

  // Wait for command to finish executing
  while(can_get_status(&txMsg) == CAN_STATUS_NOT_COMPLETED);

  // Return success
  return CANARD_OK;
}

int UAVCAN_Driver::_readFrame(CanardCANFrame* rxf)
{
  // Timeout counter
  uint64_t time;

  // CAN message object
  st_cmd_t rxMsg;

  // Create receive buffer
  uint8_t rxBuffer[8];

  // Clear receive buffer
  memset(rxBuffer, 0, sizeof(rxBuffer));

  // Assign receive buffer
  rxMsg.pt_data = &rxBuffer[0];

  // Send transfer command to the CAN controller
  rxMsg.cmd = CMD_RX_DATA;

  // Wait for the command to be accepted by the controller
  time = millis();
  while(can_cmd(&rxMsg) != CAN_CMD_ACCEPTED)
  {
    if(millis() - time > 1000){return -1;}
  }

  // Wait for command to finish executing
  time = millis();
  while(can_get_status(&rxMsg) == CAN_STATUS_NOT_COMPLETED)
  {
    if(millis() - time > 1000){return -1;}
  }

  // Get message ID
  rxf->id = rxMsg.id.ext;
  rxf->id |= CANARD_CAN_FRAME_EFF;

  // Get message Data
  memcpy(rxf->data, rxMsg.pt_data, rxMsg.dlc);

  // Get message DLC
  rxf->data_len = rxMsg.dlc;

  // Process received message frame
  return (int)canardHandleRxFrame(&_canard, rxf, 1000*millis());
}

int UAVCAN_Driver::_sendQueue()
{
  // Frames sent
  int frames = 0;

  // Iterate through Canard frame queue
  for(const CanardCANFrame* txf = NULL; (txf = canardPeekTxQueue(&_canard)) != NULL;)
  {
    // Send frame
    int reVal = _sendFrame(txf);

    // Success, remove frame from Canard queue
    if(reVal == CANARD_OK)
    {
      canardPopTxQueue(&_canard);
      frames++;
    }

    // Failure, return error code
    else
    {
      return reVal;
    }
  }

  // Return number of frames sent
  return frames;
}
