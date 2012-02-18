set(SOURCES
	test.cpp acl_message_test.cpp message_test.cpp
)

set(TEST_TARGET unit-test_messages)
add_executable(${TEST_TARGET} ${SOURCES})
target_link_libraries(${TEST_TARGET} ${PROJECT_NAME})
