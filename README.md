# Venus Game Engine
The Venus Game Engine is a C++ framework meant to simplify game development.

## Usage
For the simplest experience, any code for game development should placed within the **projects** folder. The engine uses [CMake](https://cmake.org/download/) 
to create a portable Windows buid. It currently supports the [MingGW](https://www.mingw-w64.org/) compiler, MSVC (Visual Studio), and g++ for Linux.

If a new `.cpp` file is added to a project directory, then that file's name will need to be added to the `PROJECT_SRC` variable in its *CMakeFileLists.txt* file. A *main.cpp* script is required in every project made with Venus, but it does not need to be added to the `PROJECT_SRC` variable. If a new directory is added to the **projects** folder, the CMake Pipeline will need to be rebuilt (this will add a *CMakeFileLists.txt* file and a *main.cpp* file).

A supplementary script is provided to export built projects into a separate **Builds** folder. This folder will be generated in the same directory that the **Venus** directory is placed. The command `build-tools/main {Project Name}` will export the executable and it's resources to the external **Builds** folder with a directory matching the project name. These exported files can be safely zipped and distributed to other Windows or Linux machines (depending on how the project was compiled).

### CMake Commands
*(Re)build CMake Pipeline*  
**MinGW**: 
``
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -S . -B build/{Project Name}
``

**MSVC**: 
``
cmake -S . -B build/{Project Name}
``

**g++**: 
``
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/{Project Name}
``

*Build project:*  
``
cmake --build build/{Project Name}
``

## Contributions
At its current state, the Venus Game Engine is not yet open to contributions.
