project(Venus)


get_filename_component(FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/platformer.cpp)
add_library(${FILE_NAME}_lib ${PROJECT_SRC})

target_include_directories(${FILE_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${FILE_NAME})
add_executable(${FILE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# target_link_options(${FILE_NAME} PRIVATE -mwindows)
target_link_libraries(${FILE_NAME} PRIVATE ${FILE_NAME}_lib venus_lib vendor_lib ecs_lib file_lib ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a -static-libgcc -static-libstdc++)