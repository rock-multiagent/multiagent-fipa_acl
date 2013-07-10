# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)
find_package(Rock)
rock_init(fipa_acl 1.3)
set(PROJECT_DESCRIPTION "Library for FIPA Agent Communication Language and handling bit-efficient FIPA Messages")
add_definitions(-g0)
add_definitions(-DTINYXML_USE_STD=1)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(Boost REQUIRED filesystem unit_test_framework regex)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

rock_standard_layout()

add_subdirectory(bindings/ruby)

