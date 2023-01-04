###########
# Project #
###########
set(EXENAME QtVtk)

if(NOT CONFIGURED)
        cmake_minimum_required(VERSION 3.4)
        project(${EXENAME})
        set(PROJECT_SOURCE_DIR ../)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
        link_directories(${PROJECT_SOURCE_DIR}/build)
        # Indicate that previous configuration exists
        set(CONFIGURED TRUE)
endif(NOT CONFIGURED)

if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif()


#########################
# External Dependencies #
#########################

# Find the Qt libraries
set(CMAKE_PREFIX_PATH $ENV{QTDIR})

find_package(Qt5Quick REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# VTK Libraries
set(VTK_DIR $ENV{VTK_DIR})
find_package(VTK REQUIRED NO_MODULE)
include(${VTK_USE_FILE})

# Headers directories
include_directories(${PROJECT_SOURCE_DIR}
	                ${PROJECT_SOURCE_DIR}/src
)

###########
# Targets #
###########

# Add QML files to project tree in Qt Creator
file(GLOB QML_DESKTOP "../resources/*.qml")
add_custom_target(QML_files SOURCES ${QML_DESKTOP})

# Sources
set (SOURCES
	main.cpp
	CanvasHandler.cpp
    	CommandModel.cpp
    	CommandModelAdd.cpp
	CommandModelActor.cpp
	CommandModelSave.cpp
    	CommandModelTranslate.cpp
    	Model.cpp
	ProcessingEngine.cpp
    	QVTKFramebufferObjectItem.cpp
    	QVTKFramebufferObjectRenderer.cpp
)

if (NOT APPLE)
	add_definitions(-std=c++11 -DPTHREADS_USED)
else()
	add_definitions(-std=c++11)
endif()

# Qt Resources
qt5_add_resources(RESOURCES qml.qrc)

# Build executable
# Control the Terminal, WIN32 == NO Terminal
add_executable(${EXENAME} ${HEADERS} ${SOURCES} ${RESOURCES})
#add_executable(${EXENAME} WIN32 ${HEADERS} ${SOURCES} ${RESOURCES})

# Link to libraries
target_link_libraries(${EXENAME} Qt5::Quick Qt5::Widgets Qt5::Qml Qt5::QuickControls2 ${VTK_LIBRARIES} ${SYSTEM_LIBS})



