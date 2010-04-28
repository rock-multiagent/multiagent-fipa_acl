
#SET(main_cpp main.cpp)
# Set 
SET(module_name ${PROJECT_NAME})
#SET(sources 
#	# Insert your sources here
#	message_parser.cpp
#)

#find_file(executable ${main_cpp} PATHS ${CMAKE_CURRENT_SOURCE_DIR} NO_DEFAULT_PATH)
#if(executable)
#    add_executable(${PROJECT_NAME} ${sources} ${main_cpp} ${sources_MOC_HDRS} ${sources_ui})   
#else()
#    add_library(${PROJECT_NAME} SHARED ${sources} ${sources_MOC_HDRS} ${MessageGenerator_LIBRARIES})
#endif(executable)
#
#find_file(projectbinary ${PROJECT_NAME})
#
#if(projectbinary)
#	target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS} ${USER_DEFINED_LIBS} ${MessageParser_LIBRARY})
#	install(TARGETS ${PROJECT_NAME} 
#			RUNTIME DESTINATION bin
#			LIBRARY DESTINATION lib
#	)
#endif(projectbinary)


####### Uncomment the following block for Qt setup
## include_directories(${CMAKE_CURRENT_BINARY_DIR})
####### Uncomment the following block for Qt setup
## include_directories(${CMAKE_CURRENT_BINARY_DIR})
## # Add your .ui files ot the following line (and uncomment it)
## QT4_WRAP_UI(ui_cpp myproject.ui)
## # Add the .h files that define QObject subclasses (that have a Q_OBJECT
## # statement)
## QT4_WRAP_CPP(moc_cpp dummy.h)

####### Define a binary
## Note: the library (see below) and executable names can't be the same
#add_executable(${PROJECT_NAME} main.cpp dummy.cpp ${moc_cpp} ${ui_cpp})

####### Define a library
## Note: the library and executable names can't be the same
##add_library(${PROJECT_NAME} SHARED dummy.cpp ${EXTERNAL_LIBS} ${moc_cpp} ${ui_cpp})
## Link the defined target (either binary or library, see above) to libraries.
## You must use ${QT_LIBRARIES} if you have enabled Qt support in the root
## CMakeLists.txt, and ${Boost_LIBRARIES} for boost libraries
##
## If you want to use a library defined in this package, just use its name
##
##target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS})

#install(TARGETS ${PROJECT_NAME} 
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#)
set(SOURCES
	message_parser.cpp
)

add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
)	
