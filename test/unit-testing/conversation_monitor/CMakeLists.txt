set(SOURCES
	test.cpp
)

add_definitions(-DBOOST_TEST_DYN_LINK)
set(TEST_TARGET unit-test_conversation_monitor)
add_executable(${TEST_TARGET} ${SOURCES})
target_link_libraries(${TEST_TARGET} ${PROJECT_NAME})
