SET(module_name ${PROJECT_NAME})

set(SOURCES
	ACLMessage.cpp
	AgentID.cpp
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

set(GENERATOR fipa-msg-generator)
add_executable(${GENERATOR} generator.cpp ${SOURCES})
target_link_libraries(${GENERATOR} ${LIBRARIES})
install(TARGETS ${GENERATOR}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

set(TEST fipa-msg-generator-test)
add_executable(${TEST} test.cpp ${SOURCES})
target_link_libraries(${TEST} ${LIBRARIES})

