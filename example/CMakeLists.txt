project(Venus)

set(VELL_SRC vellichor.cpp)
add_library(vell_lib ${VELL_SRC})
target_include_directories(vell_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../example ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/Venus)
add_executable(main main.cpp)

target_link_libraries(main PRIVATE vell_lib venus_lib vendor_lib ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a -static-libgcc -static-libstdc++)

option(RMV_CONSOLE "" OFF)
if(RMV_CONSOLE)
    target_link_options(main PRIVATE -mwindows)
endif()