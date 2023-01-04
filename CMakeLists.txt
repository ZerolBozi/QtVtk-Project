###########
# Project #
###########
cmake_minimum_required(VERSION 3.4)
project(QtVtk)

#######################
# Folder organisation #
#######################
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Indicate that previous configuration exists
set(CONFIGURED TRUE)

###########
# Targets #
###########
add_subdirectory(src)
