rock_executable(unit-test_messages test.cpp 
        acl_message_test.cpp 
        message_test.cpp
    DEPS ${PROJECT_NAME}
    NOINSTALL)

rock_executable(unit-test_envelope test.cpp 
        envelope_test.cpp
    DEPS ${PROJECT_NAME}
    NOINSTALL)
