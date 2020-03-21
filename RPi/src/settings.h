#ifndef RPI_RPI_SETTINGS_H_
#define RPI_RPI_SETTINGS_H_

#define MAX 1024
#define SA struct sockaddr
#define SERIAL_PORT "/dev/ttyACM0"
#define BAUD 9600
#define QSIZE 100
#define NUM_THREADS 9

// Camera directories
#define COORDS_ORIENT_DIR "/home/pi/Desktop/coords_orien/"
#define IMAGES_TO_SCAN_DIR "/home/pi/Desktop/images_to_scan/"
#define IMAGES_FOUND_DIR "/home/pi/Desktop/images_found/"
#define DONE "DONE"
#define DONE_FILE COORDS_ORIENT_DIR DONE

#endif //RPI_RPI_SETTINGS_H_
