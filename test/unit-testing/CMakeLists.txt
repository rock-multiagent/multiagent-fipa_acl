set(SOURCES
	StateMachineTest.cpp
	MainTest.cpp
	
)


#include(FindPkgConfig RESULT_VARIABLE PKG_CONFIG_FOUND)
#pkg_check_modules(ConversationMonitor REQUIRED "conversation-monitor")

#include_directories(${ConversationMonitor_INCLUDE_DIRS})
#link_directories(${ConversationMonitor_LIBRARY_DIRS})

#set(TEST_TARGET2 builder_test_${PROJECT_NAME})
#add_executable(${TEST_TARGET2} main.cpp)
#target_link_libraries(${PROJECT_NAME} conversation-monitor ${LIBRARIES} ${EXTERNAL_LIBS})

set(TEST_TARGET unit-test_${PROJECT_NAME})
add_executable(${TEST_TARGET} ${SOURCES})
target_link_libraries(${TEST_TARGET} message-generator ${PROJECT_NAME} cppunit ${LIBRARIES})

