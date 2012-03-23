# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)
include($ENV{ROCK_CMAKE_MACROS}/Rock.cmake)
rock_init(fipa_acl 1.2)
set(PROJECT_DESCRIPTION "Library for FIPA Agent Communication Language and handling bit-efficient FIPA Messages")
set(CMAKE_BUILD_TYPE Release)

find_package(Boost REQUIRED filesystem unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

rock_standard_layout()

add_subdirectory(bindings/ruby)

#INCLUDE(RubyExtensions)
#IF(RUBY_EXTENSIONS_AVAILABLE)
#    ENABLE_TESTING()
#    ADD_SUBDIRECTORY(bindings/ruby)
#    ADD_SUBDIRECTORY(test/ruby)
#    if (DOXYGEN_FOUND)
#      ADD_DEPENDENCIES(doc ruby_doc)
#    endif (DOXYGEN_FOUND)
#ENDIF(RUBY_EXTENSIONS_AVAILABLE)
#

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)



