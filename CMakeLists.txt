cmake_minimum_required(VERSION 3.25.0)
project(Venus)

set(CMAKE_SKIP_INSTALL_RULES True)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(vendor)

option(BUILD_EXAMPLES "Whether or not to build examples" ON)

if(BUILD_EXAMPLES)
    message(STATUS "Building examples...")
    add_subdirectory(example)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/glfw3.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/..)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/..)