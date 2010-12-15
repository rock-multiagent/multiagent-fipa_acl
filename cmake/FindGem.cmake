# Author thomas.roehr@dfki.de
#
# Version 0.1 2010-12-15
# 	- support basic search functionality 
#       - tested to find rice
#
# INPUT: 
# Gem_NAME to the gem you want to find
#
# OUTPUT:
#
# GEM_INCLUDE_DIRS	After successful search contains the include directory
#
# GEM_LIBRARIES		After successful search contains the full path to the library
# 
#
# Usage: 
# set(Gem_DEBUG TRUE)
# set(Gem_NAME "rice")
# find_package(Gem)
# include_directories(${GEM_INCLUDE_DIRS})
# target_link_libraries(${GEM_LIBRARIES}
#

# TBD: check for gem first !?
set(GEM_EXECUTABLE gem)

if(NOT Gem_NAME)
	MESSAGE(FATAL_ERROR "If searching for a Gem you have to set Gem_NAME first") 
endif()

# If no gem version has been specified use the highest version found
if(Gem_FIND_VERSION)
 # TBD
endif(Gem_FIND_VERSION)

set(GEM_HOME ENV{GEM_HOME})

EXECUTE_PROCESS(COMMAND ${GEM_EXECUTABLE} list --local OUTPUT_VARIABLE GEMS_LOCAL_INFO)

if("${GEMS_LOCAL_INFO}" STREQUAL "")
	MESSAGE(FATAL_ERROR "No local gem found. Check your GEM_HOME setting!") 
else()
	STRING(REGEX MATCH "${Gem_NAME} \\(([0-9.]+)" GEM_INFO "${GEMS_LOCAL_INFO}")
	STRING(REGEX REPLACE ".* \\(([0-9.]+).*" "\\1" GEM_VERSION "${GEM_INFO}")
	MESSAGE("Found gem: ${Gem_NAME} version: ${GEM_VERSION}")
endif()

if("${GEM_VERSION}" STREQUAL "")
	MESSAGE(FATAL_ERROR "No local gem found. Check your GEM_HOME setting!") 
else()
	if("$ENV{GEM_HOME}" STREQUAL "")
		MESSAGE(FATAL_ERROR "GEM_HOME. Check your GEM_HOME setting!") 
	endif()

	set(GEM_INCLUDE_DIRS "$ENV{GEM_HOME}/gems/${Gem_NAME}-${GEM_VERSION}")

	# Our heuristic to libary names available for linking
	# since there is no real standard for where to put the 
	# library
	set(_library_NAMES lib${Gem_NAME}.a
			   lib${Gem_NAME}.so
			   ${Gem_NAME}.a
			   ${Gem_NAME}.so
	)

	set(_library_SEARCH_DIRS
			${GEM_INCLUDE_DIRS}
			${GEM_INCLUDE_DIRS}/lib
			${GEM_INCLUDE_DIRS}/${Gem_NAME}
	)
	
	# Search for an existing library, but only within the gems folder
	foreach(_library_NAME ${_library_NAMES})
		foreach(_library_SEARCH_DIR ${_library_SEARCH_DIRS})
		
			
			find_file(GEM_LIBRARY ${_library_NAME}
					PATHS ${_library_SEARCH_DIR}
					NO_DEFAULT_PATH
			)

			if(Gem_DEBUG)
				message(STATUS "Searching ${_library_NAME} in ${_library_SEARCH_DIR}")	
				message(STATUS " >> ${GEM_LIBRARY}")
			endif()	
			
			if(NOT "${GEM_LIBRARY}" STREQUAL "GEM_LIBRARY-NOTFOUND")
				# Use the first library found
				if("${GEM_LIBRARIES}" STREQUAL "")
					set(GEM_LIBRARIES ${GEM_LIBRARY})
				endif()
			endif()
		endforeach()
	endforeach()
endif()

if(Gem_DEBUG)
	message(STATUS "${Gem_NAME} library dir: ${GEM_LIBRARIES}")
	message(STATUS "${Gem_NAME} include dir: ${GEM_INCLUDE_DIRS}")
endif()
	
if(Gem_FIND_REQUIRED)
	MESSAGE(FATAL_ERROR "Gem: ${Gem_NAME} could not be found")
endif()

