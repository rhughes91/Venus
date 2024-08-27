# Venus Game Engine
The Venus Game Engine is a C++ framework meant to simplify game development.

## Usage
For the simplest experience, any code for game development should placed within the **projects** folder. The engine uses [CMake](https://cmake.org/download/) 
to create a portable Windows or Linux buid. It currently supports the [MingGW](https://www.mingw-w64.org/) compiler, MSVC (Visual Studio), and g++ for Linux.

If a new `.cpp` file is added to a project directory, then that file's name will need to be added to the `PROJECT_SRC` variable in its *CMakeFileLists.txt* file. A *main.cpp* script is required in every project made with Venus, but it does not need to be added to the `PROJECT_SRC` variable. If a new directory is added to the **projects** folder, the CMake Pipeline will need to be rebuilt (this will add a *CMakeFileLists.txt* file and a *main.cpp* file).

A supplementary script is provided to export built projects into the **export** folder. The command `build-tools/main {Project Name}` will export the executable and it's resources to this folder with a directory matching the project name. These exported files can be safely zipped and distributed to other Windows or Linux machines (depending on how the project was compiled).

## CMake Commands
### Example Project
If only the TestProject is being built, basic CMake commands can be used in the **build** directory.

#### (Re)build CMake Pipeline
```
cmake ..
```

#### Build project
```
cmake --build .
```

### Other Cases
If other projects are being built, CMake will need more details to accurately build the project. These commands should be run in the main **Venus** directory. The *{Project Name}* should be replaced with the name of your directory in the **projects** folder. For example, if you added a directory called "Platformer" in the **projects** folder, you would need to run `cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -S . -B build/Platformer` in order to make the CMake pipeline using MinGW. For the TestProject example provided, *{Project Name}* can simply be replaced with *TestProject*.

#### (Re)build CMake Pipeline
<ins>*MinGW*</ins>
```
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -S . -B build/{Project Name}
```

<ins>*MSVC*</ins>
```
cmake -S . -B build/{Project Name}
```

<ins>*g++*</ins>
```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/{Project Name}
```

#### Build project
```
cmake --build build/{Project Name}
```

## Contributions
At its current state, the Venus Game Engine is not yet open to contributions.
