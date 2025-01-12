cmake_minimum_required(VERSION 3.16)

set(VCPKG_FEATURE_FLAGS "versions")

# Add folder where are supportive functions
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include(DeepsEngineSetup)

# Basic information about project
set(DEEPS_ENGINE_VERSION "2022.0.1")
project(DeepsEngine VERSION ${DEEPS_ENGINE_VERSION})

# find the libraries for running deeps engine
DEEPS_ENGINE_SETUP()

# get include directories for libraries used
include_directories(${PROJECT_SOURCE_DIR}/src/engine/include)

# source files for engine code
FILE(GLOB ENGINE_SOURCE_FILES
        src/engine/renderer/glad.c
        src/engine/*.cpp
        src/engine/component/*.cpp
        src/engine/renderer/*.cpp
        src/engine/renderer/primitive/*.cpp
        src/engine/window/*.cpp
        src/engine/scene/*.cpp
        src/engine/util/*.cpp
        src/engine/audio/*.cpp)

message(STATUS "Engine source files: ${ENGINE_SOURCE_FILES}")

if(WITH_EDITOR)
    if (APPLE)
        FIND_LIBRARY(CARBON_LIBRARY Carbon)
        set(EXTRA_LIBS ${CARBON_LIBRARY})
    endif()

    find_package(Qt5 COMPONENTS Widgets Concurrent OpenGL CONFIG REQUIRED)

    include_directories(${PROJECT_SOURCE_DIR}/src/editor/include)

    include(QtCommon)
    fix_project_version()

    FILE(GLOB EDITOR_SOURCE_FILES
            src/editor/*.cpp
            src/editor/widget/*.cpp
            src/editor/widget/common/*.cpp
            src/editor/widget/component/*.cpp
            src/editor/include/*.h)

    if(APPLE)
        FILE(GLOB MACOS_SOURCE_FILES
                src/editor/changetitlebarcolor.mm)
    endif()

    add_project_meta(META_FILES_TO_INCLUDE)

    set(RESOURCE_FILES src/editor/example.qrc)

    source_group("UI Files" FILES ${UI_FILES})

    add_executable(${PROJECT_NAME} ${OS_BUNDLE} src/main.cpp ${MACOS_SOURCE_FILES} ${ENGINE_SOURCE_FILES} ${EDITOR_SOURCE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES})

    target_precompile_headers(${PROJECT_NAME} INTERFACE QtWidgets.h)

    target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::OpenGL yaml-cpp)

    if (APPLE)
        message(STATUS CARBON_LIBRARY=${CARBON_LIBRARY})
        target_link_libraries(${PROJECT_NAME} ${CARBON_LIBRARY})
    endif()

    add_custom_command(
            TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/res
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/MacOS/assets/res)
else()
    if (AS_LIBRARY)
        # create shared library
        add_library(${PROJECT_NAME} SHARED ${ENGINE_SOURCE_FILES})
        set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/src/engine/lib)

        # copy build files to web build folder
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/engine/include ${PROJECT_SOURCE_DIR}/res/example-project/sys/DeepsEngine/include)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/engine/external ${PROJECT_SOURCE_DIR}/res/example-project/sys/DeepsEngine/external)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/engine/lib ${PROJECT_SOURCE_DIR}/res/example-project/sys/DeepsEngine/lib)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/cmake ${PROJECT_SOURCE_DIR}/res/example-project/sys/DeepsEngine/cmake)
    else()
        # create executable application
        add_executable(${PROJECT_NAME} src/main.cpp ${ENGINE_SOURCE_FILES})
    endif()

    if(NOT EMSCRIPTEN)
        # standalone build
        target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
    endif()

    DEEPS_ENGINE_LINK_LIBRARIES_TO_TARGET()
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE DEEPS_ENGINE_VERSION=${DEEPS_ENGINE_VERSION})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)