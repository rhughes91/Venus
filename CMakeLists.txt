cmake_minimum_required(VERSION 3.25.0)

# set(DCMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")
# set(DCMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")

project(Venus)

set(CMAKE_SKIP_INSTALL_RULES True)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(vendor)

option(BUILD_EXAMPLES "Whether or not to build examples" ON)

if(BUILD_EXAMPLES)
    message(STATUS "Building examples...")
    add_subdirectory(projects)
endif()

IF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../glfw3.dll)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/GLFW/glfw3.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/..)
endif()

IF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../path.config)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/default_resources/path.config DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/..)
endif()

IF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../resources)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../resources)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/default_resources/audio DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../resources)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/default_resources/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../resources)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/default_resources/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../resources)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/default_resources/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../resources)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../resources/models)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/default_resources/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../resources)
endif()