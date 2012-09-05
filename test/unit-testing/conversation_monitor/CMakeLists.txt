set(SOURCES test.cpp
        statemachine_test.cpp
)

rock_executable(unit-test_conversation_monitor ${SOURCES}
    DEPS ${PROJECT_NAME}
    NOINSTALL)
