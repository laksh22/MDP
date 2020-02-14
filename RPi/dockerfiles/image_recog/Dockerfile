FROM debian:stretch

# Based in part on on schickling/opencv by Johannes Schickling <schickling.j@gmail.com>
ARG OPENCV_VERSION="4.0.1"

# install dependencies
RUN apt-get update -y
RUN apt-get install -y libopencv-dev yasm libjpeg-dev libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev libv4l-dev libtbb-dev libqt4-dev libgtk2.0-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils pkg-config curl build-essential checkinstall cmake nano python3 python3-numpy python3-pip git

# download opencv
RUN curl -sL https://github.com/opencv/opencv/archive/$OPENCV_VERSION.tar.gz | tar xvz -C /tmp

RUN mkdir -p /tmp/opencv-$OPENCV_VERSION/build

WORKDIR /tmp/opencv-$OPENCV_VERSION/build

# install
RUN cmake -DWITH_FFMPEG=OFF -DWITH_OPENEXR=OFF -DBUILD_TIFF=OFF -DWITH_CUDA=OFF -DWITH_NVCUVID=OFF -DBUILD_PNG=OFF ..
RUN make
RUN make install

# configure
RUN echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf
RUN ldconfig
RUN ln /dev/null /dev/raw1394 # hide warning - http://stackoverflow.com/questions/12689304/ctypes-error-libdc1394-error-failed-to-initialize-libdc1394

WORKDIR /

RUN git clone https://github.com/pjreddie/darknet
RUN cd darknet && perl -pi -e 's/OPENCV=0/OPENCV=1/gs' Makefile && make -j3

# Install opencv-python dependency
RUN pip3 install opencv-python

WORKDIR /darknet