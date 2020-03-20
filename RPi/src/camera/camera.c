#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "camera.h"
#include "../hub/hub.h"
#include "../settings.h"

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
  int file_count = 0;
  DIR *dir;
  struct dirent *entry;

  if ((dir = opendir(path)) != NULL) {
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) {
        // Do not want invisible files like .DS_Store
        if (entry->d_name[0] == '.') {
          continue;
        }
        // If the entry is a regular file
        file_count++;
      }
    }
    closedir(dir);
  } else {
    // Could not open directory
    perror("[count_files] Could not open directory");
    return -1;
  }
  return file_count;
}

int files_in_dir(char *path, char ***files) {
  DIR *dir;
  struct dirent *entry;
  int file_count = 0;

  if ((dir = opendir(path)) != NULL) {
    printf("[files_in_dir] Files in folder [%s]: ", path);
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) {
        *files = realloc(*files, sizeof(**files) * (file_count + 1));
        // Remember to free from calling function
        (*files)[file_count] = malloc(strlen(entry->d_name) + 1);
        strcpy((*files)[file_count], entry->d_name);
        file_count++;
      }
    }
    closedir(dir);
    return file_count;
  } else {
    // Could not open directory
    perror("[files_in_dir] Could not open directory");
    return 0;
  }
}

int l_files_in_dir(char *path, arraylist *l) {
  DIR *dir;
  struct dirent *entry;
  char *filename = NULL;
  int file_count = 0;
  unsigned int idx = 0;

  if ((dir = opendir(path)) != NULL) {
//    printf("[l_files_in_dir] New files in folder [%s]: \n", path);
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) {
        // Do not want invisible files like .DS_Store
        if (entry->d_name[0] == '.') {
          continue;
        } else if (l->size == 0) {
          // Nothing in the list, insert into Arraylist
          // +2 for delimiter and null terminator char
          filename = malloc(strlen(entry->d_name) + 2);
          strcpy(filename, entry->d_name);
          arraylist_add(l, filename);
          file_count++;
          continue;
        }

        // Perform string comparison and insert into list if not exists
        for (idx = 0; idx < l->size; idx++) {
          if (strcmp(arraylist_get(l, idx), entry->d_name) == 0) {
            // String found, nothing needs to be done
            break;
          } else if (idx == l->size - 1) {
            // Looped to end of list and filename does not exist
            // Insert filename into Arraylist
            // +2 for delimiter and null terminator char
            filename = malloc(strlen(entry->d_name) + 2);
            strcpy(filename, entry->d_name);
            arraylist_add(l, filename);
          }
        }
        file_count++;
      }
    }
    closedir(dir);
    return file_count;
  } else {
    // Could not open directory
    perror("[l_files_in_dir] Could not open directory");
    return 0;
  }
}

void *read_img_labels() {
  // Thread to check if image recognition is done
  arraylist *l;
  unsigned int pre_list_idx = 0;
  unsigned int loop_idx = 0;
  int file_count;
  int cnt = 0;
  char * filename;

  // Ensure required folders are created
  create_work_directories();

  // Initialise Arraylist
  l = arraylist_create();

  // Endless loop
  while (1) {
    // Find files in IMAGES_FOUND_DIR
    l_files_in_dir(IMAGES_FOUND_DIR, l);

//    printf("[read_img_labels] l->size: %d\n", l->size);
//    printf("[read_img_labels] pre_list_idx: %d\n", l->size);

    if (l->size > pre_list_idx) {
      // New filenames found, send new filenames to RPi
      for (loop_idx = pre_list_idx; loop_idx < l->size; loop_idx++) {
        filename = arraylist_get(l, loop_idx);
        // C in RPi does not play well with exclamation point as the last char
        *(filename + strlen(filename)) = '!';
        *(filename + strlen(filename) + 1) = '\0';
        distribute_command(filename, 'b');
      }
      pre_list_idx = l->size;
    }

    // Get number of files in COORDS_ORIENT_DIR and filenames
    file_count = count_files_in_dir(COORDS_ORIENT_DIR);

    // Check if the DONE file is created in COORDS_ORIENT_DIR
    if (access(DONE_FILE, F_OK) != -1 && file_count == 1) {
      // DONE file exists and is the only file
      // Break out of endless-loop and proceed to end of function/thread
      break;
    }

    //Pause thread for 1 second
    sleep(1);
  }

  // Free up memory used by Arraylist
  for (loop_idx = 0; loop_idx < l->size; loop_idx++) {
    printf("[read_img_labels] Freeing Arraylist idx: %d\n", loop_idx);
    free(arraylist_get(l, loop_idx));
  }
  arraylist_destroy(l);
}

