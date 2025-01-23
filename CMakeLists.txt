cmake_minimum_required(VERSION 3.16)
project(barcode_tracker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Release build type by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Windows specific settings
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

# Print Qt version information for debugging
if(NOT DEFINED Qt6_DIR)
    message(STATUS "Qt6_DIR not defined")
endif()
message(STATUS "Looking for Qt6...")

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
message(STATUS "Found Qt6 version: ${Qt6_VERSION}")

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/product.cpp
    src/logger.cpp
)

# Header files
set(HEADERS
    include/mainwindow.h
    include/product.h
    include/logger.h
)

# Resource files
set(RESOURCES
    resources/resources.qrc
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(barcode_tracker
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_link_libraries(barcode_tracker PRIVATE
    Qt6::Widgets
    Qt6::Core
)

# Copy resource files to build directory
add_custom_command(TARGET barcode_tracker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_BINARY_DIR}/resources
)

# Installation settings
install(TARGETS barcode_tracker
    RUNTIME DESTINATION bin
)

install(DIRECTORY resources
    DESTINATION bin
)

# Windows deployment
if(WIN32)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET barcode_tracker POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:barcode_tracker> "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/resources" "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/resources"
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations --no-system-d3d-compiler --no-opengl-sw "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/$<TARGET_FILE_NAME:barcode_tracker>"
    )

    # Create ZIP archive
    add_custom_command(TARGET barcode_tracker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_CURRENT_BINARY_DIR}/BarkodTakip.zip" --format=zip "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
    )
endif() 