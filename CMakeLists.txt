# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)

project(message-parser)
set(PROJECT_VERSION 1.1)
set(PROJECT_DESCRIPTION "Library for parsing FIPA Messages")
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(CommonSetup)
include(RPATHHandling)

add_definitions(-Wall)
CMAKE_USE_FULL_RPATH("${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib/fipa-message-parser")


# Include headers within source
# If you create subdirectories within source include headers with subdirectory, i.e. 
# #include "subdirectory/myheader.h"
include_directories(${PROJECT_SOURCE_DIR}/external/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

##### End specification of build directory ##############################

include(FindPkgConfig RESULT_VARIABLE PKG_CONFIG_FOUND)
pkg_check_modules(MessageGenerator REQUIRED "message-generator")

set(Boost_LOCALFIRST TRUE)
set(Boost_ADDITIONAL_VERSION 1.42 1.43)
find_package(Boost 1.42 REQUIRED)
set(EXTERNAL_LIBS ${Boost_LIBRARIES} )

include_directories(${MessageGenerator_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_directories(${MessageGenerator_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

# Process CMakeLists.txt in the following subdirectory
add_subdirectory(src)

INCLUDE(RubyExtensions)
IF(RUBY_EXTENSIONS_AVAILABLE)
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(bindings/ruby)
    ADD_SUBDIRECTORY(test/ruby)
    if (DOXYGEN_FOUND)
      ADD_DEPENDENCIES(doc ruby_doc)
    endif (DOXYGEN_FOUND)
ENDIF(RUBY_EXTENSIONS_AVAILABLE)

if (Boost_UNIT_TEST_FRAMEWORK_FOUND)
    message(STATUS "boost/test found ... building test suite")
    add_subdirectory(test/unit-testing)
endif(Boost_UNIT_TEST_FRAMEWORK_FOUND)

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)



