SET(module_name ${PROJECT_NAME})

set(SOURCES
	Transition.cpp
	State.cpp
	StateMachine.cpp
	StateMachineBuilder.cpp
	#main.cpp
)

###### Define a binary
# Note: the library (see below) and executable names can't be the same
#add_executable(${PROJECT_NAME} main.cpp dummy.cpp ${moc_cpp} ${ui_cpp})
set(TEST_TARGET builder_test_${PROJECT_NAME})
#add_executable(${TEST_TARGET} main.cpp)
add_executable(${TEST_TARGET} main.cpp )
target_link_libraries(${TEST_TARGET} message-generator ${LIBRARIES} ${PROJECT_NAME})


###### Define a library
# Note: the library and executable names can't be the same
add_library(${PROJECT_NAME} SHARED ${SOURCES})
# Link the defined target (either binary or library, see above) to libraries.
# You must use ${QT_LIBRARIES} if you have enabled Qt support in the root
# CMakeLists.txt, and ${Boost_LIBRARIES} for boost libraries
#
# If you want to use a library defined in this package, just use its name
#
target_link_libraries(${PROJECT_NAME} message-generator ${LIBRARIES} ${EXTERNAL_LIBS})



install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)





