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
CMAKE_BINARY_DIR = C:\Projects\Venus\build\TestProject

# Include any dependencies generated for this target.
include projects/TestProject/CMakeFiles/TestProject.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include projects/TestProject/CMakeFiles/TestProject.dir/compiler_depend.make

# Include the progress variables for this target.
include projects/TestProject/CMakeFiles/TestProject.dir/progress.make

# Include the compile flags for this target's objects.
include projects/TestProject/CMakeFiles/TestProject.dir/flags.make

projects/TestProject/CMakeFiles/TestProject.dir/project.cpp.obj: projects/TestProject/CMakeFiles/TestProject.dir/flags.make
projects/TestProject/CMakeFiles/TestProject.dir/project.cpp.obj: projects/TestProject/CMakeFiles/TestProject.dir/includes_CXX.rsp
projects/TestProject/CMakeFiles/TestProject.dir/project.cpp.obj: C:/Projects/Venus/projects/TestProject/project.cpp
projects/TestProject/CMakeFiles/TestProject.dir/project.cpp.obj: projects/TestProject/CMakeFiles/TestProject.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Projects\Venus\build\TestProject\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object projects/TestProject/CMakeFiles/TestProject.dir/project.cpp.obj"
	cd /d C:\Projects\Venus\build\TestProject\projects\TestProject && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT projects/TestProject/CMakeFiles/TestProject.dir/project.cpp.obj -MF CMakeFiles\TestProject.dir\project.cpp.obj.d -o CMakeFiles\TestProject.dir\project.cpp.obj -c C:\Projects\Venus\projects\TestProject\project.cpp

projects/TestProject/CMakeFiles/TestProject.dir/project.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TestProject.dir/project.cpp.i"
	cd /d C:\Projects\Venus\build\TestProject\projects\TestProject && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Projects\Venus\projects\TestProject\project.cpp > CMakeFiles\TestProject.dir\project.cpp.i

projects/TestProject/CMakeFiles/TestProject.dir/project.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TestProject.dir/project.cpp.s"
	cd /d C:\Projects\Venus\build\TestProject\projects\TestProject && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Projects\Venus\projects\TestProject\project.cpp -o CMakeFiles\TestProject.dir\project.cpp.s

projects/TestProject/CMakeFiles/TestProject.dir/main.cpp.obj: projects/TestProject/CMakeFiles/TestProject.dir/flags.make
projects/TestProject/CMakeFiles/TestProject.dir/main.cpp.obj: projects/TestProject/CMakeFiles/TestProject.dir/includes_CXX.rsp
projects/TestProject/CMakeFiles/TestProject.dir/main.cpp.obj: C:/Projects/Venus/projects/TestProject/main.cpp
projects/TestProject/CMakeFiles/TestProject.dir/main.cpp.obj: projects/TestProject/CMakeFiles/TestProject.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Projects\Venus\build\TestProject\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object projects/TestProject/CMakeFiles/TestProject.dir/main.cpp.obj"
	cd /d C:\Projects\Venus\build\TestProject\projects\TestProject && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT projects/TestProject/CMakeFiles/TestProject.dir/main.cpp.obj -MF CMakeFiles\TestProject.dir\main.cpp.obj.d -o CMakeFiles\TestProject.dir\main.cpp.obj -c C:\Projects\Venus\projects\TestProject\main.cpp

projects/TestProject/CMakeFiles/TestProject.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TestProject.dir/main.cpp.i"
	cd /d C:\Projects\Venus\build\TestProject\projects\TestProject && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Projects\Venus\projects\TestProject\main.cpp > CMakeFiles\TestProject.dir\main.cpp.i

projects/TestProject/CMakeFiles/TestProject.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TestProject.dir/main.cpp.s"
	cd /d C:\Projects\Venus\build\TestProject\projects\TestProject && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Projects\Venus\projects\TestProject\main.cpp -o CMakeFiles\TestProject.dir\main.cpp.s

# Object files for target TestProject
TestProject_OBJECTS = \
"CMakeFiles/TestProject.dir/project.cpp.obj" \
"CMakeFiles/TestProject.dir/main.cpp.obj"

# External object files for target TestProject
TestProject_EXTERNAL_OBJECTS =

TestProject.exe: projects/TestProject/CMakeFiles/TestProject.dir/project.cpp.obj
TestProject.exe: projects/TestProject/CMakeFiles/TestProject.dir/main.cpp.obj
TestProject.exe: projects/TestProject/CMakeFiles/TestProject.dir/build.make
TestProject.exe: src/libvenus_lib.a
TestProject.exe: src/libgraphics_lib.a
TestProject.exe: vendor/libvendor_lib.a
TestProject.exe: src/libfile_lib.a
TestProject.exe: src/libaudio_lib.a
TestProject.exe: src/libmath_lib.a
TestProject.exe: C:/Projects/Venus/lib/libglfw3.a
TestProject.exe: projects/TestProject/CMakeFiles/TestProject.dir/linkLibs.rsp
TestProject.exe: projects/TestProject/CMakeFiles/TestProject.dir/objects1
TestProject.exe: projects/TestProject/CMakeFiles/TestProject.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Projects\Venus\build\TestProject\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ..\..\TestProject.exe"
	cd /d C:\Projects\Venus\build\TestProject\projects\TestProject && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\TestProject.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
projects/TestProject/CMakeFiles/TestProject.dir/build: TestProject.exe
.PHONY : projects/TestProject/CMakeFiles/TestProject.dir/build

projects/TestProject/CMakeFiles/TestProject.dir/clean:
	cd /d C:\Projects\Venus\build\TestProject\projects\TestProject && $(CMAKE_COMMAND) -P CMakeFiles\TestProject.dir\cmake_clean.cmake
.PHONY : projects/TestProject/CMakeFiles/TestProject.dir/clean

projects/TestProject/CMakeFiles/TestProject.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Projects\Venus C:\Projects\Venus\projects\TestProject C:\Projects\Venus\build\TestProject C:\Projects\Venus\build\TestProject\projects\TestProject C:\Projects\Venus\build\TestProject\projects\TestProject\CMakeFiles\TestProject.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : projects/TestProject/CMakeFiles/TestProject.dir/depend
