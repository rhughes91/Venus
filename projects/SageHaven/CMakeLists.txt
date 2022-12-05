project(Venus)

get_filename_component(PARENT_DIR ${CMAKE_BINARY_DIR} DIRECTORY)
get_filename_component(PARENT_DIR_NAME ${PARENT_DIR} NAME)

get_filename_component(FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if(${FILE_NAME} STREQUAL ${PARENT_DIR_NAME})
    set(PROJECT_SRC platformer.cpp)
    add_library(${FILE_NAME}_lib ${PROJECT_SRC})

    target_include_directories(${FILE_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include)

    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${FILE_NAME})
    add_executable(${FILE_NAME} main.cpp)

    # target_link_options(${FILE_NAME} PRIVATE -mwindows)
    target_link_libraries(${FILE_NAME} PRIVATE ${FILE_NAME}_lib venus_lib vendor_lib ecs_lib file_lib ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a -static-libgcc -static-libstdc++)
endif()