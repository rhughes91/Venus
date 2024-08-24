set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(c_path "C:/linux_gcc/13.3_rel1/bin")
set(target "aarch64-none-linux-gnu-")

set(CMAKE_C_COMPILER "${c_path}/${target}gcc.exe")
set(CMAKE_CXX_COMPILER "${c_path}/${target}g++.exe")
SET(CMAKE_FIND_ROOT_PATH  "C:/linux_gcc/13.3_rel1/aarch64-none-linux-gnu/include/c++/13.3.1") 

include_directories("C:/linux_gcc/glibc")
include_directories("C:/linux_gcc/glibc/include")
include_directories("C:/linux_gcc/build-essential")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_SYSROOT "C:/ubuntux86-64")

set(CMAKE_C_FLAGS "--sysroot=${CMAKE_SYSROOT}")
set(CMAKE_CXX_FLAGS "--sysroot=${CMAKE_SYSROOT}")