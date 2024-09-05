project(Venus)

if(${EXAMPLE})
    set(DIR_NAME ${DEFAULT_PROJECT})
endif()

set(PROJ_NAME ${CMAKE_CURRENT_SOURCE_DIR}/${DIR_NAME})

# Add CMakeLists.txt file
if(NOT EXISTS ${PROJ_NAME}/CMakeLists.txt)
    file(COPY ${CMAKE_SOURCE_DIR}/misc/default_resources/CMakeLists.txt DESTINATION ${PROJ_NAME})
endif()

# Add default main file
if(NOT EXISTS ${PROJ_NAME}/main.cpp)
    file(COPY ${CMAKE_SOURCE_DIR}/misc/default_resources/main.cpp DESTINATION ${PROJ_NAME})
endif()

# Add default resources
if(NOT EXISTS ${PROJ_NAME}/resources)
    file(MAKE_DIRECTORY ${full_path}/resources)
    if(NOT EXISTS ${PROJ_NAME}/resources/audio)
        file(MAKE_DIRECTORY ${PROJ_NAME}/resources/audio)
    endif()
    if(NOT EXISTS ${PROJ_NAME}/resources/images)
        file(MAKE_DIRECTORY ${PROJ_NAME}/resources/images)
    endif()
    if(NOT EXISTS ${PROJ_NAME}/resources/model)
        file(MAKE_DIRECTORY ${PROJ_NAME}/resources/models)
    endif()
    if(NOT EXISTS ${PROJ_NAME}/resources/data)
        file(COPY ${CMAKE_SOURCE_DIR}/misc/default_resources/data DESTINATION ${PROJ_NAME}/resources)
    endif()
    if(NOT EXISTS ${PROJ_NAME}/resources/fonts)
        file(COPY ${CMAKE_SOURCE_DIR}/misc/default_resources/fonts DESTINATION ${PROJ_NAME}/resources)
    endif()
    if(NOT EXISTS ${PROJ_NAME}/resources/shaders)
        file(COPY ${CMAKE_SOURCE_DIR}/misc/default_resources/shaders DESTINATION ${PROJ_NAME}/resources)
    endif()
endif()

add_subdirectory(${DIR_NAME})
get_directory_property(PROJECT_SRC DIRECTORY ${DIR_NAME} DEFINITION PROJECT_SRC)
get_directory_property(FILE_NAME DIRECTORY ${DIR_NAME} DEFINITION FILE_NAME)