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
CMAKE_BINARY_DIR = C:\Projects\Venus\build\TestProject\cmake

# Include any dependencies generated for this target.
include vendor/CMakeFiles/vendor_lib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include vendor/CMakeFiles/vendor_lib.dir/compiler_depend.make

# Include the progress variables for this target.
include vendor/CMakeFiles/vendor_lib.dir/progress.make

# Include the compile flags for this target's objects.
include vendor/CMakeFiles/vendor_lib.dir/flags.make

vendor/CMakeFiles/vendor_lib.dir/glad.c.obj: vendor/CMakeFiles/vendor_lib.dir/flags.make
vendor/CMakeFiles/vendor_lib.dir/glad.c.obj: vendor/CMakeFiles/vendor_lib.dir/includes_C.rsp
vendor/CMakeFiles/vendor_lib.dir/glad.c.obj: C:/Projects/Venus/vendor/glad.c
vendor/CMakeFiles/vendor_lib.dir/glad.c.obj: vendor/CMakeFiles/vendor_lib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Projects\Venus\build\TestProject\cmake\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object vendor/CMakeFiles/vendor_lib.dir/glad.c.obj"
	cd /d C:\Projects\Venus\build\TestProject\cmake\vendor && C:\msys64\mingw64\bin\cc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT vendor/CMakeFiles/vendor_lib.dir/glad.c.obj -MF CMakeFiles\vendor_lib.dir\glad.c.obj.d -o CMakeFiles\vendor_lib.dir\glad.c.obj -c C:\Projects\Venus\vendor\glad.c

vendor/CMakeFiles/vendor_lib.dir/glad.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vendor_lib.dir/glad.c.i"
	cd /d C:\Projects\Venus\build\TestProject\cmake\vendor && C:\msys64\mingw64\bin\cc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Projects\Venus\vendor\glad.c > CMakeFiles\vendor_lib.dir\glad.c.i

vendor/CMakeFiles/vendor_lib.dir/glad.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vendor_lib.dir/glad.c.s"
	cd /d C:\Projects\Venus\build\TestProject\cmake\vendor && C:\msys64\mingw64\bin\cc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Projects\Venus\vendor\glad.c -o CMakeFiles\vendor_lib.dir\glad.c.s

vendor/CMakeFiles/vendor_lib.dir/stb_image.cpp.obj: vendor/CMakeFiles/vendor_lib.dir/flags.make
vendor/CMakeFiles/vendor_lib.dir/stb_image.cpp.obj: vendor/CMakeFiles/vendor_lib.dir/includes_CXX.rsp
vendor/CMakeFiles/vendor_lib.dir/stb_image.cpp.obj: C:/Projects/Venus/vendor/stb_image.cpp
vendor/CMakeFiles/vendor_lib.dir/stb_image.cpp.obj: vendor/CMakeFiles/vendor_lib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Projects\Venus\build\TestProject\cmake\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object vendor/CMakeFiles/vendor_lib.dir/stb_image.cpp.obj"
	cd /d C:\Projects\Venus\build\TestProject\cmake\vendor && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT vendor/CMakeFiles/vendor_lib.dir/stb_image.cpp.obj -MF CMakeFiles\vendor_lib.dir\stb_image.cpp.obj.d -o CMakeFiles\vendor_lib.dir\stb_image.cpp.obj -c C:\Projects\Venus\vendor\stb_image.cpp

vendor/CMakeFiles/vendor_lib.dir/stb_image.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vendor_lib.dir/stb_image.cpp.i"
	cd /d C:\Projects\Venus\build\TestProject\cmake\vendor && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Projects\Venus\vendor\stb_image.cpp > CMakeFiles\vendor_lib.dir\stb_image.cpp.i

vendor/CMakeFiles/vendor_lib.dir/stb_image.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vendor_lib.dir/stb_image.cpp.s"
	cd /d C:\Projects\Venus\build\TestProject\cmake\vendor && C:\msys64\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Projects\Venus\vendor\stb_image.cpp -o CMakeFiles\vendor_lib.dir\stb_image.cpp.s

# Object files for target vendor_lib
vendor_lib_OBJECTS = \
"CMakeFiles/vendor_lib.dir/glad.c.obj" \
"CMakeFiles/vendor_lib.dir/stb_image.cpp.obj"

# External object files for target vendor_lib
vendor_lib_EXTERNAL_OBJECTS =

vendor/libvendor_lib.a: vendor/CMakeFiles/vendor_lib.dir/glad.c.obj
vendor/libvendor_lib.a: vendor/CMakeFiles/vendor_lib.dir/stb_image.cpp.obj
vendor/libvendor_lib.a: vendor/CMakeFiles/vendor_lib.dir/build.make
vendor/libvendor_lib.a: vendor/CMakeFiles/vendor_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Projects\Venus\build\TestProject\cmake\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libvendor_lib.a"
	cd /d C:\Projects\Venus\build\TestProject\cmake\vendor && $(CMAKE_COMMAND) -P CMakeFiles\vendor_lib.dir\cmake_clean_target.cmake
	cd /d C:\Projects\Venus\build\TestProject\cmake\vendor && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\vendor_lib.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
vendor/CMakeFiles/vendor_lib.dir/build: vendor/libvendor_lib.a
.PHONY : vendor/CMakeFiles/vendor_lib.dir/build

vendor/CMakeFiles/vendor_lib.dir/clean:
	cd /d C:\Projects\Venus\build\TestProject\cmake\vendor && $(CMAKE_COMMAND) -P CMakeFiles\vendor_lib.dir\cmake_clean.cmake
.PHONY : vendor/CMakeFiles/vendor_lib.dir/clean

vendor/CMakeFiles/vendor_lib.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Projects\Venus C:\Projects\Venus\vendor C:\Projects\Venus\build\TestProject\cmake C:\Projects\Venus\build\TestProject\cmake\vendor C:\Projects\Venus\build\TestProject\cmake\vendor\CMakeFiles\vendor_lib.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : vendor/CMakeFiles/vendor_lib.dir/depend

