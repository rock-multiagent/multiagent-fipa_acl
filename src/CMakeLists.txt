#SET(main_cpp main.cpp)
# Set 
SET(module_name ${PROJECT_NAME})
## Note: the library and executable names can't be the same
##add_library(${PROJECT_NAME} SHARED dummy.cpp ${EXTERNAL_LIBS} ${moc_cpp} ${ui_cpp})
## Link the defined target (either binary or library, see above) to libraries.
## You must use ${QT_LIBRARIES} if you have enabled Qt support in the root
## CMakeLists.txt, and ${Boost_LIBRARIES} for boost libraries
##
## If you want to use a library defined in this package, just use its name
##
##target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS})

#install(TARGETS ${PROJECT_NAME} 
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#)
set(SOURCES
	message_parser/agent_id.cpp
	message_parser/message_printer.cpp
	message_parser/message_parser.cpp
	message_parser/grammar_bitefficient.cpp
        message_generator/acl_message.cpp
        message_generator/agent_id.cpp
        message_generator/userdef_param.cpp
        message_generator/acl_message_output_parser.cpp
)

#add_executable(testParser test.cpp ${SOURCES})
#target_link_libraries(testParser ${LIBRARIES})

add_library(${PROJECT_NAME} SHARED ${SOURCES})

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	)
	
add_executable(testParser message_parser/main.cpp )
target_link_libraries(testParser ${LIBRARIES} ${PROJECT_NAME})

set(GENERATOR fipa_acl-msg_generator)
add_executable(${GENERATOR} message_generator/generator.cpp)
target_link_libraries(${GENERATOR} ${LIBRARIES} ${PROJECT_NAME})
install(TARGETS ${GENERATOR}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

