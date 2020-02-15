# Object Detection and Recognition
This path contains the the required sources to perform object detection and recognition using YOLOv3. 

# 1. Client dependencies
The client dependencies are managed with the Dockerfile to ensure reproducibility, and environment consistency.

The required dependencies have already been configured in the docker file provided in `~/dockerfiles/Dockerfile`.

The main dependencies that are configured are mainly:
- python 3.5.3
- OpenCV 4.0.1
- YOLOv3/Darknet (master branch)

## 1.1 Building the Dockerfile
Note: When building the `YOLOv3/Darknet` from source, the make command specifies that **3** cores should be used. 
```dockerfile
RUN git clone https://github.com/pjreddie/darknet
RUN cd darknet && perl -pi -e 's/OPENCV=0/OPENCV=1/gs' Makefile && make -j3
```
Do ensure that your host machine that is running Docker has the required resources to perform the above specifications. Modify the `-j` flag should you not have the required resources accordingly.

Make sure that you are in the `Object\ Detection/` path before proceeding.

Run the following command to build the docker image. Replace the `[image_name]` field with the image name of your choice. 
```shell script
docker build -t [image_name] -f Dockerfile dockerfiles/
```

For **MDP_GRP_01**, please ensure that you are in the `Object\ Detection/` directory before proceeding.
```shell script
cd dockerfile
docker build -t mdp_obj_detection -f Dockerfile .
```

## 1.2 Running the Docker image as a container
TODO

# 2. Server dependencies

# 2.1 picamera
Ensure that you have `picamera` installed on your Raspberry Pi.
```shell script
sudo apt-get update
sudo apt-get install python-picamera python3-picamera 
```
