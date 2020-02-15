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
To run a container of the image that was built, run the command below.
```shell script
docker run --name=mdp_grp01_obj_detect -v ~/work_dir:/darknet/work_dir -p 5000:8003 mdp_obj_detection
```

**What is happening**:
- `--name=mdp_grp01_obj_detect` names the container so we can refer to it more easily.
- `-v ~/work_dir:/darknet/work_dir` sets up a bindmount volume that links the `/darknet/work_dir` directory from inside the **mdp_obj_detection** container to the `~/work_dir` directory on the host machine. Docker uses a : to split the host’s path from the container path, and the host path always comes first.
- `-p 5000:8003` sets up a port forward. The **mdp_obj_detection** container is listening on port 8003 (should there be any process that is binding to it). This flag maps the container’s port 8003 to port 5000 on the host system. Docker uses a : to split the host’s port from the container port, and the host port always comes first.
- `mdp_obj_detection` specifies that the container should be built from the `mdp_obj_detection` image.

**Other flags:**
- TODO: add if there are any.

# 2. Server dependencies

# 2.1 picamera
Ensure that you have `picamera` installed on your Raspberry Pi.
```shell script
sudo apt-get update
sudo apt-get install python-picamera python3-picamera 
```
