project(Venus)

add_subdirectory(GLFW)

add_library(glad glad/glad.c)
add_library(stb_image stb_image/stb_image.cpp)

target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)
target_include_directories(stb_image PUBLIC ${CMAKE_SOURCE_DIR}/vendor/stb_image/include)