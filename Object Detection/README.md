# Description of implementation

1. Whenever RPi receives a take picture command from TCP `@r_x_y_orientation!`. 
The coordinates and orientation will be saved as a file in a folder (**coords_orien**) with filename (`x_y_orientation`). 
The file will have no contents.

2. There will be Python script "polling" the **coords_orien** folder. 
If it sees a file (or an increase in number of files), it will take the file name, take a picture and save the image as **x_y_orientation.jpg** (same as the file name) and save it to a folder, **images_to_scan**. 
The polling is done at every 35ms.

3. We have another Python script on the same PC running the algorithm for exploration and fastest path to run YOLOv3 - object detection, which will constantly scan the folder, **images_to_scan** where all images awaiting object detection are saved via FTP.
Once it has finish scanning an image, it will delete the image that was scanned from **images_to_scan**, reducing the number of images in the **images_to_scan** folder.

4. If an object is detected, it will save it back to RPi via FTP as an image with the name **id,x,y.jpg** in an output folder named, **images_found** and also, on the PC to be displayed later in a single window, as part of the laboratory requirements.

5. After it has receive *EXPLORE_DONE* command from TCP, it will create a **DONE** file in the input folder **coords_orien**.

6. Once PC sees the **DONE** file via FTP, and it has finished scanning all the images in **images_to_scan** it will display all the images in ONE single window.

7. RPi will then read all the image filenames in the **images_found** folder and send it to Bluetooth as separate packets for each image in **images_found** with the filename of the images as packet contents.

# Object Position
Depending on the position of the bounding box, the **coord_orien** in the form of `x_y_orientation` needs to be resolved.
`x_y_orientation` is the center of the obstacle detected by the robot.

To illustrate this, assume that the camera is facing right. An object is detected on the image `2_10_NORTH`. This means that the robot is facing **NORTH**. Hence, this means that the camera is facing **EAST**.
 
The image is delimited to three zones, hence, it will have 2 delimiters (d1, d2). In such a state, there will be 3 case:
1. Center of bounding box of detected object < d1
2. Center of bounding box of detected object >= d1 and < d2
3. Center of bounding box of detected object >= d2

The resulting coordinates of the image should be resolved as such:
| no. | state                    | coord. of object | robot orient. |
|-----|--------------------------|------------------|---------------|
| 1   | bb_center < d1           | id_2_11          | NORTH         |
| 2   | bb_center >= d1 and < d2 | id_2_10          | NORTH         |
| 3   | bb_center >= d2          | id_2_9           | NORTH         |

As such, if the camera is facing rightwards, the generalisation below holds:

```python
x = coordinate_x
y = coordinate_y

if region == 1:
    # Center region; x,y coordinates will never change
    pass
elif region == 0:
    # Top region
    if ORIENTATION == "NORTH":
        y += 1
    elif ORIENTATION == "SOUTH":
        y -= 1
    elif ORIENTATION == "EAST":
        x += 1
    elif ORIENTATION == "WEST":
        x -= 1
    else:
        print("Invalid orientation")
elif region == 2:
    # Bottom region
    if ORIENTATION == "NORTH":
        y -= 1
    elif ORIENTATION == "SOUTH":
        y += 1
    elif ORIENTATION == "EAST":
        x -= 1
    elif ORIENTATION == "WEST":
        x += 1
    else:
        print("Invalid orientation")

new_filename = "%s,%s,%s" % (detected_label, coord_x, coord_y)
```