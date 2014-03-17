set(SOURCES test.cpp
        statemachine_test.cpp
        conversation_monitor_test.cpp
        conversation_test.cpp
        utils.cpp
)

rock_executable(unit-test_conversation_monitor ${SOURCES}
    DEPS ${PROJECT_NAME}
    NOINSTALL)
