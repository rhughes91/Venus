# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.25

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Projects\Venus

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Projects\Venus\build\Venus\cmake

# Include any dependencies generated for this target.
include src/CMakeFiles/ecs_lib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/ecs_lib.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/ecs_lib.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/ecs_lib.dir/flags.make

src/CMakeFiles/ecs_lib.dir/structure.cpp.obj: src/CMakeFiles/ecs_lib.dir/flags.make
src/CMakeFiles/ecs_lib.dir/structure.cpp.obj: src/CMakeFiles/ecs_lib.dir/includes_CXX.rsp
src/CMakeFiles/ecs_lib.dir/structure.cpp.obj: C:/Projects/Venus/src/structure.cpp
src/CMakeFiles/ecs_lib.dir/structure.cpp.obj: src/CMakeFiles/ecs_lib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Projects\Venus\build\Venus\cmake\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/ecs_lib.dir/structure.cpp.obj"
	cd /d C:\Projects\Venus\build\Venus\cmake\src && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/ecs_lib.dir/structure.cpp.obj -MF CMakeFiles\ecs_lib.dir\structure.cpp.obj.d -o CMakeFiles\ecs_lib.dir\structure.cpp.obj -c C:\Projects\Venus\src\structure.cpp

src/CMakeFiles/ecs_lib.dir/structure.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ecs_lib.dir/structure.cpp.i"
	cd /d C:\Projects\Venus\build\Venus\cmake\src && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Projects\Venus\src\structure.cpp > CMakeFiles\ecs_lib.dir\structure.cpp.i

src/CMakeFiles/ecs_lib.dir/structure.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ecs_lib.dir/structure.cpp.s"
	cd /d C:\Projects\Venus\build\Venus\cmake\src && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Projects\Venus\src\structure.cpp -o CMakeFiles\ecs_lib.dir\structure.cpp.s

# Object files for target ecs_lib
ecs_lib_OBJECTS = \
"CMakeFiles/ecs_lib.dir/structure.cpp.obj"

# External object files for target ecs_lib
ecs_lib_EXTERNAL_OBJECTS =

src/libecs_lib.a: src/CMakeFiles/ecs_lib.dir/structure.cpp.obj
src/libecs_lib.a: src/CMakeFiles/ecs_lib.dir/build.make
src/libecs_lib.a: src/CMakeFiles/ecs_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Projects\Venus\build\Venus\cmake\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libecs_lib.a"
	cd /d C:\Projects\Venus\build\Venus\cmake\src && $(CMAKE_COMMAND) -P CMakeFiles\ecs_lib.dir\cmake_clean_target.cmake
	cd /d C:\Projects\Venus\build\Venus\cmake\src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\ecs_lib.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/ecs_lib.dir/build: src/libecs_lib.a
.PHONY : src/CMakeFiles/ecs_lib.dir/build

src/CMakeFiles/ecs_lib.dir/clean:
	cd /d C:\Projects\Venus\build\Venus\cmake\src && $(CMAKE_COMMAND) -P CMakeFiles\ecs_lib.dir\cmake_clean.cmake
.PHONY : src/CMakeFiles/ecs_lib.dir/clean

src/CMakeFiles/ecs_lib.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Projects\Venus C:\Projects\Venus\src C:\Projects\Venus\build\Venus\cmake C:\Projects\Venus\build\Venus\cmake\src C:\Projects\Venus\build\Venus\cmake\src\CMakeFiles\ecs_lib.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/ecs_lib.dir/depend

