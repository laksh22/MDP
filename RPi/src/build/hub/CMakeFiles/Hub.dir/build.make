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
include hub/CMakeFiles/Hub.dir/depend.make

# Include the progress variables for this target.
include hub/CMakeFiles/Hub.dir/progress.make

# Include the compile flags for this target's objects.
include hub/CMakeFiles/Hub.dir/flags.make

hub/CMakeFiles/Hub.dir/hub.c.o: hub/CMakeFiles/Hub.dir/flags.make
hub/CMakeFiles/Hub.dir/hub.c.o: ../hub/hub.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object hub/CMakeFiles/Hub.dir/hub.c.o"
	cd /home/pi/Desktop/MDP/RPi/src/build/hub && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Hub.dir/hub.c.o   -c /home/pi/Desktop/MDP/RPi/src/hub/hub.c

hub/CMakeFiles/Hub.dir/hub.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Hub.dir/hub.c.i"
	cd /home/pi/Desktop/MDP/RPi/src/build/hub && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/Desktop/MDP/RPi/src/hub/hub.c > CMakeFiles/Hub.dir/hub.c.i

hub/CMakeFiles/Hub.dir/hub.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Hub.dir/hub.c.s"
	cd /home/pi/Desktop/MDP/RPi/src/build/hub && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/Desktop/MDP/RPi/src/hub/hub.c -o CMakeFiles/Hub.dir/hub.c.s

hub/CMakeFiles/Hub.dir/hub.c.o.requires:

.PHONY : hub/CMakeFiles/Hub.dir/hub.c.o.requires

hub/CMakeFiles/Hub.dir/hub.c.o.provides: hub/CMakeFiles/Hub.dir/hub.c.o.requires
	$(MAKE) -f hub/CMakeFiles/Hub.dir/build.make hub/CMakeFiles/Hub.dir/hub.c.o.provides.build
.PHONY : hub/CMakeFiles/Hub.dir/hub.c.o.provides

hub/CMakeFiles/Hub.dir/hub.c.o.provides.build: hub/CMakeFiles/Hub.dir/hub.c.o


# Object files for target Hub
Hub_OBJECTS = \
"CMakeFiles/Hub.dir/hub.c.o"

# External object files for target Hub
Hub_EXTERNAL_OBJECTS =

hub/libHub.a: hub/CMakeFiles/Hub.dir/hub.c.o
hub/libHub.a: hub/CMakeFiles/Hub.dir/build.make
hub/libHub.a: hub/CMakeFiles/Hub.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/Desktop/MDP/RPi/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libHub.a"
	cd /home/pi/Desktop/MDP/RPi/src/build/hub && $(CMAKE_COMMAND) -P CMakeFiles/Hub.dir/cmake_clean_target.cmake
	cd /home/pi/Desktop/MDP/RPi/src/build/hub && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Hub.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
hub/CMakeFiles/Hub.dir/build: hub/libHub.a

.PHONY : hub/CMakeFiles/Hub.dir/build

hub/CMakeFiles/Hub.dir/requires: hub/CMakeFiles/Hub.dir/hub.c.o.requires

.PHONY : hub/CMakeFiles/Hub.dir/requires

hub/CMakeFiles/Hub.dir/clean:
	cd /home/pi/Desktop/MDP/RPi/src/build/hub && $(CMAKE_COMMAND) -P CMakeFiles/Hub.dir/cmake_clean.cmake
.PHONY : hub/CMakeFiles/Hub.dir/clean

hub/CMakeFiles/Hub.dir/depend:
	cd /home/pi/Desktop/MDP/RPi/src/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/Desktop/MDP/RPi/src /home/pi/Desktop/MDP/RPi/src/hub /home/pi/Desktop/MDP/RPi/src/build /home/pi/Desktop/MDP/RPi/src/build/hub /home/pi/Desktop/MDP/RPi/src/build/hub/CMakeFiles/Hub.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : hub/CMakeFiles/Hub.dir/depend

