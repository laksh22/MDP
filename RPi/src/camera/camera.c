#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "camera.h"
#include "../settings.h"
#include "../hub/hub.h"

int save_coord_orientation(char *coord_orientation) {
  char emptyDir[50];
  FILE *fp;
  strcpy(emptyDir, COORDS_ORIENT_DIR);
  strcat(emptyDir, coord_orientation);
  fp = fopen(emptyDir, "w");
  fclose(fp);
  return 1;
}

int create_work_directories() {
  // Create all required working directories
  char dirList[3][35] =
      {COORDS_ORIENT_DIR, IMAGES_TO_SCAN_DIR, IMAGES_FOUND_DIR};
  char emptyDir[1024];
  DIR *dir;
  int i;

  // Will remove and recreate already existing folders
  for (i = 0; i < 3; i++) {
    dir = opendir(dirList[i]);
    // Remove existing folders
    if (dir) {
      strcpy(emptyDir, "sudo rm -rf ");
      strcat(emptyDir, dirList[i]);

      // Execute command
      system(emptyDir);
      closedir(dir);
    }

    // Create folders
    mkdir(dirList[i], 0777);
  }

  return 1;
}

int count_files_in_dir(char *path) {
  int fileCount = 0;
  DIR *dir;
  struct dirent *entry;

  if ((dir = opendir(path)) != NULL) {
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) {
        // If the entry is a regular file
        fileCount++;
      }
    }
    closedir(dir);
  } else {
    // Could not open directory
    perror("[count_files] Could not open directory");
    return -1;
  }
  return fileCount;
}

void files_in_dir(char *path, char **files) {
  DIR *dir;
  struct dirent *entry;
  int i = 0;

  if ((dir = opendir(path)) != NULL) {
    printf("[process_files_in_dir] Files in folder [%s]: ", path);
    while ((entry = readdir(dir)) != NULL) {
      // Do not want ".", ".." and only want regular files
      if (entry->d_type == DT_REG) {
        printf("%d: %s\n", i, entry->d_name);

        i++;
      }

      // Do not want to send more than 5 packets
      if (i >= 5) {
        break;
      }
    }
    closedir(dir);
  } else {
    // Could not open directory
    perror("[process_files_in_dir] Could not open directory");
  }
}

void process_files_in_dir(char *path) {
  DIR *dir;
  struct dirent *entry;
  int i = 0;
  char buf[MAX];

  if ((dir = opendir(path)) != NULL) {
    printf("[files_in_dir] Printing files in folder [%s]\n", path);
    while ((entry = readdir(dir)) != NULL) {
      // Do not want ".", ".." and only want regular files
      if (entry->d_type == DT_REG) {
        printf("%d: %s\n", i, entry->d_name);

        /* Be careful when using strtok as it is destructive of the input, it
         * will zero out the delimiter, which is the file extension.
         *
         * This is okay in our case as we will not use the file extension
         * downstream in any of our code.
         */
        strcpy(buf, "@bBLOCK");
        strcat(buf, strtok(files[i], strrchr(files[i], '.')));
        strcat(buf, "!");

        /* Bluetooth is insensitive to source, source does not matter.
         * Put the message into the Bluetooth queue.
         */
        distribute_command(buf, 'b');
        i++;
      }
    }
    printf("[files_in_dir] Done printing files in folder [%s]\n", path);
    closedir(dir);
  } else {
    // Could not open directory
    perror("[files_in_dir] Could not open directory");
  }
}

void *read_img_labels() {
  // Thread to check if image recognition is done
  int fileCount;
  char buf[MAX];

  // Ensure required folders are created
  create_work_directories();

  // Endless loop
  while (1) {
    // Get number of files in COORDS_ORIENT_DIR folder
    fileCount = count_files_in_dir(COORDS_ORIENT_DIR);

    // Check if the DONE file is created
    if (access(DONE_FILE, F_OK) != -1 && fileCount == 1) {
      // DONE file exists and is the only file
      process_files_in_dir(IMAGES_FOUND_DIR);

      // Breaking out of endless-loop
      break;
    }

    sleep(1);
  }
}
