set(SOURCES
	ACLMessageTest.cpp
	MainTest.cpp
)
set (SOURCES2 mytest.cpp)

#include(FindPkgConfig RESULT_VARIABLE PKG_CONFIG_FOUND)
#pkg_check_modules(MESSAGE_PARSER "message-parser")

set(TEST_TARGET unit-test_${PROJECT_NAME})
set(TEST_TARGET2 mytest_${PROJECT_NAME})
add_executable(${TEST_TARGET} MainTest.cpp ${SOURCES})
target_link_libraries(${TEST_TARGET} message-generator cppunit ${LIBRARIES})

add_executable(${TEST_TARGET2} mytest.cpp ${SOURCES2})
target_link_libraries(${TEST_TARGET2} message-generator ${LIBRARIES})
