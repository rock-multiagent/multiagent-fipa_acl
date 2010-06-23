
set(SOURCES
	ACLMessageTest.cpp
	MainTest.cpp
)
#set (SOURCES2 mytest.cpp)

include(FindPkgConfig RESULT_VARIABLE PKG_CONFIG_FOUND)
#pkg_check_modules(MESSAGE_PARSER REQUIRED "message-parser")
pkg_check_modules(MESSAGE_GENERATOR REQUIRED "message-generator")

include_directories( ${MESSAGE_GENERATOR_INCLUDE_DIRS}
	 ${MESSAGE_PARSER_INCLUDE_DIRS})
link_directories( ${MESSAGE_GENERATOR_LIBRARY_DIRS} 
	${MESSAGE_PARSER_LIBRARY_DIRS})

set(TEST_TARGET unit-test_${PROJECT_NAME})
#set(TEST_TARGET2 mytest_${PROJECT_NAME})
add_executable(${TEST_TARGET} ${SOURCES})
target_link_libraries(${TEST_TARGET} message-generator cppunit message-parser ${LIBRARIES})

#add_executable(${TEST_TARGET2} mytest.cpp ${SOURCES2})
#target_link_libraries(${TEST_TARGET2} message-generator ${LIBRARIES})
