# Define project
cmake_minimum_required(VERSION 3.2)
project(CG1Exercise5)

set(CMAKE_CXX_STANDARD 14)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "CMakeAutomocTargets")

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set target name
set(target51 CG1Exercise51)
set(target52 CG1Exercise52)
set(target53 CG1Exercise53)

find_package(OpenGL REQUIRED)

# Include QT5 libraries
find_package(Qt5Gui 5.7 REQUIRED)

# Define source files
set(util_sources
	src/util/box.cpp
	src/util/box.h
    src/util/objio.cpp
    src/util/objio.h
    src/util/polygonaldrawable.cpp
    src/util/polygonaldrawable.h
    src/util/abstractexercise.cpp
    src/util/abstractexercise.h
    src/util/abstractpainter.h
    src/util/abstractpainter.cpp
    src/util/camera.h
    src/util/camera.cpp
    src/util/glviewer.h
    src/util/glviewer.cpp
    src/util/icosahedron.cpp
    src/util/icosahedron.h
    src/util/openglfunctions.h
    src/util/paintermode.h
    src/util/unitcube.cpp
    src/util/unitcube.h
)

set(target51_sources
    src/exercise51.cpp
	src/exercise51.h
)

set(target52_sources
	data/model.frag
	data/model.geom
	data/model.vert
	src/exercise52.cpp
	src/exercise52.h
)

set(target53_sources
	data/cube.frag
	data/cube.tcs
	data/cube.tes
	data/cube.geom
	data/cube.vert
    src/exercise53.cpp
	src/exercise53.h
)

# Create util library
add_library(util STATIC ${util_sources})

# Create executable
add_executable(${target51} ${target51_sources})
add_executable(${target52} ${target52_sources})
add_executable(${target53} ${target53_sources})

# Create executable
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# link libraries
target_link_libraries(util Qt5::Gui)
target_link_libraries(${target51} util Qt5::Gui ${OPENGL_LIBRARIES})
target_link_libraries(${target52} util Qt5::Gui ${OPENGL_LIBRARIES})
target_link_libraries(${target53} util Qt5::Gui ${OPENGL_LIBRARIES})
#${OPENGL_LIBRARIES}
