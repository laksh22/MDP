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
include tcp/CMakeFiles/Tcp.dir/depend.make

# Include the progress variables for this target.
include tcp/CMakeFiles/Tcp.dir/progress.make

# Include the compile flags for this target's objects.
include tcp/CMakeFiles/Tcp.dir/flags.make

tcp/CMakeFiles/Tcp.dir/tcp.c.o: tcp/CMakeFiles/Tcp.dir/flags.make
tcp/CMakeFiles/Tcp.dir/tcp.c.o: ../tcp/tcp.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tcp/CMakeFiles/Tcp.dir/tcp.c.o"
	cd /home/pi/Desktop/MDP/RPi/src/build/tcp && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Tcp.dir/tcp.c.o   -c /home/pi/Desktop/MDP/RPi/src/tcp/tcp.c

tcp/CMakeFiles/Tcp.dir/tcp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Tcp.dir/tcp.c.i"
	cd /home/pi/Desktop/MDP/RPi/src/build/tcp && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/Desktop/MDP/RPi/src/tcp/tcp.c > CMakeFiles/Tcp.dir/tcp.c.i

tcp/CMakeFiles/Tcp.dir/tcp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Tcp.dir/tcp.c.s"
	cd /home/pi/Desktop/MDP/RPi/src/build/tcp && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/Desktop/MDP/RPi/src/tcp/tcp.c -o CMakeFiles/Tcp.dir/tcp.c.s

tcp/CMakeFiles/Tcp.dir/tcp.c.o.requires:

.PHONY : tcp/CMakeFiles/Tcp.dir/tcp.c.o.requires

tcp/CMakeFiles/Tcp.dir/tcp.c.o.provides: tcp/CMakeFiles/Tcp.dir/tcp.c.o.requires
	$(MAKE) -f tcp/CMakeFiles/Tcp.dir/build.make tcp/CMakeFiles/Tcp.dir/tcp.c.o.provides.build
.PHONY : tcp/CMakeFiles/Tcp.dir/tcp.c.o.provides

tcp/CMakeFiles/Tcp.dir/tcp.c.o.provides.build: tcp/CMakeFiles/Tcp.dir/tcp.c.o


# Object files for target Tcp
Tcp_OBJECTS = \
"CMakeFiles/Tcp.dir/tcp.c.o"

# External object files for target Tcp
Tcp_EXTERNAL_OBJECTS =

tcp/libTcp.a: tcp/CMakeFiles/Tcp.dir/tcp.c.o
tcp/libTcp.a: tcp/CMakeFiles/Tcp.dir/build.make
tcp/libTcp.a: tcp/CMakeFiles/Tcp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libTcp.a"
	cd /home/pi/Desktop/MDP/RPi/src/build/tcp && $(CMAKE_COMMAND) -P CMakeFiles/Tcp.dir/cmake_clean_target.cmake
	cd /home/pi/Desktop/MDP/RPi/src/build/tcp && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Tcp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tcp/CMakeFiles/Tcp.dir/build: tcp/libTcp.a

.PHONY : tcp/CMakeFiles/Tcp.dir/build

tcp/CMakeFiles/Tcp.dir/requires: tcp/CMakeFiles/Tcp.dir/tcp.c.o.requires

.PHONY : tcp/CMakeFiles/Tcp.dir/requires

tcp/CMakeFiles/Tcp.dir/clean:
	cd /home/pi/Desktop/MDP/RPi/src/build/tcp && $(CMAKE_COMMAND) -P CMakeFiles/Tcp.dir/cmake_clean.cmake
.PHONY : tcp/CMakeFiles/Tcp.dir/clean

tcp/CMakeFiles/Tcp.dir/depend:
	cd /home/pi/Desktop/MDP/RPi/src/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/Desktop/MDP/RPi/src /home/pi/Desktop/MDP/RPi/src/tcp /home/pi/Desktop/MDP/RPi/src/build /home/pi/Desktop/MDP/RPi/src/build/tcp /home/pi/Desktop/MDP/RPi/src/build/tcp/CMakeFiles/Tcp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tcp/CMakeFiles/Tcp.dir/depend
