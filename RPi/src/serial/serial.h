#ifndef RPI_SERIAL_H
#define RPI_SERIAL_H

extern int fd_serial;

// Connect and verify the use of serial connection
int serial_connect();

// Disconnect the use of serial connection
void serial_disconnect();

// To reconnect serial client
void serial_reconnect();

// Read from the serial port if data is available
char *serial_read();

// Setup thread to read from serial port
void *serial_reader_create(void *args);

// Send data to device through serial port
int serial_send(char *msg);

// Setup a thread to start writing data received from serial port to devices
void *serial_sender_create(void *args);

#endif //RPI_SERIAL_H
