#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdint.h>
#include "bluetooth.h"
#include "../settings.h"

// BT Variables
// Unique UUID 00000000-0000-0000-0000-000000000000
uint32_t svc_uuid_int[] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
int bt_sock, client;

// To create and establish variables
int bt_connect() {
  // Initialise connection variables
  char buf[MAX];
  struct sockaddr_rc loc_addr, rem_addr;
  socklen_t opt = sizeof(rem_addr);

  // Registers the bluetooth service
  sdp_session_t *session = register_service(BT_PORT);

  // Creates an RFCOMM bluetooth socket for communication
  bt_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (bt_sock == -1) {
    perror("bt_connect: Error encountered when creating BT socket: ");
    return 0;
  } else {
    printf("bt_connect: Creation of BT socket successful....\n");
  }

  // Clears the memory of the loc_addr variable
  bzero(&loc_addr, sizeof(loc_addr));

  // Assigns the respective bluetooth variables for binding
  loc_addr.rc_family = AF_BLUETOOTH;
  // Sets which local bluetooth device to be used
  loc_addr.rc_bdaddr = *BDADDR_ANY;
  // Sets which RFCOMM channel to be used
  loc_addr.rc_channel = (uint8_t) BT_PORT;


  // Binds socket to port 1 of the first available local bluetooth adapter
  if ((bind(bt_sock, (SA *) &loc_addr, sizeof(loc_addr))) != 0) {
    perror("[bt_connect]: Error encountered when trying to bind BT socket: ");
    return 0;
  } else {
    printf("[bt_connect]: Binding of BT socket successful....\n");
  }

  // Configure server to listen for incoming connections
  if (listen(bt_sock, 1) != 0) {
    perror("[bt_connect]: Error encountered when listing for BT connections: ");
    return 0;
  } else {
    printf(
        "[bt_connect]: Bluetooth Server is now listening for connections...\n");
  }

  // Accepts the incoming data packet from client
  client = accept(bt_sock, (SA *) &rem_addr, &opt);
  if (client < 0) {
    perror(
        "[bt_connect]: Error encountered when trying to accept BT clients....: ");
    return 0;
  } else {
    printf("[bt_connect]: BT Server has accepted the client successfully...\n");
  }

  ba2str(&rem_addr.rc_bdaddr, buf);
  fprintf(stderr, "[bt_connect]: Accepted connection from %s\n", buf);
  memset(buf, 0, sizeof(buf));

  return 1;
}