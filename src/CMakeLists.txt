SET(module_name ${PROJECT_NAME})

set(SOURCES
	ACLMessage.cpp
	AgentAID.cpp
	UserdefParam.cpp
	ACLMessageOutputParser.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)	


#SET(main_cpp test.cpp )
#SET(main_cpp )
# Set 
#SET(module_name ${PROJECT_NAME})
#SET(sources 
#	# Insert your sources here
#	# ACLMessage.h
#	ACLMessage.cpp
#	AgentAID.cpp
#	UserdefParam.cpp
#	ACLMessageOutputParser.cpp
#	test.cpp
#	#../../message-parser/src/message_parser.cpp
#	
#	# message_parser.cpp
#       ) 
#SET(USER_DEFINED_LIBS
#	ACLMessage.h
#	)
#SET(sources_MOC_HDRS
#	ACLMessage.h
#	)

# ADD_EXECUTABLE(test1 test.cpp)
#find_file(executable ${main_cpp} PATHS ${CMAKE_CURRENT_SOURCE_DIR} NO_DEFAULT_PATH)
#if(executable)
#    add_executable(${PROJECT_NAME} ${sources} ${main_cpp})   
#else()
#    message("Add lib")
#    add_library(${PROJECT_NAME} SHARED ${sources}) 
#endif(executable)
#
#
#find_file(projectbinary ${PROJECT_NAME})
#if(projectbinary)
#	target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS} ${USER_DEFINED_LIBS} )
#	install(TARGETS ${PROJECT_NAME} 
#			RUNTIME DESTINATION bin
#			LIBRARY DESTINATION lib
#	)
#endif(projectbinary)
#
#install(TARGETS ${PROJECT_NAME} 
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#)

