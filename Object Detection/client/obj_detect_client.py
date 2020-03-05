import ftplib
import os
import time
from os import listdir
from os.path import isfile, join

import cv2
import numpy as np

from YoloOrchestrator import Yolo

# For actual run
HOST = "192.168.101.1"
USER = "pi"
PASS = "raspberry"

# For testing
# HOST = "speedtest.tele2.net"
# USER = "anonymous"
# PASS = "anonymous"

# Remote directories (Absolute path)
REMOTE_COORDS_ORIEN_DIR = "/Desktop/coords_orien/"
REMOTE_IMAGES_TO_SCAN_DIR = "/Desktop/images_to_scan/"
REMOTE_IMAGES_FOUND_DIR = "/Desktop/images_found/"
REMOTE_DONE_FILE = REMOTE_COORDS_ORIEN_DIR + "DONE"

LOCAL_IMAGES_TO_SCAN_DIR = "images/images_to_scan/"
LOCAL_IMAGES_FOUND_DIR = "images/found/"

if __name__ == "__main__":
    # Create required folder if it does not exist
    if not os.path.exists(LOCAL_IMAGES_TO_SCAN_DIR):
        os.makedirs(LOCAL_IMAGES_TO_SCAN_DIR)

    # Initialise yolo detector
    yolo = Yolo(yolo_asset_dir="../model", confidence=.5, threshold=.3)

    # Establish FTP connection
    with ftplib.FTP(host=HOST) as ftp:
        print("Establishing FTP connection to: %s" % HOST)
        ftp.login(user=USER, passwd=PASS)
        print("Connection established")

        # Navigate to IMAGES_TO_SCAN_DIR
        ftp.cwd(REMOTE_IMAGES_TO_SCAN_DIR)

        while 1:
            # Get all images to scan sorted by modified time
            images_to_scan = sorted(ftp.nlst(), key=lambda x: ftp.voidcmd(
                "MDTM {}".format(x)))

            for img in images_to_scan:
                if ".jpeg" in img:
                    file_size = ftp.size(img)

                    # Download remote image
                    handle = open(
                        LOCAL_IMAGES_TO_SCAN_DIR.rstrip("/") + "/" + img.lstrip(
                            "/"), 'wb')
                    ftp.retrbinary('RETR %s' % img, handle.write)
                    handle.close()

                    # Delete remote image after it has been downloaded
                    ftp.delete(img)

                    # Run YOLO object detection on image
                    detected_img_name = yolo.process_image(
                        filename=img, parent_dir=LOCAL_IMAGES_TO_SCAN_DIR)

                    # Delete raw image from local images_to_scan
                    os.remove(LOCAL_IMAGES_TO_SCAN_DIR + img)

                    # Write file back to RPi if object is found
                    if detected_img_name:
                        # Navigate to IMAGES_FOUND_DIR
                        ftp.cwd(REMOTE_IMAGES_FOUND_DIR)

                        # Open file to send
                        file_to_send = open(
                            LOCAL_IMAGES_FOUND_DIR + detected_img_name, "rb")

                        # Send the file
                        ftp.storbinary("STOR {}".format(detected_img_name),
                                       file_to_send)

                        # Navigate back to IMAGES_TO_SCAN_DIR
                        ftp.cwd(REMOTE_IMAGES_TO_SCAN_DIR)
                    else:
                        print("No object detected")

            # Check the IMAGES_TO_SCAN_DIR directory again
            images_to_scan = sorted(ftp.nlst(), key=lambda x: ftp.voidcmd(
                "MDTM {}".format(x)))

            # Check if all images has been scanned
            if len(images_to_scan) == 1 and images_to_scan[0] == "DONE":
                print("No more images to scan")
                break

            # Sleep for 1s
            time.sleep(1)

    # All images scanned for objects; Display all of them in a single window
    # Get all images to display
    image_to_disp_dirs = [LOCAL_IMAGES_FOUND_DIR + f for f in
                          listdir(LOCAL_IMAGES_FOUND_DIR)
                          if isfile(join(LOCAL_IMAGES_FOUND_DIR, f))
                          if not f.startswith(".")]

    cv2_imgs_to_disp = [cv2.imread(images_to_scan) for image_to_disp_dir in
                        image_to_disp_dirs]

    h_stacks = []
    # Start from index 1 instead of 0
    # Create horizontal stacks of 2 images in each row first
    for idx in range(1, len(cv2_imgs_to_disp)):
        if idx % 2 == 1:
            h_stacks.append(
                np.hstack((cv2_imgs_to_disp[idx - 1], cv2_imgs_to_disp[idx])))
        elif idx % 2 == 0 and idx == len(cv2_imgs_to_disp):
            # Odd number of images to stitch together
            # Handle last image
            h_stacks.append(cv2_imgs_to_disp)

    # Vertically stack the images together
    v_stack = np.vstack(tuple(h_stacks))

    all_detected_obj_name = "%s_all_detected" % int(time.time())

    # Write the image out
    print("Saving all images in a single window...")
    cv2.imwrite(all_detected_obj_name + ".jpeg")

    # Display the image
    cv2.imshow(all_detected_obj_name, v_stack)
    cv2.waitKey(0)

    print("Terminating YOLO object detection client...")
