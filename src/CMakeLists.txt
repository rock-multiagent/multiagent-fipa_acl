#SET(main_cpp )
# Set 
SET(module_name ${PROJECT_NAME})
SET(sources 
	ACLMessage.cpp
	ACLMessageOutputParser.cpp
        AgentAID.cpp
	UserdefParam.cpp
	) 

#find_file(executable ${main_cpp} PATHS ${CMAKE_CURRENT_SOURCE_DIR} NO_DEFAULT_PATH)
#if(executable)
#    add_executable(${PROJECT_NAME} ${sources} ${main_cpp} ${sources_MOC_HDRS} ${sources_ui})   
#else()
    add_library(${PROJECT_NAME} ${sources} ${sources_MOC_HDRS})
#endif(executable)

find_file(projectbinary ${PROJECT_NAME})
if(projectbinary)
	target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS} ${USER_DEFINED_LIBS})
	install(TARGETS ${PROJECT_NAME} 
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
	)
endif(projectbinary)
