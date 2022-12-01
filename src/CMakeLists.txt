project(Venus)

set(VENUS_SRC component.cpp event.cpp file_util.cpp global.cpp graphics.cpp setup.cpp shader.cpp structure.cpp vector.cpp)
add_library(venus_lib ${VENUS_SRC})

target_include_directories(venus_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)