set(SOURCES
	StateMachineTest.cpp
	MainTest.cpp
)

set(TEST_TARGET unit-test_${PROJECT_NAME})
add_executable(${TEST_TARGET} ${SOURCES})
target_link_libraries(${TEST_TARGET} message-generator ${PROJECT_NAME} cppunit ${LIBRARIES})

