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
	agent_id.cpp
	message_printer.cpp
	message_parser.cpp
	grammar_bitefficient.cpp
)

#add_executable(testParser test.cpp ${SOURCES})
#target_link_libraries(testParser ${LIBRARIES})

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} message-generator ${LIBRARIES})
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	)
	
set(TEST_TARGET test_${PROJECT_NAME})
add_executable(${TEST_TARGET} test.cpp ${SOURCES})
target_link_libraries(${TEST_TARGET} message-generator ${LIBRARIES})

add_executable(testParser main.cpp ${SOURCES})
target_link_libraries(testParser message-generator ${LIBRARIES})

