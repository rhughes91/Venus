cmake_minimum_required(VERSION 3.25.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

get_filename_component(DIR_NAME ${CMAKE_BINARY_DIR} NAME)
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/path.config)
    if(${DIR_NAME} STREQUAL "build")
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/default_resources/path_test.config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
        file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/path_test.config ${CMAKE_CURRENT_BINARY_DIR}/path.config)
    else()
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/default_resources/path.config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif()
endif()