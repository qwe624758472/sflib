# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

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
CMAKE_SOURCE_DIR = /home/skyfire/CLionProjects/sflib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/skyfire/CLionProjects/sflib/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/test_rpc_client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_rpc_client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_rpc_client.dir/flags.make

CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.o: CMakeFiles/test_rpc_client.dir/flags.make
CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.o: ../test/test_rpc/test_rpc_client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/skyfire/CLionProjects/sflib/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.o -c /home/skyfire/CLionProjects/sflib/test/test_rpc/test_rpc_client.cpp

CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/skyfire/CLionProjects/sflib/test/test_rpc/test_rpc_client.cpp > CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.i

CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/skyfire/CLionProjects/sflib/test/test_rpc/test_rpc_client.cpp -o CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.s

# Object files for target test_rpc_client
test_rpc_client_OBJECTS = \
"CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.o"

# External object files for target test_rpc_client
test_rpc_client_EXTERNAL_OBJECTS =

test_rpc_client: CMakeFiles/test_rpc_client.dir/test/test_rpc/test_rpc_client.cpp.o
test_rpc_client: CMakeFiles/test_rpc_client.dir/build.make
test_rpc_client: CMakeFiles/test_rpc_client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/skyfire/CLionProjects/sflib/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_rpc_client"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_rpc_client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_rpc_client.dir/build: test_rpc_client

.PHONY : CMakeFiles/test_rpc_client.dir/build

CMakeFiles/test_rpc_client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_rpc_client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_rpc_client.dir/clean

CMakeFiles/test_rpc_client.dir/depend:
	cd /home/skyfire/CLionProjects/sflib/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/skyfire/CLionProjects/sflib /home/skyfire/CLionProjects/sflib /home/skyfire/CLionProjects/sflib/cmake-build-debug /home/skyfire/CLionProjects/sflib/cmake-build-debug /home/skyfire/CLionProjects/sflib/cmake-build-debug/CMakeFiles/test_rpc_client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_rpc_client.dir/depend

