cmake_minimum_required(VERSION 3.25.0)

set(PROJECT_DIRECTORY "${CMAKE_SOURCE_DIR}/projects" CACHE STRING "default project directory for source code and resources.")
set(DEFAULT_PROJECT "TestProject" CACHE STRING "default no-arg build project.")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Venus)

get_filename_component(DIR_NAME ${CMAKE_BINARY_DIR} NAME)
set(EXAMPLE (NOT EXISTS ${CMAKE_SOURCE_DIR}/${PROJECT_DIRECTORY}/${DIR_NAME}))
set(PROJECT_SRC "")

if(${EXAMPLE})
    set(DIR_NAME ${DEFAULT_PROJECT})
endif()

set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/vendor/GLFW/include ${CMAKE_SOURCE_DIR}/vendor/glad/include  ${CMAKE_SOURCE_DIR}/vendor/stb_image/include)
add_subdirectory(src)
add_subdirectory(vendor)
add_subdirectory(projects)

get_directory_property(PROJECT_SRC DIRECTORY projects DEFINITION PROJECT_SRC)

set(SOURCE_PATH "")
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/path.config)
    set(SOURCE_PATH ${PROJECT_DIRECTORY}/${DIR_NAME}/)

    file(COPY misc/default_resources/path_build.config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/path_build.config ${CMAKE_CURRENT_BINARY_DIR}/path.config)
    file(READ ${CMAKE_CURRENT_BINARY_DIR}/path.config FILE_CONTENTS)
    string(REPLACE "source: " "source: ${SOURCE_PATH}" FILE_CONTENTS "${FILE_CONTENTS}")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/path.config "${FILE_CONTENTS}")
endif()

set(ADJ_PROJECT_SRC "")
foreach(SRC ${PROJECT_SRC})
    list(APPEND ADJ_PROJECT_SRC "${PROJECT_DIRECTORY}/${DIR_NAME}/${SRC}")
endforeach()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable(${DIR_NAME} ${ADJ_PROJECT_SRC} "${PROJECT_DIRECTORY}/${DIR_NAME}/main.cpp")

set(PROJECT_LIBS venus graphics file audio math stb_image glad glfw)
set(LINK_ARGS "")
if(WIN32)
    if(MINGW)
        set(LINK_ARGS -static-libgcc -static-libstdc++ -static)
        # set(VENUS_ADLIB ${CMAKE_SOURCE_DIR}/lib/libglfw3.a)
    else()
        set(LINK_ARGS "")
        if(CMAKE_GENERATOR MATCHES "Visual Studio")
            set_property(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} PROPERTY VS_STARTUP_PROJECT ${DIR_NAME})
        endif()
        # set(VENUS_ADLIB ${CMAKE_SOURCE_DIR}/lib/glfw3_mt.lib ${CMAKE_SOURCE_DIR}/lib/glfw3.lib)
    endif()
elseif(UNIX)
    set(LINK_ARGS -static-libgcc -static-libstdc++)
    # set(VENUS_ADLIB ${CMAKE_SOURCE_DIR}/lib/libglfw3_linux.a)
endif()

# target_link_options(${DIR_NAME} PRIVATE -mwindows)

target_link_libraries(${DIR_NAME} PRIVATE ${PROJECT_LIBS} ${VENUS_ADLIB} ${LINK_ARGS})

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS ${DIR_NAME} DESTINATION ./${DIR_NAME})
install(DIRECTORY "${PROJECT_DIRECTORY}/${DIR_NAME}/resources" DESTINATION ./${DIR_NAME})
install(FILES "misc/default_resources/path.config" DESTINATION ./${DIR_NAME})