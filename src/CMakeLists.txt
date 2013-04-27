set(SOURCES
    message_parser/agent_id.cpp
    message_parser/date_time.cpp
    message_parser/message_printer.cpp
    message_parser/message_parser.cpp
    message_parser/envelope_parser.cpp
    message_parser/grammar/grammar_bitefficient.cpp
    message_parser/grammar/grammar_bitefficient_envelope.cpp
    message_generator/acl_message.cpp
    message_generator/acl_envelope.cpp
    message_generator/agent_id.cpp
    message_generator/envelope_generator.cpp
    message_generator/format/bitefficient_format.cpp
    message_generator/format/bitefficient_envelope_format.cpp
    message_generator/format/bitefficient_message_format.cpp
    message_generator/message_format.cpp
    message_generator/message_generator.cpp
    message_generator/userdef_param.cpp
    conversation_monitor/message_archive.cpp
    conversation_monitor/role.cpp
    conversation_monitor/state.cpp
    conversation_monitor/statemachine_factory.cpp
    conversation_monitor/statemachine_reader.cpp
    conversation_monitor/statemachine.cpp
    conversation_monitor/transition.cpp
)

set(HEADERS
    bitefficient_message.h
    conversation_monitor.h
    conversation_monitor/message_archive.h
    conversation_monitor/role.h
    conversation_monitor/state.h
    conversation_monitor/statemachine_factory.h
    conversation_monitor/statemachine_reader.h
    conversation_monitor/statemachine.h
    conversation_monitor/transition.h
    fipa_acl.h
    message_generator/exception.h
    message_generator/userdef_param.h
    message_generator/types.h
    message_generator/agent_id.h
    message_generator/acl_message.h
    message_generator/acl_envelope.h
    message_generator/envelope_generator.h
    message_generator/format/bitefficient_format.h
    message_generator/format/bitefficient_envelope_format.h
    message_generator/format/bitefficient_message_format.h
    message_generator/message_format.h
    message_generator/message_generator.h
    message_generator/received_object.h
    message_parser/agent_id.h
    message_parser/byte_sequence.h
    message_parser/date_time.h
    message_parser/debug.h
    message_parser/envelope_parser.h
    message_parser/grammar/grammar_bitefficient.h
    message_parser/grammar/grammar_bitefficient_envelope.h
    message_parser/message_parser.h
    message_parser/message_printer.h
    message_parser/parameter.h
    message_parser/types.h
)

rock_library(${PROJECT_NAME} 
        SOURCES ${SOURCES}
        HEADERS ${HEADERS}
        DEPS_PKGCONFIG base-lib base-types tinyxml
        )
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
	
rock_executable(testParser 
    SOURCES message_parser/main.cpp
    DEPS ${PROJECT_NAME}
    NOINSTALL)

rock_executable(fipa_acl-msg_generator
        SOURCES message_generator/generator.cpp
        DEPS ${PROJECT_NAME}
        )

rock_executable(fipa_acl-conv_monitor-reader
        SOURCES conversation_monitor/main.cpp
        DEPS ${PROJECT_NAME}
        )

# do not install - only for local testing
rock_executable(fipa_acl-benchmark
        SOURCES benchmark.cpp
        DEPS ${PROJECT_NAME}
        NOINSTALL)

    #install(DIRECTORY ${PROJECT_SOURCE_DIR}/configuration/protocols DESTINATION configuration/${PROJECT_NAME})
    #
