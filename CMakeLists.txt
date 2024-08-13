cmake_minimum_required(VERSION 3.16)
project(Browser LANGUAGES CXX)



find_package(Qt6 REQUIRED COMPONENTS Core Gui WebEngineWidgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(BROWSER_SOURCE_DIRECTORY ${CMAKE_SOURCE_DIR}/code)
file(GLOB_RECURSE UI_FILES       code/*.ui)
file(GLOB_RECURSE HEADER_FILES   code/*.h)
file(GLOB_RECURSE SRC_FILES      code/*.cpp)
file(GLOB_RECURSE RESOURCE_FILES code/*.qrc)

# filter the files in visual studio as they are stored in the folder.
if (WIN32)
    # filter all Header Files
    foreach(_source IN ITEMS ${HEADER_FILES})
        get_filename_component(_source_path "${_source}" PATH)
        file(RELATIVE_PATH _source_path_rel "${BROWSER_SOURCE_DIRECTORY}" "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
        #message("create filter: ${_group_path} with files: ${_source}")
        source_group("Header Files\\${_group_path}" FILES "${_source}")
    endforeach()

    # filter all Source Files
    foreach(_source IN ITEMS ${SRC_FILES})
        get_filename_component(_source_path "${_source}" PATH)
        file(RELATIVE_PATH _source_path_rel "${BROWSER_SOURCE_DIRECTORY}" "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_source_path_rel}")

        # main.cpp should be in the root folder
        if(${_source} MATCHES "main.cpp")
            source_group("" FILES "${_source}")
        else()
            source_group("Source Files\\${_group_path}" FILES "${_source}")
        endif()
    endforeach()

    # Group UI and QRC Files
    source_group("UI Files" FILES "${UI_FILES}")
    source_group("QRC Files" FILES "${RESOURCE_FILES}")
endif()

add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES} ${HEADER_FILES} ${UI_FILES} ${RESOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Gui
    Qt::WebEngineWidgets
)
