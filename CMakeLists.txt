# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)
include($ENV{ROCK_CMAKE_MACROS}/Rock.cmake)

project(conversation-monitor)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "Control a fipa agent converstation based on an interaction protocol definition")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(CommonSetup)

add_definitions(-DBASE_LOG_NAMESPACE="${PROJECT_NAME}")

# Include headers within source
# If you create subdirectories within source include headers with subdirectory, i.e. 
# #include "subdirectory/myheader.h"
include_directories(${PROJECT_SOURCE_DIR}/external/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

include(FindPkgConfig RESULT_VARIABLE PKG_CONFIG_FOUND)

pkg_check_modules(FipaAcl REQUIRED "fipa_acl")
include_directories(${FipaAcl_INCLUDE_DIRS})
link_directories(${FipaAcl_LIBRARY_DIRS})

pkg_check_modules(TinyXML REQUIRED "tinyxml")
include_directories(${TinyXML_INCLUDE_DIRS})
link_directories(${TinyXML_LIBRARY_DIRS})

pkg_check_modules(Base REQUIRED "base-lib")
include_directories(${Base_INCLUDE_DIRS})
link_directories(${Base_LIBRARY_DIRS})

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)

### Uncomment if you want to use Boost. Use the first find_package stanza to
### search for headers only, and the second one if you need binary libraries
#set(Boost_ADDITIONAL_VERSION 1.40 1.41)
find_package(Boost REQUIRED COMPONENTS filesystem unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

### Update external libraries that are available
set(EXTERNAL_LIBS ${Boost_LIBRARIES} ${TinyXML_LIBRARIES} ${Base_LIBRARIES} ${FipaAcl_LIBRARIES})

# Process CMakeLists.txt in the following subdirectory
add_subdirectory(src)
add_subdirectory(test/unit-testing)


