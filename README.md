# AST UAVCAN

UAVCAN driver class for the AST-CAN485 development board sold by SparkFun.

## Contributors

Joe DeFrance (jdefrance7)

## Getting Started

1. Download the Arduino IDE if you do not already have it installed.

2. Open a terminal and navigate to the Arduino IDE libraries directory (typically found in `[User]/Documents/Arduino/libraries`)

3. Type `git clone` into the terminal and then copy and paste the link provided by clicking the green `Clone or Download` button on this page.

4. Add the AST-CAN485 Development Board to the Arduino IDE by following SparkFun's AST-CAN485 hookup guide section [Board Installation Using Boards Manager](https://learn.sparkfun.com/tutorials/ast-can485-hookup-guide?_ga=2.39481377.365903456.1581038177-271346267.1574810854)

5. Download the `ASTCanLib` library for the Arduino IDE by following SparkFun's AST-CAN485 hookup guide section [Manual Library Installation](https://learn.sparkfun.com/tutorials/ast-can485-hookup-guide?_ga=2.39481377.365903456.1581038177-271346267.1574810854)

## Library Contents

### UAVCAN Sources

`canard.h` - Canard header file from [UAVCAN/libcanard](https://github.com/UAVCAN/libcanard/tree/legacy-v0) tree `legacy-v0`.

`canard.c` - Canard source file from [UAVCAN/libcanard](https://github.com/UAVCAN/libcanard/tree/legacy-v0) tree `legacy-v0`.

`canard_internals.h` - Canard internal support file from [UAVCAN/libcanard](https://github.com/UAVCAN/libcanard/tree/legacy-v0) tree `legacy-v0`.

### Custom Files

`uavcan_data_types.h` - UAVCAN data type structures and information.

`ast-uavcan.h` - UAVCAN driver header and class declaration.

`ast-uavcan.cpp` - UAVCAN driver class function definitions.

## Using the Library

### Setup

1. Include the library header `<ast-uavcan.h>` in your sketch.

2. Create an instance of the `UAVCAN_Driver` class.

3. Create an instance of the `UAVCAN_Node` structure. (Recommended)

4. Define the callback handler functions `shouldAcceptTransfer` and `onTransferReceived` (see `canard.h` for details).

5. Set the driver's node ID using `setId()`

6. Set the driver's bitrate using `setBitrate()`

7. Assign the driver's callback functions using `setCallbacks()`

8. Start the driver using `begin()`


### Broadcasts

Broadcasts can be sent in two ways:

#### 1. Generic

Using the generic broadcast function. Please note that the `inout_transfer_id` must be defined outside of this function and be declared static. Because of this, you cannot mix using the generic broadcast function and specific broadcast function for the same data types. 

```c
int broadcast(
  uint64_t data_type_signature,
  uint16_t data_type_id,
  uint8_t* inout_transfer_id,
  uint8_t priority,
  const void* payload,
  uint16_t payload_len
);
```

#### 2. Specific

Using a specific broadcast function. Please note that this function handles declaring and defining the static `inout_transfer_id` variable. However, this means that you cannot mix this function with the generic broadcast function for the same data types. Additionally, all data type transfers are currently given `CANARD_TRANSFER_PRIORITY_MEDIUM`, though this can be changed in their respective functions.

```c
int broadcast(NodeStatus* node_status);

int broadcast(AngularCommand* angular_command);

...
```

### Requests/Responses

Requests and responses can be sent in a similar way to Broadcasts but using the generic or specific `service()` funcitons with variable `CanardRequest` or `CanardResponse` transfer types.
