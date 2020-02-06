# RPi communication Hub
TODO

## Compiling
You must be in the `RPi/src` directory before running the commands below.
```shell script
cd RPi/src/
```

To compile, run the following commands:
```shell script
mkdir build
cd build
cmake ..
make
``` 

## Running
TODO

## Message structure
TODO

## Dependencies

### Bluetooth
Make sure you have the required bluetooth header files by running:
```shell script
sudo apt-get install bluez libbluetooth-dev
```
A well known SPP UUID `00001101-0000-1000-8000-00805F9B34FB` should be used when connecting to to a Bluetooth serial board (Big-endian).


This 128-bit number is used to identify this Bluetooth service. The words are ordered from most to least significant (Big-endian).
 
```c
uint32_t svc_uuid_int[] = { 0x00001101, 0x00001000, 0x80000080, 0x5F9B34FB };
```

Read more about why this string was use [here](https://developer.android.com/reference/android/bluetooth/BluetoothDevice.html#createRfcommSocketToServiceRecord%28java.util.UUID%29).  

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

To resolve this issue, you probably forgot to run the [install Bluetooth dependency command](#bluetooth). Make sure to run the package install command.
