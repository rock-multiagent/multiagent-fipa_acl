# Set Gem name to find name
# Set Gem version to find the correct version of gem

set(GEM_EXECUTABLE gem)

if(NOT Gem_NAME)
	MESSAGE(FATAL_ERROR "If searching for a Gem you have to set Gem_NAME first") 
endif()

# If no gem version has been specified use the highest version found
if(Gem_FIND_VERSION)

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

	set(Gem_INCLUDE_DIRS "$ENV{GEM_HOME}/gems/${Gem_NAME}-${GEM_VERSION}")
endif()

message(${Gem_INCLUDE_DIRS})




	
if(Gem_FIND_REQUIRED)
	MESSAGE(FATAL_ERROR "Gem ${Gem_NAME}" could not be found) 
endif()

