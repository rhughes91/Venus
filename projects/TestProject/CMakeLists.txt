project(Venus)

# additional '.cpp' files can be placed in the variable below
set(PROJECT_SRC "project.cpp")

set(EMPTY_PROJ FALSE)
if(PROJECT_SRC STREQUAL "")
    set(EMPTY_PROJ TRUE)
endif()

get_filename_component(DIR_NAME ${CMAKE_BINARY_DIR} NAME)
get_filename_component(FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if(${FILE_NAME} STREQUAL ${DIR_NAME} OR ${DIR_NAME} STREQUAL "build")
    if(${DIR_NAME} STREQUAL "build")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
    else()
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${FILE_NAME})
    endif()
    
    add_executable(${FILE_NAME} ${PROJECT_SRC} main.cpp)

    if(WIN32)
        if(MSVC)
            if(CMAKE_GENERATOR MATCHES "Visual Studio")
                set_property(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} PROPERTY VS_STARTUP_PROJECT ${FILE_NAME})
            endif()
            set(VENUS_ADLIB ${CMAKE_SOURCE_DIR}/lib/glfw3_mt.lib ${CMAKE_SOURCE_DIR}/lib/glfw3.lib)
        else()
            set(VENUS_ADLIB ${CMAKE_SOURCE_DIR}/lib/libglfw3.a)
        endif()
    elseif(UNIX)
    	set(VENUS_ADLIB ${CMAKE_SOURCE_DIR}/lib/libglfw3_linux.a)
    endif()

    # This command will suppress the default Windows console; nothing can be printed to the console, and no console will show up when the built '.exe' file is run.
    # target_link_options(${FILE_NAME} PRIVATE -mwindows)

    if(MSVC)
        target_link_libraries(${FILE_NAME} PRIVATE venus_lib graphics_lib vendor_lib file_lib audio_lib math_lib ${VENUS_ADLIB})
    elseif(WIN32)
        target_link_libraries(${FILE_NAME} PRIVATE venus_lib graphics_lib vendor_lib file_lib audio_lib math_lib ${VENUS_ADLIB} -static-libgcc -static-libstdc++ -static)
    elseif(UNIX)
    	target_link_libraries(${FILE_NAME} PRIVATE venus_lib graphics_lib vendor_lib file_lib audio_lib math_lib ${VENUS_ADLIB} -static-libgcc -static-libstdc++)
    endif()
endif()