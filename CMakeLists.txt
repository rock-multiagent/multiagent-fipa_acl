# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)

project(fipa_acl)
set(PROJECT_VERSION 1.2)
set(PROJECT_DESCRIPTION "Library for FIPA Agent Communication Language and handling bit-efficient FIPA Messages")
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" $ENV{ROCK_CMAKE_MACROS})
include(CommonSetup)
include(RPATHHandling)

add_definitions(-Wall -DBASE_LOG_NAMESPACE=${PROJECT_NAME})
CMAKE_USE_FULL_RPATH("${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}")


# Include headers within source
# If you create subdirectories within source include headers with subdirectory, i.e. 
# #include "subdirectory/myheader.h"
include_directories(${PROJECT_SOURCE_DIR}/external/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

include(FindPkgConfig)
pkg_check_modules(Base REQUIRED "base-lib")
include_directories(${Base_INCLUDE_DIRS})
link_directories(${Base_LIBRARY_DIRS})
set(LIBRARIES ${LIBRARIES} ${Base_LIBRARIES} )

pkg_check_modules(TinyXML REQUIRED "tinyxml")
include_directories(${TinyXML_INCLUDE_DIRS})
link_directories(${TinyXML_LIBRARY_DIRS})
set(LIBRARIES ${LIBRARIES} ${TinyXML_LIBRARIES} )

##### End specification of build directory ##############################
set(Boost_LOCALFIRST TRUE)
set(Boost_ADDITIONAL_VERSION 1.42 1.43)
find_package(Boost 1.42 REQUIRED filesystem unit_test_framework)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
set(LIBRARIES ${LIBRARIES} ${Boost_LIBRARIES} )

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
    add_subdirectory(test/unit-testing/conversation_monitor)
    add_subdirectory(test/unit-testing/messages)
endif(Boost_UNIT_TEST_FRAMEWORK_FOUND)

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)



