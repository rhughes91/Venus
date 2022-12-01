# Venus Game Engine
The Venus Game Engine is a C++ framework meant to simplify game development.

## Usage
For the simplest experience, any code for game development can be built from code within the **example** folder. The engine uses [CMake](https://cmake.org/download/) 
to create a portable Windows buid. The commands to use CMake make use of the [MingGW](https://www.mingw-w64.org/) compiler, but other compilers should work just as well with CMake.

### CMake Commands
```bash
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -S . -B build/Venus/cmake
```
```bash
cmake --build build/Venus/cmake
```

## Contributions
At its current state, the Venus Game Engine is not yet open to contributions.
