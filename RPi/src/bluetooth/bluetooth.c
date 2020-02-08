#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "bluetooth.h"
#include "../settings.h"
#include "../queue/queue.h"
#include "../hub/hub.h"

// BT Variables
bdaddr_t bdaddr_any = {0, 0, 0, 0, 0, 0};
bdaddr_t bdaddr_local = {0, 0, 0, 0xff, 0xff, 0xff};

// UUID 00001101-0000-1000-8000-00805f9b34fb
uint32_t svc_uuid_int[] = {0x01110000, 0x00100000, 0x80000080, 0xFB349B5F};
int bt_sock, client;

sdp_session_t *register_service(uint8_t rfcomm_channel) {
  // Variables initialised for the registration of the sdp server
  const char *service_name = "2020Group01 Bluetooth server";
  const char *svc_dsc = "MDP Bluetooth Server";
  const char *service_prov = "2020Group01";

  uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid, svc_class_uuid;

  sdp_list_t *l2cap_list = 0, *rfcomm_list = 0, *root_list = 0, *proto_list = 0,
      *access_proto_list = 0, *svc_class_list = 0, *profile_list = 0;

  sdp_data_t *channel = 0;
  sdp_profile_desc_t profile;
  sdp_record_t record = {0};
  sdp_session_t *session = 0;

  char str[256] = "";

  // Set the general service ID
  sdp_uuid128_create(&svc_uuid, &svc_uuid_int);
  sdp_set_service_id(&record, svc_uuid);
  sdp_uuid2strn(&svc_uuid, str, 256);
  printf("[register_service]: Registering UUID %s\n", str);

  // Set the service class
  sdp_uuid16_create(&svc_class_uuid, SERIAL_PORT_SVCLASS_ID);
  svc_class_list = sdp_list_append(0, &svc_class_uuid);
  sdp_set_service_classes(&record, svc_class_list);

  // Set the Bluetooth profile information
  sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
  profile.version = 0x0100;
  profile_list = sdp_list_append(0, &profile);
  sdp_set_profile_descs(&record, profile_list);

  // Allows the service record to be publicly browsable
  sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
  root_list = sdp_list_append(0, &root_uuid);
  sdp_set_browse_groups(&record, root_list);

  // Set l2cap information
  sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
  l2cap_list = sdp_list_append(0, &l2cap_uuid);
  proto_list = sdp_list_append(0, l2cap_list);

  // Register the RFCOMM channel for RFCOMM sockets
  sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
  channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
  rfcomm_list = sdp_list_append(0, &rfcomm_uuid);
  sdp_list_append(rfcomm_list, channel);
  sdp_list_append(proto_list, rfcomm_list);

  access_proto_list = sdp_list_append(0, proto_list);
  sdp_set_access_protos(&record, access_proto_list);

  // Set the name, provider, and description
  sdp_set_info_attr(&record, service_name, service_prov, svc_dsc);

  // Connect to the local SDP server, register the service record, and disconnect
  session = sdp_connect(&bdaddr_any, &bdaddr_local, SDP_RETRY_IF_BUSY);
  sdp_record_register(session, &record, 0);

  // Cleanup
  sdp_data_free(channel);
  sdp_list_free(l2cap_list, 0);
  sdp_list_free(rfcomm_list, 0);
  sdp_list_free(root_list, 0);
  sdp_list_free(access_proto_list, 0);
  sdp_list_free(svc_class_list, 0);
  sdp_list_free(profile_list, 0);

  return session;
}

int bt_connect() {
  // Initialise connection variables
  char buf[MAX];
  struct sockaddr_rc loc_addr = {0}, rem_addr = {0};
  socklen_t opt = sizeof(rem_addr);

  // Local Bluetooth Adapter
  // Assigns the respective Bluetooth variables for binding
  loc_addr.rc_family = AF_BLUETOOTH;
  // Sets which local Bluetooth device to be used
  loc_addr.rc_bdaddr = *BDADDR_ANY;
  // Sets which RFCOMM channel to be used
  loc_addr.rc_channel = (uint8_t) BT_PORT;

  // Registers the Bluetooth service
  sdp_session_t *session = register_service(BT_PORT);

  // Creates an RFCOMM Bluetooth socket for communication
  bt_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (bt_sock == -1) {
    perror("[bt_connect]: Error encountered when creating BT socket: ");
    return 0;
  } else {
    printf("[bt_connect]: Creation of BT socket successful...\n");
  }

  // Clears the memory of the loc_addr variable
  bzero(&loc_addr, sizeof(loc_addr));

  // Binds socket to port 1 of the first available local bluetooth adapter
  if ((bind(bt_sock, (struct sockaddr *) &loc_addr, sizeof(loc_addr))) != 0) {
    perror("[bt_connect]: Error encountered when trying to bind BT socket: ");
    return 0;
  } else {
    printf("[bt_connect]: Binding of BT socket successful..\n");
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
  client = accept(bt_sock, (struct sockaddr *) &rem_addr, &opt);
  if (client < 0) {
    perror(
        "[bt_connect]: Error encountered when trying to accept BT clients...: ");
    return 0;
  } else {
    printf("[bt_connect]: BT Server has accepted the client successfully...\n");
  }

  ba2str(&rem_addr.rc_bdaddr, buf);
  fprintf(stderr, "[bt_connect]: Accepted connection from %s\n", buf);
  memset(buf, 0, sizeof(buf));

  return 1;
}

void bt_disconnect() {
  if (!close(client) && !close(bt_sock)) {
    printf("[bt_disconnect]: Bluetooth connection is closed successfully!\n");
  } else {
    perror(
        "[bt_disconnect]: Error encountered when trying to close Bluetooth connection: ");
  }
}

void *bt_reader_create(void *args) {
  char read_buf[MAX];
  char *rpointer;

  while (1) {
    rpointer = bt_read();
    if (rpointer) {
      strcpy(read_buf, rpointer);
      memset(rpointer, '\0', MAX);
      distribute_command(read_buf, 'b');
    } else {
      perror(
          "[bt_reader_create]: Error encountered when receiving data from bt_read:  ");
    }
  }
}

char *bt_read() {
  int count = 0;
  char bt_buf[MAX];
  char *p;
  int bytes_read = 0;

  while (1) {
    bytes_read = read(client, &bt_buf[count], MAX - count);
    count += bytes_read;
    if (bytes_read > 0) {
      if (strlen(bt_buf) == 0) {
        continue;
      } else if (bt_buf[0] == '@') {
        if (bt_buf[count - 1] != '!') {
          continue;
        } else {
          printf("[bt_read]: Received [%s] from Bluetooth client connection\n",
                 bt_buf);
          bt_buf[count] = '\0';
          p = bt_buf;
          return p;
        }
      } else {
        printf(
            "[bt_read]: Invalid string [%s] received, please send a new command\n",
            bt_buf);
      }
    } else {
      perror("[bt_read]: Error encountered when trying to read from Bluetooth: ");
      bt_reconnect();
      return NULL;
    }
  }
}

void bt_reconnect() {
  int conn = 0;

  while (!conn) {
    printf("[bt_reconnect]: Attempting to restart Bluetooth server...\n");
    bt_disconnect();
    conn = bt_connect();
    sleep(1);
  }

  printf(
      "[bt_reconnect]: Bluetooth services have been successfully reconnected!\n");
}

void *bt_sender_create(void *args) {
  char *q;

  // Endless loop
  while (1) {
    if (!isEmpty(b_queue)) {
      pthread_mutex_lock(&lock);
      q = dequeue(b_queue);
      write_hub(q, 'b');
      pthread_mutex_unlock(&lock);
    }
  }
}

int bt_send(char *msg) {
  int bytes_write = 0;
  char send[MAX];

  if (strlen(msg) > 0) {
    strcpy(send, msg);
    strcat(send, "\n");
    bytes_write = write(client, msg, strlen(msg));
    if (bytes_write > 0) {
      printf("[bt_send]: RPi send message [%s] to BT.\n", msg);
      fflush(stdout);
      return 1;
    } else {
      perror("[bt_send]: Encountered error when RPi tried to send to BT: ");
    }
  }
  return 0;
}