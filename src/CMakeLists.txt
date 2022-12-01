project(Venus)

set(VENUS_SRC event.cpp global.cpp graphics.cpp setup.cpp shader.cpp vector.cpp)
add_library(venus_lib ${VENUS_SRC})
add_library(file_lib file_util.cpp)
add_library(ecs_lib structure.cpp)

target_include_directories(venus_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(file_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(ecs_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)