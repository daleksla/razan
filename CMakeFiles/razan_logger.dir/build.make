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
include CMakeFiles/razan_logger.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/razan_logger.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/razan_logger.dir/flags.make

CMakeFiles/razan_logger.dir/src/logger.c.o: CMakeFiles/razan_logger.dir/flags.make
CMakeFiles/razan_logger.dir/src/logger.c.o: src/logger.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/salih/code/projects/razan/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/razan_logger.dir/src/logger.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/razan_logger.dir/src/logger.c.o   -c /home/salih/code/projects/razan/src/logger.c

CMakeFiles/razan_logger.dir/src/logger.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/razan_logger.dir/src/logger.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/salih/code/projects/razan/src/logger.c > CMakeFiles/razan_logger.dir/src/logger.c.i

CMakeFiles/razan_logger.dir/src/logger.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/razan_logger.dir/src/logger.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/salih/code/projects/razan/src/logger.c -o CMakeFiles/razan_logger.dir/src/logger.c.s

# Object files for target razan_logger
razan_logger_OBJECTS = \
"CMakeFiles/razan_logger.dir/src/logger.c.o"

# External object files for target razan_logger
razan_logger_EXTERNAL_OBJECTS =

lib/librazan_logger.a: CMakeFiles/razan_logger.dir/src/logger.c.o
lib/librazan_logger.a: CMakeFiles/razan_logger.dir/build.make
lib/librazan_logger.a: CMakeFiles/razan_logger.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/salih/code/projects/razan/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library lib/librazan_logger.a"
	$(CMAKE_COMMAND) -P CMakeFiles/razan_logger.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/razan_logger.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/razan_logger.dir/build: lib/librazan_logger.a

.PHONY : CMakeFiles/razan_logger.dir/build

CMakeFiles/razan_logger.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/razan_logger.dir/cmake_clean.cmake
.PHONY : CMakeFiles/razan_logger.dir/clean

CMakeFiles/razan_logger.dir/depend:
	cd /home/salih/code/projects/razan && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/salih/code/projects/razan /home/salih/code/projects/razan /home/salih/code/projects/razan /home/salih/code/projects/razan /home/salih/code/projects/razan/CMakeFiles/razan_logger.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/razan_logger.dir/depend

