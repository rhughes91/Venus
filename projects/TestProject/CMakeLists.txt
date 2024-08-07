project(Venus)


# additional '.cpp' files can be placed in the variable below
set(PROJECT_SRC "project.cpp")

set(EMPTY_PROJ FALSE)
if(PROJECT_SRC STREQUAL "")
    set(EMPTY_PROJ TRUE)
endif()

get_filename_component(PARENT_DIR ${CMAKE_BINARY_DIR} DIRECTORY)
get_filename_component(PARENT_DIR_NAME ${PARENT_DIR} NAME)
get_filename_component(FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if(${FILE_NAME} STREQUAL ${PARENT_DIR_NAME})
    
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${FILE_NAME})
    add_executable(${FILE_NAME} ${PROJECT_SRC} main.cpp)

    set(VENUS_ADLIB ${CMAKE_SOURCE_DIR}/lib/libOpenAL32.a ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a)

    # This command will suppress the default Windows console; nothing can be printed to the console, and no console will show up when the built '.exe' file is run.
    # target_link_options(${FILE_NAME} PRIVATE -mwindows)

    target_link_libraries(${FILE_NAME} PRIVATE venus_lib graphics_lib vendor_lib file_lib audio_lib math_lib ecs_lib ${VENUS_ADLIB} -static-libgcc -static-libstdc++ -static)
endif()