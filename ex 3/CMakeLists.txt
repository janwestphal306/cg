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
set(target31 CG1Exercise31)
set(target32 CG1Exercise32)
set(target33 CG1Exercise33)
set(target34 CG1Exercise34)

# Include QT5 libraries
set(CMAKE_AUTOMOC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)

find_package(Qt5Widgets 5.7 REQUIRED)

# Define source files
set(util_sources
    src/util/objio.cpp
    src/util/objio.h
    src/util/polygonaldrawable.cpp
    src/util/polygonaldrawable.h
    src/util/abstractexercise.cpp
    src/util/abstractexercise.h
    src/util/abstractpainter.h
    src/util/abstractpainter.cpp
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
    src/util/icosahedron.cpp
    src/util/icosahedron.h
    src/util/openglfunctions.h
    src/util/paintermode.h
    src/util/unitcube.cpp
    src/util/unitcube.h
)

set(target31_sources
    src/exercise31.cpp
)

set(target32_sources
    src/exercise32.cpp
)

set(target33_sources
    src/exercise33.cpp
)

set(target34_sources
    src/exercise34.cpp
)

# Create util library
add_library(util STATIC ${util_sources})

# Create executable
add_executable(${target31} ${target31_sources})
add_executable(${target32} ${target32_sources})
add_executable(${target33} ${target33_sources})
add_executable(${target34} ${target34_sources})

# Create executable
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# Use the Widgets and OpenGL module from Qt 5
target_link_libraries(util Qt5::Widgets)
target_link_libraries(${target31} util Qt5::Widgets)
target_link_libraries(${target32} util Qt5::Widgets)
target_link_libraries(${target33} util Qt5::Widgets)
target_link_libraries(${target34} util Qt5::Widgets)
