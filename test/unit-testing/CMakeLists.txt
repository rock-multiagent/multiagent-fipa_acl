set(SOURCES
	test.cpp
)

add_definitions(-DBOOST_TEST_DYN_LINK)
set(TEST_TARGET unit-test_${PROJECT_NAME})
add_executable(${TEST_TARGET} ${SOURCES})
target_link_libraries(${TEST_TARGET} ${PROJECT_NAME})

