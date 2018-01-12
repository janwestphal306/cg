# Define project
cmake_minimum_required(VERSION 3.2)
project(CG1Exercise4)

set(CMAKE_CXX_STANDARD 14)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set target name
set(target41 CG1Exercise41)
set(target42 CG1Exercise42)
set(target43 CG1Exercise43)

# Include QT5 libraries
set(CMAKE_AUTOMOC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)

find_package(Qt5Widgets 5.7 REQUIRED)

# Define source files
set(util_sources
    src/util/abstractexercise.cpp
    src/util/abstractexercise.h
    src/util/abstractpainter.h
    src/util/abstractpainter.cpp
    src/util/box.h
    src/util/box.cpp
    src/util/cachedvalue.h
    src/util/cachedvalue.hpp
    src/util/camera.h
    src/util/camera.cpp
    src/util/fileassociatedasset.h
    src/util/fileassociatedasset.cpp
    src/util/fileassociatedshader.h
    src/util/fileassociatedshader.cpp
    src/util/glviewer.h
    src/util/glviewer.cpp
    src/util/icosahedron.h
    src/util/icosahedron.cpp
    src/util/openglfunctions.h
    src/util/objio.h
    src/util/objio.cpp
    src/util/paintermode.h
    src/util/polygonaldrawable.h
    src/util/polygonaldrawable.cpp
    src/util/unitcube.cpp
    src/util/unitcube.h
    src/util/movingunitsphere.cpp
    src/util/movingunitsphere.h
)

set(target41_sources
    src/exercise41.cpp
    src/exercise41.h
)

set(target42_sources
    src/exercise42.cpp
)

set(target43_sources
    src/exercise43.cpp
    src/exercise43.h
)

# Create util library
add_library(util STATIC ${util_sources})

# Create executable
add_executable(${target41} ${target41_sources})
add_executable(${target42} ${target42_sources})
add_executable(${target43} ${target43_sources})

# Create executable
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# Use the Widgets and OpenGL module from Qt 5
target_link_libraries(util Qt5::Widgets)
target_link_libraries(${target41} util Qt5::Widgets)
target_link_libraries(${target42} util Qt5::Widgets)
target_link_libraries(${target43} util Qt5::Widgets)
