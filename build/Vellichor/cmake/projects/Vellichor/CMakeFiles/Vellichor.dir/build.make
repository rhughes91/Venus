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
CMAKE_BINARY_DIR = C:\Projects\Venus\build\Vellichor\cmake

# Include any dependencies generated for this target.
include projects/Vellichor/CMakeFiles/Vellichor.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include projects/Vellichor/CMakeFiles/Vellichor.dir/compiler_depend.make

# Include the progress variables for this target.
include projects/Vellichor/CMakeFiles/Vellichor.dir/progress.make

# Include the compile flags for this target's objects.
include projects/Vellichor/CMakeFiles/Vellichor.dir/flags.make

projects/Vellichor/CMakeFiles/Vellichor.dir/main.cpp.obj: projects/Vellichor/CMakeFiles/Vellichor.dir/flags.make
projects/Vellichor/CMakeFiles/Vellichor.dir/main.cpp.obj: projects/Vellichor/CMakeFiles/Vellichor.dir/includes_CXX.rsp
projects/Vellichor/CMakeFiles/Vellichor.dir/main.cpp.obj: C:/Projects/Venus/projects/Vellichor/main.cpp
projects/Vellichor/CMakeFiles/Vellichor.dir/main.cpp.obj: projects/Vellichor/CMakeFiles/Vellichor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Projects\Venus\build\Vellichor\cmake\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object projects/Vellichor/CMakeFiles/Vellichor.dir/main.cpp.obj"
	cd /d C:\Projects\Venus\build\Vellichor\cmake\projects\Vellichor && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT projects/Vellichor/CMakeFiles/Vellichor.dir/main.cpp.obj -MF CMakeFiles\Vellichor.dir\main.cpp.obj.d -o CMakeFiles\Vellichor.dir\main.cpp.obj -c C:\Projects\Venus\projects\Vellichor\main.cpp

projects/Vellichor/CMakeFiles/Vellichor.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Vellichor.dir/main.cpp.i"
	cd /d C:\Projects\Venus\build\Vellichor\cmake\projects\Vellichor && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Projects\Venus\projects\Vellichor\main.cpp > CMakeFiles\Vellichor.dir\main.cpp.i

projects/Vellichor/CMakeFiles/Vellichor.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Vellichor.dir/main.cpp.s"
	cd /d C:\Projects\Venus\build\Vellichor\cmake\projects\Vellichor && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Projects\Venus\projects\Vellichor\main.cpp -o CMakeFiles\Vellichor.dir\main.cpp.s

# Object files for target Vellichor
Vellichor_OBJECTS = \
"CMakeFiles/Vellichor.dir/main.cpp.obj"

# External object files for target Vellichor
Vellichor_EXTERNAL_OBJECTS =

C:/Projects/Venus/build/Vellichor/Vellichor.exe: projects/Vellichor/CMakeFiles/Vellichor.dir/main.cpp.obj
C:/Projects/Venus/build/Vellichor/Vellichor.exe: projects/Vellichor/CMakeFiles/Vellichor.dir/build.make
C:/Projects/Venus/build/Vellichor/Vellichor.exe: projects/Vellichor/libVellichor_lib.a
C:/Projects/Venus/build/Vellichor/Vellichor.exe: src/libvenus_lib.a
C:/Projects/Venus/build/Vellichor/Vellichor.exe: vendor/libvendor_lib.a
C:/Projects/Venus/build/Vellichor/Vellichor.exe: src/libecs_lib.a
C:/Projects/Venus/build/Vellichor/Vellichor.exe: src/libfile_lib.a
C:/Projects/Venus/build/Vellichor/Vellichor.exe: C:/Projects/Venus/lib/libglfw3dll.a
C:/Projects/Venus/build/Vellichor/Vellichor.exe: projects/Vellichor/CMakeFiles/Vellichor.dir/linkLibs.rsp
C:/Projects/Venus/build/Vellichor/Vellichor.exe: projects/Vellichor/CMakeFiles/Vellichor.dir/objects1
C:/Projects/Venus/build/Vellichor/Vellichor.exe: projects/Vellichor/CMakeFiles/Vellichor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Projects\Venus\build\Vellichor\cmake\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable C:\Projects\Venus\build\Vellichor\Vellichor.exe"
	cd /d C:\Projects\Venus\build\Vellichor\cmake\projects\Vellichor && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Vellichor.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
projects/Vellichor/CMakeFiles/Vellichor.dir/build: C:/Projects/Venus/build/Vellichor/Vellichor.exe
.PHONY : projects/Vellichor/CMakeFiles/Vellichor.dir/build

projects/Vellichor/CMakeFiles/Vellichor.dir/clean:
	cd /d C:\Projects\Venus\build\Vellichor\cmake\projects\Vellichor && $(CMAKE_COMMAND) -P CMakeFiles\Vellichor.dir\cmake_clean.cmake
.PHONY : projects/Vellichor/CMakeFiles/Vellichor.dir/clean

projects/Vellichor/CMakeFiles/Vellichor.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Projects\Venus C:\Projects\Venus\projects\Vellichor C:\Projects\Venus\build\Vellichor\cmake C:\Projects\Venus\build\Vellichor\cmake\projects\Vellichor C:\Projects\Venus\build\Vellichor\cmake\projects\Vellichor\CMakeFiles\Vellichor.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : projects/Vellichor/CMakeFiles/Vellichor.dir/depend
