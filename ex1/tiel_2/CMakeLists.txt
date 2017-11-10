# Define project
cmake_minimum_required(VERSION 3.2)
project(CG1Exercise2)

set(CMAKE_CXX_STANDARD 14)

if(POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0043)
	cmake_policy(SET CMP0043 NEW)
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# Tell CMake to run uic when necessary:
set(CMAKE_AUTOUIC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
find_package(Qt5Widgets 5.7 REQUIRED)

# Define source files
set(sources
	imageeffects.cpp
	imageeffects.h
	imageviewer.cpp
	imageviewer.h
	imageviewer.ui
	main.cpp
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

# Create executable
add_executable(${PROJECT_NAME} ${sources})

# Use the Widgets module from Qt 5
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
