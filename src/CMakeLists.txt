SET(main_cpp main.cpp)
# Set 
SET(module_name ${PROJECT_NAME})
SET(sources 
	# Insert your sources here
        dummy.cpp) 

if(QT_FOUND)
	# Insert any headers of objects that needs to be compiled with moc
	SET(sources_MOC_HDRS
		# source1.h
		# source2.h
	)


	# QT Ui Files 
	SET(UI_SRCS	
		# source.ui
	)

	# Generate headers from .ui files
	QT4_WRAP_UI(sources_ui ${UI_SRCS})

	# Perform moc compilation
	QT4_WRAP_CPP(sources_MOC_SRCS ${subsystem_MOC_HDRS})

	# Include the output directory here when you use QT, otherwise the ui file won't be wrapped
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif(QT_FOUND)

find_file(executable ${main_cpp} PATHS ${CMAKE_CURRENT_SOURCE_DIR} NO_DEFAULT_PATH)
if(executable)
    add_executable(${PROJECT_NAME} ${sources} ${main_cpp} ${sources_MOC_HDRS} ${sources_ui})   
else()
    add_library(${PROJECT_NAME} ${sources} ${sources_MOC_HDRS})
endif(executable)

find_file(projectbinary ${PROJECT_NAME})
if(projectbinary)
	target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS} ${USER_DEFINED_LIBS})
	install(TARGETS ${PROJECT_NAME} 
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
	)
endif(projectbinary)
