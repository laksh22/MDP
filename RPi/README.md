# RPi communication Hub
TODO

## Compiling

## Dependencies

### Bluetooth
Make sure you have the required bluetooth header files by running:
```shell script
sudo apt-get install bluez libbluetooth-dev
```

### Serial
**This is only required when developing on a non Raspbian platform with no wiringPi header files.**

Make sure you have the required wiringPi and wiringSerial header files by running (If you are working on a linux based machine):
```shell script
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build
``` 

This should give you the required header files so that the IDE you are working on will not complain about undefined reference(s) to wiringPi/wiringSerial dependencies.
However, your code will still be uncompilable on a non Raspbian platform.


## Common errors

### Missing Bluetooth dependencies
```text
CMake Error: The following variables are used in this project, but they are set to NOTFOUND.
Please set them or make sure they are set and tested correctly in the CMake files: Bluez_LIB
    linked by target "RPi" in directory /home/pi/MDP/RPi

-- Configuring incomplete, errors occurred!
```

To resolve this issue, you probably forgot to run the [install Bluetooth dependency command](#Bluetooth). Make sure to run the package install command.
