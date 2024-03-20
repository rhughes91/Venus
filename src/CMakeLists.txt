project(Venus)

set(VENUS_SRC event.cpp graphics.cpp setup.cpp shader.cpp vector.cpp)
add_library(ecs_lib structure.cpp)
add_library(file_lib file_util.cpp)
add_library(audio_lib audio.cpp)
add_library(venus_lib ${VENUS_SRC})
add_library(global_lib global.cpp)

target_include_directories(global_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(venus_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(ecs_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(file_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(audio_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)