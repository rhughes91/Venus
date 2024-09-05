project(Venus)

set(VENDOR_SRC glad.c stb_image.cpp)
add_library(vendor ${VENDOR_SRC})

target_include_directories(vendor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)