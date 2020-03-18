#ifndef RPI_RPI_SRC_PICAMERA_PICAMERA_H_
#define RPI_RPI_SRC_PICAMERA_PICAMERA_H_

#include "../arraylist/arraylist.h"

// Save the coordinates and orientation sent by TCP
int save_coord_orientation(char *coord_orientation);

// Check and create required folders
int create_work_directories();

// Count number of files in a directory
int count_files_in_dir(char *path)

// Get all file names in directory and save it into a string array
// returns the number of files in directory
int files_in_dir(char *path, char ***files);

// Get all file names in directory and save it into a Arraylist
// returns the number of files in directory
int l_files_in_dir(char *path, arraylist *l);

// Thread to reads the labels containing id,x,y to be sent to Bluetooth
void *read_img_labels();

#endif //RPI_RPI_SRC_PICAMERA_PICAMERA_H_
