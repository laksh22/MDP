#ifndef RPI_RPI_BLUETOOTH_BLUETOOTH_H_
#define RPI_RPI_BLUETOOTH_BLUETOOTH_H_

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#define BT_PORT 3

// To create and establish service discovery p
sdp_session_t *register_service(uint8_t rfcomm_channel);

// To create and establish variables
int bt_connect();

// To disconnect a Bluetooth client
void bt_disconnect();

// To reconnect a client
void bt_reconnect();

// To setup thread to read from rfcomm channel
void *bt_reader_create(void *args);

// To read from the Bluetooth port if data is available
char *bt_read();

// Setup thread to start writing data received from Bluetooth to device
void* bt_sender_create(void *args);

#endif //RPI_RPI_BLUETOOTH_BLUETOOTH_H_
