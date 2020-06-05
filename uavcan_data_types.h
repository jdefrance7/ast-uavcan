//##############################################################################
// File Information
//##############################################################################




//##############################################################################
// Multiple Inclusion Guard
//##############################################################################

#ifndef UAVCAN_DATA_TYPES_H
#define UAVCAN_DATA_TYPES_H

//##############################################################################
// UAVCAN Node Status Structure
//##############################################################################

#define NODE_STATUS_DATA_TYPE_ID        341
#define NODE_STATUS_DATA_TYPE_MAX_SIZE  56
#define NODE_STATUS_DATA_TYPE_SIGNATURE 0x0f0868d0c1a7c6f1

#define MAX_BROADCASTING_PERIOD_MS 1000
#define MIN_BROADCASTING_PERIOD_MS 2

#define OFFLINE_TIMEOUT_MS 3000

#define HEALTH_OK       0
#define HEALTH_WARNING  1
#define HEALTH_ERROR    2
#define HEALTH_CRITICAL 3

#define MODE_OPERATIONAL      0
#define MODE_INITIALIZATION   1
#define MODE_MAINTENANCE      2
#define MODE_SOFTWARE_UPDATE  3
#define MODE_OFFLINE          7

typedef struct
{
  uint32_t uptime_sec;
  uint8_t health;
  uint8_t mode;
  uint8_t sub_mode;
  uint16_t vendor_specific_status_code;
} NodeStatus;

//##############################################################################
// UAVCAN Software Version Structure
//##############################################################################

#define SOFTWARE_VERSION_DATA_TYPE_ID         -1
#define SOFTWARE_VERSION_DATA_TYPE_MAX_SIZE   120
#define SOFTWARE_VERSION_DATA_TYPE_SIGNATURE  0xdd46fd376527fea1

#define OPTIONAL_FIELD_FLAG_VCS_COMMIT  1
#define OPTIONAL_FIELD_FLAG_IMAGE_CRC   2

typedef struct
{
  uint8_t major;
  uint8_t minor;
  uint8_t optional_field_flags;
  uint32_t vcs_commit;
  uint64_t image_crc;
} SoftwareVersion;

//##############################################################################
// UAVCAN Hardware Version Structure
//##############################################################################

#define HARDWARE_VERSION_DATA_TYPE_ID         -1
#define HARDWARE_VERSION_DATA_TYPE_MAX_SIZE   2192
#define HARDWARE_VERSION_DATA_TYPE_SIGNATURE  0x0ad5c4c933f4a0c4

typedef struct
{
  uint8_t major;
  uint8_t minor;
  uint8_t unique_id[16];
  uint8_t certificate_of_authenticity[255];
} HardwareVersion;

//##############################################################################
// UAVCAN Angular Command Structure
//##############################################################################

#define ANGULAR_COMMAND_DATA_TYPE_ID        1040
#define ANGULAR_COMMAND_DATA_TYPE_MAX_SIZE  80
#define ANGULAR_COMMAND_DATA_TYPE_SIGNATURE 0x4af6e57b2b2be29c

#define COMMAND_MODE_ANGULAR_VELOCITY         0
#define COMMAND_MODE_ORIENTATION_FIXED_FRAME  1
#define COMMAND_MODE_ORIETNATION_BODY_FRAME   2
#define COMMAND_MODE_GEO_POI                  3

typedef struct
{
  uint8_t gimbal_id;
  uint8_t mode;
  float quaternion_xyzw[4];
} AngularCommand;

//##############################################################################
// UAVCAN Get Node Info Structure
//##############################################################################

#define GET_NODE_INFO_DATA_TYPE_ID        1
#define GET_NODE_INFO_DATA_TYPE_MAX_SIZE  3015
#define GET_NODE_INFO_DATA_TYPE_SIGNATURE 0xee468a8121c46a9e

typedef struct
{
  NodeStatus status;
  SoftwareVersion software_version;
  HardwareVersion hardware_version;
  uint8_t name[80];
} GetNodeInfo;

//##############################################################################
// Multiple Inclusion Guard
//##############################################################################

#endif // UAVCAN_DATA_TYPES_H
