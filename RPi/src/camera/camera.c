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

int files_in_dir(char *path, char ***files) {
  DIR *dir;
  struct dirent *entry;
  int n = 0;

  if ((dir = opendir(path)) != NULL) {
    printf("[files_in_dir] Files in folder [%s]: ", path);
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) {
        *files = realloc(*files, sizeof(**files) * (n + 1));
        // Remember to free from calling function
        (*files)[n] = malloc(strlen(entry->d_name));
        strcpy((*files)[n], entry->d_name);
        n++;
      }
    }
    closedir(dir);
    return n;
  } else {
    // Could not open directory
    perror("[files_in_dir] Could not open directory");
    return 0;
  }
}

int process_file_in_dir(char *path, char ***files) {
  DIR *dir;
  struct dirent *entry;
  char *file_extension;
  size_t n = 0;

  if ((dir = opendir(path)) != NULL) {
    printf("[process_files_in_dir] Printing files in folder [%s]\n", path);
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) {
//        *files = realloc(*files, sizeof(**files) * (n + 1));

        // Get file extension
//        file_extension = malloc(16);
//        strcpy(file_extension, strrchr(entry->d_name, '.'));

//        (*files)[n] = malloc(strlen(entry->d_name) + 10);
//        strcpy((*files)[n], "@bBLOCK:");
//        strcat((*files)[n], entry->d_name);
//        (*files)[n][strlen((const char *) (*files)[n])
//            - strlen(file_extension)] = '\0';
//        strcat((*files)[n], "!");
//        printf("Constructed string: %s\n", (*files)[n]);
//
//        free(file_extension);
        distribute_command(entry->d_name, 'b');
        n++;
      }
    }
    printf("[process_files_in_dir] Done printing files in folder [%s]\n", path);
    closedir(dir);
    return n;
  } else {
    // Could not open directory
    perror("");
    return 0;
  }
}

void *read_img_labels() {
  // Thread to check if image recognition is done
  int fileCount;
  char **files = NULL;
  int i = 0;

  // Ensure required folders are created
  create_work_directories();

  // Endless loop
  while (1) {
    // Get number of files in COORDS_ORIENT_DIR folder
    fileCount = count_files_in_dir(COORDS_ORIENT_DIR);

    // Check if the DONE file is created
    if (access(DONE_FILE, F_OK) != -1 && fileCount == 1) {
      // DONE file exists and is the only file

      // Process file in dir
      fileCount = process_file_in_dir(IMAGES_FOUND_DIR, &files);

      // Ensure messages are sent before free-ing
      sleep(10);
//      for (i = 0; i < fileCount; i++) {
//        printf("Freeing: %s\n", files[i]);
//        free(files[i]);
//      }
//      free(files);

      // Breaking out of endless-loop
      break;
    }

    sleep(1);
  }
}

