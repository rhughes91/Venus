project(Venus)

set(VENUS_SRC event.cpp application.cpp)
add_library(math vector.cpp)
add_library(file file_util.cpp)
add_library(audio audio.cpp)
add_library(graphics graphics.cpp ui.cpp shader.cpp)
add_library(venus ${VENUS_SRC})

target_include_directories(math PUBLIC ${INCLUDE_DIRS})
target_include_directories(file PUBLIC ${INCLUDE_DIRS})
target_include_directories(audio PUBLIC ${INCLUDE_DIRS})
target_include_directories(graphics PUBLIC ${INCLUDE_DIRS})
target_include_directories(venus PUBLIC ${INCLUDE_DIRS})