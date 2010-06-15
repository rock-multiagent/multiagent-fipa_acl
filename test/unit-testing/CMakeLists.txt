SET(module_name ${PROJECT_NAME})

set(SOURCES
	ACLMessageTest.cpp
	MainTest.cpp
)

#add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	#ARCHIVE DESTINATION lib
	#LIBRARY DESTINATION lib
)	

#include(FindPkgConfig RESULT_VARIABLE PKG_CONFIG_FOUND)
#pkg_check_modules(MESSAGE_PARSER "message-parser")

set(TEST_TARGET unit-test_${PROJECT_NAME})
add_executable(${TEST_TARGET} MainTest.cpp ${SOURCES})
target_link_libraries(${TEST_TARGET} message-generator ${LIBRARIES})
