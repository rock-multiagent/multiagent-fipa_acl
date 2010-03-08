# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

# ${PROJECT_SOURCE_DIR} refers to the folder of the CMakeLists.txt (project)
# ${PROJECT_BINARY_DIR} refers to the folder from which cmake was executed (project/build).


# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
if(DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
Release RelWithDebInfo MinSizeRel.")
else()
   ##### Build types #################################################
   # single-configuration generator like Makefile generator creates following variables per default
   #
   # None (CMAKE_C_FLAGS or CMAKE_CXX_FLAGS used)
   # Debug (CMAKE_C_FLAGS_DEBUG or CMAKE_CXX_FLAGS_DEBUG)
   # Release (CMAKE_C_FLAGS_RELEASE or CMAKE_CXX_FLAGS_RELEASE)
   # RelWithDebInfo (CMAKE_C_FLAGS_RELWITHDEBINFO or CMAKE_CXX_FLAGS_RELWITHDEBINFO
   # MinSizeRel (CMAKE_C_FLAGS_MINSIZEREL or CMAKE_CXX_FLAGS_MINSIZEREL) 
   ####################################################################
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build,
options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
RelWithDebInfo MinSizeRel.")
endif()

message("Build type set to: " ${CMAKE_BUILD_TYPE})

cmake_minimum_required(VERSION 2.6)

##### Set Project name and version #####################################
project(message-parser)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "Library for parsing FIPA Messages")
##### End Set Project name and version #################################

##### Specification of build directory #################################
# Specifies a common place where CMake should put all the executables.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# Specifies a common place where CMake should put all the libraries
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
# Include headers within source
# If you create subdirectories within source include headers with subdirectory, i.e. 
# #include "subdirectory/myheader.h"
include_directories(${PROJECT_SOURCE_DIR}/src)
##### End specification of build directory ##############################


# Process CMakeLists.txt in the following subdirectory
add_subdirectory(src)

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)


##### Select required libraries and desired versions######################
set(PROJECT_USES_QT FALSE)
set(DESIRED_QT_VERSION 4.5)
set(PROJECT_USES_BOOST TRUE)
# Set your boost version here, otherwise the installed version might not be found
set( Boost_ADDITIONAL_VERSIONS "1.42.0")
##### End Select required libaries #######################################

##### Add Qt support #####################################################
if(PROJECT_USES_QT)
	# FINDING QT
	# Note: entries are case sensitive, i.e. use 'Qt4' and not 'QT4'
	# If you have a local installation of a qt version, add it to the search path
	set(QT_SEARCH_PATH /opt)
	message(Desired Qt version is: ${DESIRED_QT_VERSION})
	# Try to find Qt4 libraries and set all required variables
	find_package(Qt4 REQUIRED)

	if(QT_FOUND)
		message("Prerequisite ok: QT installation found")
		# Activate the required Qt-libraries here
		set (QT_USE_QTTEST TRUE)
		set (QT_USE_QTTHREAD TRUE)
		set(QT_USE_QTXML TRUE)
		set(QT_USE_QTNETWORK TRUE)
		set(QT_USE_QTGUI TRUE)
		# QtCore and QtGui libraries are loaded by default
		# set(QT_DONT_USE_QTCORE TRUE)
		# set(QT_DONT_USE_QTGUI TRUE)

		# Definitions to use when compiling code that uses Qt
		add_definitions(${QT_DEFINITIONS})

		# Set your desired Qt-Version here
		include_directories(${QT_INCLUDE_DIR})
		# Path to a Cmake file that can be included to compile Qt4 applications and libraries
		include(${QT_USE_FILE})
	else(QT_FOUND)
		message("QT required to build this project, but installation cannot be found!")
	endif(QT_FOUND)
endif(PROJECT_USES_QT)
##### End Qt support #######################################################

##### Add boost support ####################################################
if(PROJECT_USES_BOOST)
	# Select boost components from (there might be more): filesystem iostreams signals serialization thread system program_options
	# Select boost components from (there might be more): filesystem iostreams signals serialization thread system program_options
	set( BOOST_ROOT /opt/boost_1_42_0 )
	set( BOOST_INCLUDEDIR /usr/local/include/boost )
	set( BOOST_LIBRARYDIR /usr/local/lib ) 
	find_package( Boost 1.42.0 EXACT )
	if(Boost_FOUND)
		message("Prerequisite ok: Boost installation found " ${Boost_VERSION} ${Boost_INCLUDE_DIRS} )
	else(Boost_FOUND)
		message("Boost required to build this project, but installation cannot be found!")
	endif(Boost_FOUND)	
	
	include_directories( ${Boost_INCLUDE_DIRS})
endif(PROJECT_USES_BOOST)
##### End boost support #####################################################

##### Update external libaries variable ####################################
set(EXTERNAL_LIBS ${Boost_LIBRARIES} ${QT_LIBRARIES})
##### End Update external libraries  #######################################

##### User defined area ####################################################

# EXAMPLE: 
# set(PC104_ROOT_DIR /opt/PC104/)
# set(PC104_LIBRARIES module monsterframes communication)
# include_directories(${PC104_ROOT_DIR})
# link_directories(${PC104_ROOT_DIR}
#		/usr/lib
# )

# Other required libraries
# set(OTHER_LIBS log4cxx sqlite3 uuid)
# add_definitions(-DLOGGER_EXTERNAL_)

# set(USER_DEFINED_LIBS ${OTHER_LIBS} ${PC104_LIBRARIES})

###### End User define area ################################################

##### COPY Configuration files into build directory
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/configuration ${PROJECT_BINARY_DIR}/configuration)

# Workaround: Cleanup the in file from build directory
execute_process(COMMAND ${CMAKE_COMMAND} -E remove -f ${PROJECT_BINARY_DIR}/configuration/${PROJECT_NAME}.pc.in)

##### INSTALL Configuration ####################################################

# include/
# TODO: recursive copy with directories
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION include/${PROJECT_NAME}
	FILES_MATCHING PATTERN "*.h")

# scripts/
install(DIRECTORY ${PROJECT_SOURCE_DIR}/scripts/ DESTINATION scripts)

# configuration/<projectname>.pc
configure_file(${PROJECT_SOURCE_DIR}/configuration/${PROJECT_NAME}.pc.in
		${PROJECT_BINARY_DIR}/configuration/${PROJECT_NAME}.pc @ONLY)

# Install pkg-config file
install(FILES ${CMAKE_BINARY_DIR}/configuration/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)

# Dont' forget to add an install for your target, when you create your executable or library
#    install(TARGETS ${PROJECT_NAME} 
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#    )

##### End INSTALL Configuration ################################################

##### Add doxygen support ###################################################
include(FindDoxygen) #sets DOXYGEN_EXECUTABLE
if(DOXYGEN_EXECUTABLE)
    # uses
    # PROJECT_NAME           = @PROJECT_NAME@
    # PROJECT_NUMBER         = @PROJECT_VERSION@
    # OUTPUT_DIRECTORY       = @PROJECT_BINARY_DIR@/doc
    # INPUT                  = @PROJECT_SOURCE_DIR@/src
    # input output @ONLY: replace @VAR@ in the input file with the cmake variables
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/doc/Doxyfile.in ${PROJECT_SOURCE_DIR}/doc/Doxyfile @ONLY)
    # documentation can be generated with 'make doc'
    ADD_CUSTOM_TARGET(doc ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/doc/Doxyfile)
    # generates documentation with cmake
    # QUIET mode is enabled in the configuration file (QUIET = YES)
    EXECUTE_PROCESS(COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/doc/Doxyfile )
endif(DOXYGEN_EXECUTABLE)
##### End doxygen support ###################################################
