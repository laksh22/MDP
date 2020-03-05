import io
import sys
import time
from os import listdir, remove
from os.path import isfile, join, getmtime

import picamera
from PIL import Image

COORDS_ORIEN_DIR = "/home/pi/Desktop/coords_orien/"
IMAGES_TO_SCAN_DIR = "/home/pi/Desktop/images_to_scan/"
IMAGES_FOUND_DIR = "/home/pi/Desktop/images_found/"
DONE_FILE = COORDS_ORIEN_DIR + "DONE"

with picamera.PiCamera() as camera:
    # Set the camera's resolution to VGA @80fps and give it a couple of seconds
    # to warm up so that it can measure exposure, etc.
    camera.resolution = (640, 480)
    camera.framerate = 80
    time.sleep(3)

    # Poll path for new files
    while True:
        # Get all files in directory, will ignore hidden file and folders
        files = [f for f in listdir(COORDS_ORIEN_DIR)
                 if isfile(join(COORDS_ORIEN_DIR, f))
                 if not f.startswith(".")]

        # Sort by modified_time
        # No modification of file, creation_time == modified_time
        files.sort(key=lambda x: getmtime(COORDS_ORIEN_DIR + x))

        print(files)

        if files:
            # Capture an image to memory
            outputs = [io.BytesIO()]
            camera.capture_sequence(outputs, "jpeg", use_video_port=True)

            # Save image to file
            for image in outputs:
                image.seek(0)
                Image.open(image).save(
                    "%s.jpeg" % IMAGES_TO_SCAN_DIR + files[0])

                # Remove COORD_ORIEN file
                remove(files[0])

        if files[0] == DONE_FILE:
            # No more images to be taken, terminate program
            print("All pictures taken, exiting program")
            sys.exit(0)
        else:
            # Sleep for 30 ms
            time.sleep(0.03)