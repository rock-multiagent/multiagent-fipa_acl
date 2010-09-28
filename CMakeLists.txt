# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)

project(message-parser)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "Library for parsing FIPA Messages")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(CommonSetup)

# Include headers within source
# If you create subdirectories within source include headers with subdirectory, i.e. 
# #include "subdirectory/myheader.h"
include_directories(${PROJECT_SOURCE_DIR}/external/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

##### End specification of build directory ##############################

include(FindPkgConfig RESULT_VARIABLE PKG_CONFIG_FOUND)
pkg_check_modules(MessageGenerator "message-generator")

set(Boost_LOCALFIRST TRUE)
set(Boost_ADDITIONAL_VERSION 1.42 1.43)
find_package(Boost 1.42 REQUIRED)

include_directories(${MessageGenerator_INCLUDE_DIRS})
link_directories(${MessageGenerator_LIBRARY_DIRS})

# Process CMakeLists.txt in the following subdirectory
add_subdirectory(src)
#add_subdirectory(test/unit-testing)
# Disabled to avoid automatic installation of cpp-test 
# TODO: change to boost-testing
#add_subdirectory(test/unit-testing)

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)

set(EXTERNAL_LIBS ${Boost_LIBRARIES} )


