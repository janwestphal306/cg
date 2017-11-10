# Define project
cmake_minimum_required(VERSION 3.2)
project(CG1Exercise1)

set(CMAKE_CXX_STANDARD 14)

# Widgets finds its own dependencies.
find_package(Qt5Widgets 5.7 REQUIRED)

# Define source files
set(sources
	crc.cpp
	crc.h
	main.cpp
	miniz.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${sources})

# Use the Widgets module from Qt 5
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
