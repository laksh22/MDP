# RPi communication Hub
This section contains everything you will need to know to get the `RPi` communication hub binary that is written in C to run.

## 1. Compiling
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

## 2. Running
After compiling, make sure that you are in the `~/work_dir/RPi/src/build` directory before running the command below.
```shell script
./RPi
```

A one-liner command is shown below:
```shell script
cd ~/work_dir/RPi/src/build && ./RPi
```

## 3. Message structure
Each message is prefixed by `@` and suffixed with `!`. New lines at the end of each messages are not allowed.

### 3.1. Examples
To send a message to **Bluetooth**:
```text
@binsert_bluetooth_message_here!
```

To send a message to **TCP**:
```text
@tinsert_tcp_message_here!
```

To send a message to **Serial**:
```text
@sinsert_serial_message_here!
```

## 4. Dependencies

### 4.1. Bluetooth
Make sure you have the required bluetooth header files by running:
```shell script
sudo apt-get install bluez libbluetooth-dev
```
A well known SPP UUID `00001101-0000-1000-8000-00805F9B34FB` should be used when connecting to to a Bluetooth serial board.


This 128-bit number is used to identify this Bluetooth service. The words are ordered from most to least significant (Big-endian).

The value was chosen from the result returned from `bluetoothctl`.
```shell script
bluetoothctl
info 68:B3:5E:56:A8:63
```

Note `68:B3:5E:56:A8:63` Bluetooth address of our group's N7 tablet. The result returned is as follows:
```text
Device 68:B3:5E:56:A8:63
	Name: B1-870
	Alias: B1-870
	Class: 0x5a010c
	Icon: computer
	Paired: yes
	Trusted: yes
	Blocked: no
	Connected: no
	LegacyPairing: no
	UUID: Serial Port               (00001101-0000-1000-8000-00805f9b34fb)
	UUID: OBEX Object Push          (00001105-0000-1000-8000-00805f9b34fb)
	UUID: Audio Source              (0000110a-0000-1000-8000-00805f9b34fb)
	UUID: A/V Remote Control Target (0000110c-0000-1000-8000-00805f9b34fb)
	UUID: A/V Remote Control        (0000110e-0000-1000-8000-00805f9b34fb)
	UUID: PnP Information           (00001200-0000-1000-8000-00805f9b34fb)
	UUID: Generic Access Profile    (00001800-0000-1000-8000-00805f9b34fb)
	UUID: Generic Attribute Profile (00001801-0000-1000-8000-00805f9b34fb)
	Modalias: bluetooth:v0046p1200d1436
```
 
Keeping in mind of the Big-endianness, `svc_uuid_int` will take the value shown below.
```c
uint32_t svc_uuid_int[] = { 0x00001101, 0x00001000, 0x80000080, 0x5F9B34FB };
```

Read more about this UUID [here](https://developer.android.com/reference/android/bluetooth/BluetoothDevice.html#createRfcommSocketToServiceRecord%28java.util.UUID%29).  

### 4.2. Serial
**This is only required when developing on a non Raspbian platform with no wiringPi header files.**

Make sure you have the required wiringPi and wiringSerial header files by running (If you are working on a linux based machine):
```shell script
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build
``` 

This should give you the required header files so that the IDE you are working on will not complain about undefined reference(s) to wiringPi/wiringSerial dependencies.
However, your code will still be uncompilable on a non Raspbian platform.

## 5. Common errors

### 5.1. Missing Bluetooth dependencies
```text
CMake Error: The following variables are used in this project, but they are set to NOTFOUND.
Please set them or make sure they are set and tested correctly in the CMake files: Bluez_LIB
    linked by target "RPi" in directory /home/pi/MDP/RPi

-- Configuring incomplete, errors occurred!
```

### 5.2. Bluetooth not connecting / waiting for connection

```text
===== Initializing connections =====
[serial_connect]: Serial port connection /dev/ttyAMA0 with 9600 established successfully.
[register_service]: Registering UUID 00001101-0000-1000-8000-00805f9b34fb
[bt_connect]: Creation of BT socket successful...
[bt_connect]: Binding of BT socket successful..
[bt_connect]: Bluetooth Server is now listening for connections..
```

This issue is caused by the usage of `/dev/ttyAMA0`, the serial port `/dev/ttyAMA0` should not be used. 

Instead, please use `dev/ttyACM0`. During testing, you can use a USB cable connected to your an Android phone to connect to the upper port nearest to the LAN port.

This should be enough to get you through your tests. For further instructions on how to read the messages sent via the serial port `dev/ttyACM0` please refer to the [Serial](#serial) section.

To resolve this issue, you probably forgot to run the [install Bluetooth dependency command](#bluetooth). Make sure to run the package install command.


### 5.3. Messages not getting sent over Bluetooth

Connected to Bluetooth, sent messages, but it is not displaying on the console of the `RPi` binary. 

Please take note that the `RPi` binary does not play well with *new-line* endings at the end of the message. 

For example:
```text
@tmessage_to_be_sent_to_tcp!\n
```
Some Android applications that can facilitate testing will add a *new-line* or *carriage-return* to the end of each message by default.

If you are using the application `Serial Bluetooth Terminal` by Kai Morich (*de.kai_morich.serial_bluetooth_terminal*) to do your testing, you can disable this as follows:
```text
1) Head into settings
2) Click on the "Send" tab
3) Change the Newline settings to None
```

Your messages sent via Bluetooth should be displayed on the console running the `RPi` binary now.

### 5.4. Bluetooth / TCP address is already in use

This issue usually arises after an initial run, and the user terminates the RPi binary. 
While the RPi binary is terminated, not all services invoked by the RPi binary is terminated.  
To reduce the number of commands that the user is required to run during the 2 minutes preparation time, some python scripts are invoked automatically by the RPi binary and sent to the background.

This line is responsible for the handling of the invocation.
```c
system("python ../../../Object\\ Detection/RPi/img_evnt_handler.py &");
```

If this python script is still running in the background, the resources that were initially used might not be released even if the RPi binary is killed.

As such, before you run the RPi binary again after the initial run, run this command.
```shell script
killall python
./RPi
``` 

Your RPi binary should start normally after.