# Venus Game Engine
The **Venus Game Engine** is a C++ framework meant to simplify game development.

## About
**Venus** uses [CMake](https://cmake.org/download/) 
to create a portable Windows or Linux buid. It currently supports the Visual Studio, [MingGW](https://www.mingw-w64.org/), and [Ninja](https://ninja-build.org/) build environments. It can currently utilize the MSVC or g++ compiler.

## Build
### Projects
Any projects in the `PROJECT_DIRECTORY` can be easily built with CMake. The default value of this variable is the **projects** directory in **Venus**, but that value can be changed when generating the build system.

#### Create Build System
```
cmake <source-path> -B <build-path>/<project-name> -G <generator> -DPROJECT_DIRECTORY=<projects-path>
```
It's important to note that the generator and projects directory do not need to be explicitly set. The generator will default to one available on your system, and the default projects directory was explained above.

In order to target a specific project, the build path *must end* in a directory with the same name. Assuming the user to be in the **build** directory within **Venus**, in order to target **TestProject** in the **projects** directory, the user can simply call:
```
cmake .. -B TestProject
```

#### Build Project
```
cmake --build <build-path>
```
The build path here must match the one used in the creation of the build system. An executable will be built in this location that the user can easily run. Using the previous example, it may simply be `TestProject`:
```
cmake --build TestProject
```

#### Install/Export Project
```
cmake --install <build-path> --prefix <install-path>
```
All the installation process consists of is copy and pasting all the required files into a directory of the user's choice. This just includes the `.exe`, `path.config`, and `resources` files. If the prefix is not set, CMake will place the files into the **bin** directory. Following the previous example, the command would be:
```
cmake --install TestProject
```

### Default Project
In the case that the set built directory does not match any project name, a default project will be build instead. This value (`DEFAULT_PROJECT`) is set to `TestProject` by default, but it can be altered by the user when creating the build system.
#### Create Build System
```
cmake <source-path> -B <build-path> -G <generator> -DPROJECT_DIRECTORY=<projects-path> -DDEFAULT_PROJECT=<project-name>
```
All the previous rules apply with this command, except the build directory can have any arbitrary name. If the user wanted to generate a default project called `Example` directly in the **build** directory, that would look like this (assuming the user is still in the **build** directory):
```
cmake .. -DDEFAULT_PROJECT=Example
```
```
cmake --build .
```
```
cmake --install .
```
For building and installation, keep in mind that the build path is no longer the name of the project. The user simply needs to reference whatever build directory they had previously chosen.

## Usage
**Venus** allows for project, build, and binary files to be placed anywhere the user specifies (inside and out of source). By default, **Venus** searches for project files in the **project** directory and exports binary files to the **bin** directory.

If a new `.cpp` file is added to a project directory, then that file will need to be added to the `PROJECT_SRC` variable in its *CMakeFileLists.txt* file.
```
project(Venus)

set(PROJECT_SRC example1.cpp example2.cpp)
```
A `main.cpp` script is required in every project made with Venus, but it does not need to be added to this variable. As long as the directory exists, a `main.cpp`, `CMakeLists.txt`, and `resources` directory will be automatically added to the project. If the directory *does not* exist, the default project will be generated instead.

## Contributions
At its current state, the Venus Game Engine is not yet open to contributions.
