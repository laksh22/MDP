# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/Desktop/MDP/RPi/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/Desktop/MDP/RPi/src/build

# Include any dependencies generated for this target.
include CMakeFiles/RPi.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/RPi.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/RPi.dir/flags.make

CMakeFiles/RPi.dir/main.c.o: CMakeFiles/RPi.dir/flags.make
CMakeFiles/RPi.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/RPi.dir/main.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RPi.dir/main.c.o   -c /home/pi/Desktop/MDP/RPi/src/main.c

CMakeFiles/RPi.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RPi.dir/main.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/Desktop/MDP/RPi/src/main.c > CMakeFiles/RPi.dir/main.c.i

CMakeFiles/RPi.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RPi.dir/main.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/Desktop/MDP/RPi/src/main.c -o CMakeFiles/RPi.dir/main.c.s

CMakeFiles/RPi.dir/main.c.o.requires:

.PHONY : CMakeFiles/RPi.dir/main.c.o.requires

CMakeFiles/RPi.dir/main.c.o.provides: CMakeFiles/RPi.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/RPi.dir/build.make CMakeFiles/RPi.dir/main.c.o.provides.build
.PHONY : CMakeFiles/RPi.dir/main.c.o.provides

CMakeFiles/RPi.dir/main.c.o.provides.build: CMakeFiles/RPi.dir/main.c.o


CMakeFiles/RPi.dir/hub/hub.c.o: CMakeFiles/RPi.dir/flags.make
CMakeFiles/RPi.dir/hub/hub.c.o: ../hub/hub.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/RPi.dir/hub/hub.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RPi.dir/hub/hub.c.o   -c /home/pi/Desktop/MDP/RPi/src/hub/hub.c

CMakeFiles/RPi.dir/hub/hub.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RPi.dir/hub/hub.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/Desktop/MDP/RPi/src/hub/hub.c > CMakeFiles/RPi.dir/hub/hub.c.i

CMakeFiles/RPi.dir/hub/hub.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RPi.dir/hub/hub.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/Desktop/MDP/RPi/src/hub/hub.c -o CMakeFiles/RPi.dir/hub/hub.c.s

CMakeFiles/RPi.dir/hub/hub.c.o.requires:

.PHONY : CMakeFiles/RPi.dir/hub/hub.c.o.requires

CMakeFiles/RPi.dir/hub/hub.c.o.provides: CMakeFiles/RPi.dir/hub/hub.c.o.requires
	$(MAKE) -f CMakeFiles/RPi.dir/build.make CMakeFiles/RPi.dir/hub/hub.c.o.provides.build
.PHONY : CMakeFiles/RPi.dir/hub/hub.c.o.provides

CMakeFiles/RPi.dir/hub/hub.c.o.provides.build: CMakeFiles/RPi.dir/hub/hub.c.o


CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o: CMakeFiles/RPi.dir/flags.make
CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o: ../rpa_queue/rpa_queue.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o   -c /home/pi/Desktop/MDP/RPi/src/rpa_queue/rpa_queue.c

CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/Desktop/MDP/RPi/src/rpa_queue/rpa_queue.c > CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.i

CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/Desktop/MDP/RPi/src/rpa_queue/rpa_queue.c -o CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.s

CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o.requires:

.PHONY : CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o.requires

CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o.provides: CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o.requires
	$(MAKE) -f CMakeFiles/RPi.dir/build.make CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o.provides.build
.PHONY : CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o.provides

CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o.provides.build: CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o


CMakeFiles/RPi.dir/serial/serial.c.o: CMakeFiles/RPi.dir/flags.make
CMakeFiles/RPi.dir/serial/serial.c.o: ../serial/serial.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/RPi.dir/serial/serial.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RPi.dir/serial/serial.c.o   -c /home/pi/Desktop/MDP/RPi/src/serial/serial.c

CMakeFiles/RPi.dir/serial/serial.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RPi.dir/serial/serial.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/Desktop/MDP/RPi/src/serial/serial.c > CMakeFiles/RPi.dir/serial/serial.c.i

CMakeFiles/RPi.dir/serial/serial.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RPi.dir/serial/serial.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/Desktop/MDP/RPi/src/serial/serial.c -o CMakeFiles/RPi.dir/serial/serial.c.s

CMakeFiles/RPi.dir/serial/serial.c.o.requires:

.PHONY : CMakeFiles/RPi.dir/serial/serial.c.o.requires

CMakeFiles/RPi.dir/serial/serial.c.o.provides: CMakeFiles/RPi.dir/serial/serial.c.o.requires
	$(MAKE) -f CMakeFiles/RPi.dir/build.make CMakeFiles/RPi.dir/serial/serial.c.o.provides.build
.PHONY : CMakeFiles/RPi.dir/serial/serial.c.o.provides

CMakeFiles/RPi.dir/serial/serial.c.o.provides.build: CMakeFiles/RPi.dir/serial/serial.c.o


CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o: CMakeFiles/RPi.dir/flags.make
CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o: ../bluetooth/bluetooth.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o   -c /home/pi/Desktop/MDP/RPi/src/bluetooth/bluetooth.c

CMakeFiles/RPi.dir/bluetooth/bluetooth.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RPi.dir/bluetooth/bluetooth.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/Desktop/MDP/RPi/src/bluetooth/bluetooth.c > CMakeFiles/RPi.dir/bluetooth/bluetooth.c.i

CMakeFiles/RPi.dir/bluetooth/bluetooth.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RPi.dir/bluetooth/bluetooth.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/Desktop/MDP/RPi/src/bluetooth/bluetooth.c -o CMakeFiles/RPi.dir/bluetooth/bluetooth.c.s

CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o.requires:

.PHONY : CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o.requires

CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o.provides: CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o.requires
	$(MAKE) -f CMakeFiles/RPi.dir/build.make CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o.provides.build
.PHONY : CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o.provides

CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o.provides.build: CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o


CMakeFiles/RPi.dir/tcp/tcp.c.o: CMakeFiles/RPi.dir/flags.make
CMakeFiles/RPi.dir/tcp/tcp.c.o: ../tcp/tcp.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/RPi.dir/tcp/tcp.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RPi.dir/tcp/tcp.c.o   -c /home/pi/Desktop/MDP/RPi/src/tcp/tcp.c

CMakeFiles/RPi.dir/tcp/tcp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RPi.dir/tcp/tcp.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/Desktop/MDP/RPi/src/tcp/tcp.c > CMakeFiles/RPi.dir/tcp/tcp.c.i

CMakeFiles/RPi.dir/tcp/tcp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RPi.dir/tcp/tcp.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/Desktop/MDP/RPi/src/tcp/tcp.c -o CMakeFiles/RPi.dir/tcp/tcp.c.s

CMakeFiles/RPi.dir/tcp/tcp.c.o.requires:

.PHONY : CMakeFiles/RPi.dir/tcp/tcp.c.o.requires

CMakeFiles/RPi.dir/tcp/tcp.c.o.provides: CMakeFiles/RPi.dir/tcp/tcp.c.o.requires
	$(MAKE) -f CMakeFiles/RPi.dir/build.make CMakeFiles/RPi.dir/tcp/tcp.c.o.provides.build
.PHONY : CMakeFiles/RPi.dir/tcp/tcp.c.o.provides

CMakeFiles/RPi.dir/tcp/tcp.c.o.provides.build: CMakeFiles/RPi.dir/tcp/tcp.c.o


# Object files for target RPi
RPi_OBJECTS = \
"CMakeFiles/RPi.dir/main.c.o" \
"CMakeFiles/RPi.dir/hub/hub.c.o" \
"CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o" \
"CMakeFiles/RPi.dir/serial/serial.c.o" \
"CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o" \
"CMakeFiles/RPi.dir/tcp/tcp.c.o"

# External object files for target RPi
RPi_EXTERNAL_OBJECTS =

RPi: CMakeFiles/RPi.dir/main.c.o
RPi: CMakeFiles/RPi.dir/hub/hub.c.o
RPi: CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o
RPi: CMakeFiles/RPi.dir/serial/serial.c.o
RPi: CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o
RPi: CMakeFiles/RPi.dir/tcp/tcp.c.o
RPi: CMakeFiles/RPi.dir/build.make
RPi: /usr/lib/libwiringPi.so
RPi: /usr/lib/arm-linux-gnueabihf/libbluetooth.so
RPi: CMakeFiles/RPi.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking C executable RPi"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RPi.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/RPi.dir/build: RPi

.PHONY : CMakeFiles/RPi.dir/build

CMakeFiles/RPi.dir/requires: CMakeFiles/RPi.dir/main.c.o.requires
CMakeFiles/RPi.dir/requires: CMakeFiles/RPi.dir/hub/hub.c.o.requires
CMakeFiles/RPi.dir/requires: CMakeFiles/RPi.dir/rpa_queue/rpa_queue.c.o.requires
CMakeFiles/RPi.dir/requires: CMakeFiles/RPi.dir/serial/serial.c.o.requires
CMakeFiles/RPi.dir/requires: CMakeFiles/RPi.dir/bluetooth/bluetooth.c.o.requires
CMakeFiles/RPi.dir/requires: CMakeFiles/RPi.dir/tcp/tcp.c.o.requires

.PHONY : CMakeFiles/RPi.dir/requires

CMakeFiles/RPi.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/RPi.dir/cmake_clean.cmake
.PHONY : CMakeFiles/RPi.dir/clean

CMakeFiles/RPi.dir/depend:
	cd /home/pi/Desktop/MDP/RPi/src/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/Desktop/MDP/RPi/src /home/pi/Desktop/MDP/RPi/src /home/pi/Desktop/MDP/RPi/src/build /home/pi/Desktop/MDP/RPi/src/build /home/pi/Desktop/MDP/RPi/src/build/CMakeFiles/RPi.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/RPi.dir/depend
