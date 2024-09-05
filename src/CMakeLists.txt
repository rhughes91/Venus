project(Venus)

set(VENUS_SRC event.cpp application.cpp)
add_library(math vector.cpp)
add_library(file file_util.cpp)
add_library(audio audio.cpp)
add_library(graphics graphics.cpp ui.cpp shader.cpp)
add_library(venus ${VENUS_SRC})

target_include_directories(math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(file PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(venus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)