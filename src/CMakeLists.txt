project(Venus)

set(VENUS_SRC event.cpp setup.cpp)
add_library(math_lib vector.cpp)
add_library(file_lib file_util.cpp)
add_library(audio_lib audio.cpp)
add_library(graphics_lib graphics.cpp ui.cpp shader.cpp)
add_library(venus_lib ${VENUS_SRC})

target_include_directories(math_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(file_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(audio_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(graphics_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(venus_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)