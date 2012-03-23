set(SOURCES
	test.cpp acl_message_test.cpp message_test.cpp
)

rock_executable(unit-test_messages ${SOURCES}
    DEPS ${PROJECT_NAME}
    NOINSTALL)
