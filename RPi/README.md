# RPi communication Hub
The general implementation of the infrastructure is written and implemented in C as speed and efficiency was kept in mind when developing. 
As such, the decision to write the communication hub in C was made as we wanted to "get as close to the metal as possible" (to be as low level as possible). The overall message sending infrastructure consists of nine (9) threads and three (4) queues - 

- 9 Threads
  - Two threads each for the communications protocol (_Bluetooth_, _Serial_, _TCP_)
  - 1 keep alive queue 
  - 2 image taking and handling threads

1. tcp_reader_create
2. tcp_sender_create
3. bt_reader_create
4. bt_sender_create
5. serial_reader_create
6. serial_sender_create
7. serial_inactiv_prvt_thread
8. read_img_labels
9. take_picture

```c
pthread_t *thread_group = malloc(sizeof(pthread_t) * NUM_THREADS);
pthread_create(&thread_group[0], NULL, tcp_reader_create, NULL);
pthread_create(&thread_group[1], NULL, tcp_sender_create, NULL);
pthread_create(&thread_group[2], NULL, bt_reader_create, NULL);
pthread_create(&thread_group[3], NULL, bt_sender_create, NULL);
pthread_create(&thread_group[4], NULL, serial_reader_create, NULL);
pthread_create(&thread_group[5], NULL, serial_sender_create, NULL);
pthread_create(&thread_group[6], NULL, serial_inactiv_prvt_thread, NULL);
pthread_create(&thread_group[7], NULL, read_img_labels, NULL);
pthread_create(&thread_group[8], NULL, take_picture, NULL);
```

- 4 Queues
    - One queue per communication interface
    
```c
// Create the respective rpa_queue for each communication port
rpa_queue_create(&s_queue, (uint32_t) QSIZE);
rpa_queue_create(&b_queue, (uint32_t) QSIZE);
rpa_queue_create(&t_queue, (uint32_t) QSIZE);
rpa_queue_create(&r_queue, (uint32_t) QSIZE);
```

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

To resolve this issue, you probably forgot to run the [install Bluetooth dependency command](#41-bluetooth). Make sure to run the package install command.

### 5.2. Bluetooth not connecting / waiting for connection

```text
===== Initializing connections =====
[serial_connect]: Serial port connection /dev/ttyAMA0 with 9600 established successfully.
[register_service]: Registering UUID 00001101-0000-1000-8000-00805f9b34fb
[bt_connect]: Creation of BT socket successful...
[bt_connect]: Binding of BT socket successful..
[bt_connect]: Bluetooth Server is now listening for connections..
```

This issue is caused by the usage of `/dev/ttyAMA0`, the serial port `/dev/ttyAMA0` should not be used. Instead, please use `dev/ttyACM0`.

This should be enough to get you through your tests. For further instructions on how to read the messages sent via the serial port `dev/ttyACM0` please refer to the [Serial](#42-serial) section.


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
