import ftplib
import os
import time
from os import listdir
from os.path import isfile, join

import shutil
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

LOCAL_IMAGES_TO_SCAN_DIR = "images/images_to_scan/"
LOCAL_IMAGES_FOUND_DIR = "images/found/"

if __name__ == "__main__":
    # Create required folder if it does not exist
    if not os.path.exists(LOCAL_IMAGES_TO_SCAN_DIR):
        os.makedirs(LOCAL_IMAGES_TO_SCAN_DIR)

    # Initialise yolo detector
    yolo = Yolo(yolo_asset_dir="../model", confidence=.6, threshold=.3)

    # Establish FTP connection
    with ftplib.FTP(host=HOST) as ftp:
        print("Establishing FTP connection to: %s" % HOST)
        ftp.login(user=USER, passwd=PASS)
        print("Connection established")

        # Navigate to IMAGES_TO_SCAN_DIR
        ftp.cwd(REMOTE_IMAGES_TO_SCAN_DIR)

        while 1:
            # Get all images to scan sorted by modified time
            # images_to_scan = sorted(ftp.nlst(), key=lambda x: ftp.voidcmd(
            #     "MDTM {}".format(x)))

            images_to_scan = ftp.nlst()

            for img in images_to_scan:
                # Only process images that ends with "_ACK.jpeg"
                if "_ACK.jpeg" in img:
                    local_download_file_path = LOCAL_IMAGES_TO_SCAN_DIR.rstrip(
                        "/") + "/" + img.lstrip("/")
                    local_download_renamed_file_path = \
                        LOCAL_IMAGES_TO_SCAN_DIR.rstrip("/") + "/" \
                        + img.replace("_ACK.jpeg", ".jpeg").lstrip("/")

                    # Download remote image
                    handle = open(local_download_file_path, 'wb')
                    ftp.retrbinary('RETR %s' % img, handle.write)
                    handle.close()

                    # Rename downloaded image
                    os.rename(local_download_file_path,
                              local_download_renamed_file_path)

                    # Delete remote image after it has been downloaded
                    ftp.delete(img)

                    # Run YOLO object detection on image
                    try:
                        detected_img_name = yolo.process_image(
                            filename=img.replace("_ACK.jpeg", ".jpeg"), parent_dir=LOCAL_IMAGES_TO_SCAN_DIR)
                        print(img)
                    except Exception as e:
                        print(e)

                    # Delete raw image from local images_to_scan
                    os.remove(local_download_renamed_file_path)

                    # Write file back to RPi if object is found
                    if detected_img_name:
                        # Copy the file with the send string as name
                        block_name = "@bBLOCK-" + \
                                     detected_img_name.split(".jpeg")[0]
                        shutil.copy(LOCAL_IMAGES_FOUND_DIR + detected_img_name,
                                    LOCAL_IMAGES_FOUND_DIR + block_name)

                        # Navigate to IMAGES_FOUND_DIR
                        ftp.cwd(REMOTE_IMAGES_FOUND_DIR)

                        # Open file to send
                        file_to_send = open(
                            LOCAL_IMAGES_FOUND_DIR + block_name, "rb")

                        # Send the file
                        ftp.storbinary("STOR {}".format(block_name),
                                       file_to_send)

                        # Remove the file
                        os.remove(LOCAL_IMAGES_FOUND_DIR + block_name)

                        # Navigate back to REMOTE_IMAGES_TO_SCAN_DIR
                        ftp.cwd(REMOTE_IMAGES_TO_SCAN_DIR)
                    else:
                        print("No object detected")

            # Check the REMOTE_IMAGES_TO_SCAN_DIR
            # images_to_scan = sorted(ftp.nlst(), key=lambda x: ftp.voidcmd(
            #     "MDTM {}".format(x)))

            images_to_scan = ftp.nlst()

            # Navigate back to REMOTE_COORDS_ORIEN_DIR
            ftp.cwd(REMOTE_COORDS_ORIEN_DIR)

            # Check the REMOTE_COORDS_ORIEN_DIR
            # coords = sorted(ftp.nlst(),
                            # key=lambda x: ftp.voidcmd("MDTM {}".format(x)))
            coords = ftp.nlst()

            # Make sure there are no more images to scan
            # Make sure that DONE file is the only file there
            if len(images_to_scan) == 0 \
                    and len(coords) == 1 and coords[0] == "DONE":
                print("No more images to scan")
                break

            # Navigate back to REMOTE_IMAGES_TO_SCAN_DIR
            ftp.cwd(REMOTE_IMAGES_TO_SCAN_DIR)

            # Sleep for 1s
            time.sleep(1)

    # All images scanned for objects; Display all of them in a single window
    # Get all images to display
    image_to_disp_dirs = [LOCAL_IMAGES_FOUND_DIR + f for f in
                          listdir(LOCAL_IMAGES_FOUND_DIR)
                          if isfile(join(LOCAL_IMAGES_FOUND_DIR, f))
                          if not f.startswith(".")]

    cv2_imgs_to_disp = [cv2.imread(image_to_disp_dir) for image_to_disp_dir in
                        image_to_disp_dirs]

    all_detected_obj_name = "%s_all_detected" % int(time.time())
    if len(cv2_imgs_to_disp) == 0:
        pass
    elif len(cv2_imgs_to_disp) == 1:
        # Only 1 image to display
        # Display the image
        cv2.imshow(all_detected_obj_name, cv2_imgs_to_disp[0])
        cv2.waitKey(0)
    else:
        h_stacks = []
        # Start from index 1 instead of 0
        # Create horizontal stacks of 2 images in each row first
        for idx in range(1, len(cv2_imgs_to_disp)):
            if idx % 2 == 1:
                h_stacks.append(
                    np.hstack(
                        (cv2_imgs_to_disp[idx - 1], cv2_imgs_to_disp[idx])))
            elif idx % 2 == 0 and idx == len(cv2_imgs_to_disp) - 1:
                # Odd number of images to stitch together
                # Handle last image
                h_stacks.append(
                    np.hstack((cv2_imgs_to_disp[idx], np.zeros((480, 640, 3)))))

        # Vertically stack the images together
        v_stack = np.vstack(tuple(h_stacks))

        # Write the image out
        print("Saving all images in a single window...")
        cv2.imwrite(all_detected_obj_name + ".jpeg", v_stack)

        wrt_img = cv2.imread(all_detected_obj_name + ".jpeg")
        dis_img = cv2.resize(wrt_img,
                             (int(wrt_img.shape[1] / 2),
                              int(wrt_img.shape[0] / 2)))
        # Display the image
        cv2.imshow(all_detected_obj_name, dis_img)
        cv2.waitKey(0)

    print("Terminating YOLO object detection client...")
