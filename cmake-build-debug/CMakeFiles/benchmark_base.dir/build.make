# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/elijah/Documents/workspace/benchmark_base

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/elijah/Documents/workspace/benchmark_base/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/benchmark_base.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/benchmark_base.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/benchmark_base.dir/flags.make

CMakeFiles/benchmark_base.dir/main.cpp.o: CMakeFiles/benchmark_base.dir/flags.make
CMakeFiles/benchmark_base.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/elijah/Documents/workspace/benchmark_base/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/benchmark_base.dir/main.cpp.o"
	/usr/local/bin/g++-9  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/benchmark_base.dir/main.cpp.o -c /Users/elijah/Documents/workspace/benchmark_base/main.cpp

CMakeFiles/benchmark_base.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/benchmark_base.dir/main.cpp.i"
	/usr/local/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/elijah/Documents/workspace/benchmark_base/main.cpp > CMakeFiles/benchmark_base.dir/main.cpp.i

CMakeFiles/benchmark_base.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/benchmark_base.dir/main.cpp.s"
	/usr/local/bin/g++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/elijah/Documents/workspace/benchmark_base/main.cpp -o CMakeFiles/benchmark_base.dir/main.cpp.s

# Object files for target benchmark_base
benchmark_base_OBJECTS = \
"CMakeFiles/benchmark_base.dir/main.cpp.o"

# External object files for target benchmark_base
benchmark_base_EXTERNAL_OBJECTS =

benchmark_base: CMakeFiles/benchmark_base.dir/main.cpp.o
benchmark_base: CMakeFiles/benchmark_base.dir/build.make
benchmark_base: CMakeFiles/benchmark_base.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/elijah/Documents/workspace/benchmark_base/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable benchmark_base"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/benchmark_base.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/benchmark_base.dir/build: benchmark_base

.PHONY : CMakeFiles/benchmark_base.dir/build

CMakeFiles/benchmark_base.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/benchmark_base.dir/cmake_clean.cmake
.PHONY : CMakeFiles/benchmark_base.dir/clean

CMakeFiles/benchmark_base.dir/depend:
	cd /Users/elijah/Documents/workspace/benchmark_base/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/elijah/Documents/workspace/benchmark_base /Users/elijah/Documents/workspace/benchmark_base /Users/elijah/Documents/workspace/benchmark_base/cmake-build-debug /Users/elijah/Documents/workspace/benchmark_base/cmake-build-debug /Users/elijah/Documents/workspace/benchmark_base/cmake-build-debug/CMakeFiles/benchmark_base.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/benchmark_base.dir/depend
