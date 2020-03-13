#ifndef RPI_RPI_SRC_PICAMERA_PICAMERA_H_
#define RPI_RPI_SRC_PICAMERA_PICAMERA_H_

// Save the coordinates and orientation sent by TCP
int save_coord_orientation(char* coord_orientation);

// Check and create required folders
int create_work_directories();

// Count the number of files in a directory
int count_files_in_dir(char *path);

// Get all file names in directory and file count
int files_in_dir(char *path, char ***files);

// Read all files in directory and send relevant image data to Bluetooth
int process_file_in_dir(char *path, char ***files);

// Thread to reads the labels containing id,x,y to be sent to Bluetooth
void *read_img_labels();

#endif //RPI_RPI_SRC_PICAMERA_PICAMERA_H_
