# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/salih/code/projects/razan

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/salih/code/projects/razan

# Include any dependencies generated for this target.
include CMakeFiles/razan_client_chat.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/razan_client_chat.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/razan_client_chat.dir/flags.make

CMakeFiles/razan_client_chat.dir/src/client_chat.cc.o: CMakeFiles/razan_client_chat.dir/flags.make
CMakeFiles/razan_client_chat.dir/src/client_chat.cc.o: src/client_chat.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/salih/code/projects/razan/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/razan_client_chat.dir/src/client_chat.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/razan_client_chat.dir/src/client_chat.cc.o -c /home/salih/code/projects/razan/src/client_chat.cc

CMakeFiles/razan_client_chat.dir/src/client_chat.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/razan_client_chat.dir/src/client_chat.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/salih/code/projects/razan/src/client_chat.cc > CMakeFiles/razan_client_chat.dir/src/client_chat.cc.i

CMakeFiles/razan_client_chat.dir/src/client_chat.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/razan_client_chat.dir/src/client_chat.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/salih/code/projects/razan/src/client_chat.cc -o CMakeFiles/razan_client_chat.dir/src/client_chat.cc.s

# Object files for target razan_client_chat
razan_client_chat_OBJECTS = \
"CMakeFiles/razan_client_chat.dir/src/client_chat.cc.o"

# External object files for target razan_client_chat
razan_client_chat_EXTERNAL_OBJECTS =

lib/librazan_client_chat.a: CMakeFiles/razan_client_chat.dir/src/client_chat.cc.o
lib/librazan_client_chat.a: CMakeFiles/razan_client_chat.dir/build.make
lib/librazan_client_chat.a: CMakeFiles/razan_client_chat.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/salih/code/projects/razan/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library lib/librazan_client_chat.a"
	$(CMAKE_COMMAND) -P CMakeFiles/razan_client_chat.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/razan_client_chat.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/razan_client_chat.dir/build: lib/librazan_client_chat.a

.PHONY : CMakeFiles/razan_client_chat.dir/build

CMakeFiles/razan_client_chat.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/razan_client_chat.dir/cmake_clean.cmake
.PHONY : CMakeFiles/razan_client_chat.dir/clean

CMakeFiles/razan_client_chat.dir/depend:
	cd /home/salih/code/projects/razan && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/salih/code/projects/razan /home/salih/code/projects/razan /home/salih/code/projects/razan /home/salih/code/projects/razan /home/salih/code/projects/razan/CMakeFiles/razan_client_chat.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/razan_client_chat.dir/depend

