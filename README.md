# Venus Game Engine
The Venus Game Engine is a C++ framework meant to simplify game development.

## Usage
For the simplest experience, any code for game development should placed within the **projects** folder. The engine uses [CMake](https://cmake.org/download/) 
to create a portable Windows buid. The commands to use CMake make use of the [MingGW](https://www.mingw-w64.org/) compiler, but other compilers should work just as well with CMake. If a new `.cpp` file is added to a project file, then that file's name will need to be added to the `PROJECT_SRC` variable in its *CMakeFileLists.txt* file. A *main.cpp* script is required in every project made with Venus, but it does not need to be added to the `PROJECT_SRC` variable. If a new directory is added to the **projects** folder, the CMake Pipeline will need to be rebuilt (this will add a *CMakeFileLists.txt* file and a *main.cpp* file).

A supplementary script is provided to export built projects into a separate **Builds** folder. This folder will be generated in the same directory that the **Venus** directory is placed. The command `build-tools/main {Project Name}` will begin that export, provided that the project has been built by CMake. This command must be run inside the project file you wish to build, otherwise it will fail. These exported files can be safely zipped and distributed to run on other Windows machines.

### CMake Commands

*(Re)build CMake Pipeline:*  
``
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -S . -B build/{Project Name}/cmake
``

*Build project:*  
``
cmake --build build/{Project Name}/cmake
``

## Contributions
At its current state, the Venus Game Engine is not yet open to contributions.
