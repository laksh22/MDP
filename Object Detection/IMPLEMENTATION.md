# Description of implementation

1. Whenever RPi receives a take picture command from TCP (@r_x_y_orientation!).

2. We will save the coordinates as a file in a folder (**coords_orien**) with filename (r_x_y_orientation). The file will have no contents. 

3. There will be code "polling" the **coords_orien** folder. If it sees a file (or an increase in number of files), it will take the file name, take a picture and save the image as **r_x_y_orientation.jpg** and save it to a folder, **images_to_scan**. The "polling" is done by `pyinotify` which is more efficient than actual polling, where the kernel tells us when it does the operation, without having to constantly request for it.

4. We have another Python script on the same PC running the algorithm for exploration and fastest path to run YOLO, which will constantly scan the folder, **images_to_scan** where all images awaiting object detection are saved via FTP, once it has finish scanning an image, it will delete the image that was scanned from **images_to_scan**, reducing the number of images in the **images_to_scan** folder.

5. If an image is detected, it will save it back to RPi via FTP as a **id,x,y.jpg** image in an output folder named, **images_found** and also, on the PC to be displayed later in a single window, satisfying the laboratory requirements.

6. After it has receive EXPLORE_DONE command from TCP, it will create a DONE file in the input folder **coords_orien**.

7. Once PC sees the DONE file via FTP, and it has finished scanning all the images in **images_to_scan** it will display all the images in 1 window.

8. RPi will then read all the image file names in the **images_found** folder and send it to bluetooth as separate packets for each image in *images_found** with the filename of the images as packet contents.

# Object Position
Depending on the position of the bounding box, the **coord_orien** in the form of `x_y_orientation` needs to be resolved.

To illustrate this, assume that the camera is facing right. An object is detected on the image `2_10_NORTH`. This means that the robot is facing **NORTH**. Hence, this means that the camera is facing **EAST**.
 
The image is delimited to three zones, hence, it will have 2 delimiters (d1, d2). In such a state, there will be 3 case:
1. Center of bounding box of detected object < d1
2. Center of bounding box of detected object >= d1 and < d2
3. Center of bounding box of detected object >= d2

The resulting coordinates of the image should be resolved as such:
| no. | state                    | coord. of object | robot orient. |
|-----|--------------------------|------------------|---------------|
| 1   | bb_center < d1           | id_3_11          | NORTH         |
| 2   | bb_center >= d1 and < d2 | id_3_10          | NORTH         |
| 3   | bb_center >= d2          | id_3_9           | NORTH         |