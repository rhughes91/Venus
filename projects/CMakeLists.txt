project(Venus)

FILE(GLOB children ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
SET(dirlist "")
FOREACH(child ${children})
    IF(IS_DIRECTORY ${child} AND NOT ${child} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} AND EXISTS ${child}/CMakeLists.txt)
        LIST(APPEND dirlist ${child})
    ENDIF()
ENDFOREACH()

FOREACH(subdir ${dirlist})
    ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()