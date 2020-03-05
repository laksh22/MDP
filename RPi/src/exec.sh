#!/bin/bash

DIR="~/MDP/"
USERNAME="https://2020mdpgrp01"
PASSWORD="%_b2EDb3q\`s#\"D{d"

if [ -d "$DIR" ]; then
  echo "MDP folder exits ${DIR}..."
else
  echo "Error: ${DIR} not found. Cloning MDP and building from source..."
  git clone ${USERMA,E}:${{PASSWPRD}}@github.com/laksh22/MDP.git
  cd MDP/RPi/src
  mkdir build && cd build
  cmake ..
  make
fi

# Start RPi camera streamer and send it to background
cd ../../../Object\ Detection/RPi/
python img_evnt_handler.py &

# Start communication hub binary
./RPi
