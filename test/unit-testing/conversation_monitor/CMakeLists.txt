add_definitions(-DBOOST_TEST_DYN_LINK)
rock_executable(unit-test_conversation_monitor test.cpp
    DEPS ${PROJECT_NAME}
    NOINSTALL)
