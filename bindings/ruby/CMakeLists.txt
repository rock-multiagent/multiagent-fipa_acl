INCLUDE(RubyExtensions)

set(Gem_DEBUG TRUE)
set(Gem_NAME "rice")
find_package(Gem)
include_directories(${GEM_INCLUDE_DIRS})

##### Begin of customization
set(_pkgconfig_name fipa-message-ruby)
set(_extension_name fipamessage_ruby)
set(RUBY_SOURCES 
	ext/fipa_message.cpp
)

message(${RUBY_INCLUDE_PATH})

set(_link_libraries ${PROJECT_NAME} ${GEM_LIBRARIES})

if(RDOC_EXECUTABLE)
    ADD_CUSTOM_TARGET(ruby_doc rdoc -o ${CMAKE_BINARY_DIR}/doc/html/ruby/ --title "FipaMesssage: a fipa message manipulation library" --main README.txt -U ext lib README.txt
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif(RDOC_EXECUTABLE)
##### End of customization


##### Generic setup
ADD_RUBY_EXTENSION(${_extension_name}
    ${RUBY_SOURCES}
)

TARGET_LINK_LIBRARIES(${_extension_name} ${_link_libraries})

# Prepare pkgconfig file
CONFIGURE_FILE(${_pkgconfig_name}.pc.in ${CMAKE_CURRENT_BINARY_DIR}/${_pkgconfig_name}.pc @ONLY)

INSTALL(TARGETS ${_extension_name}
    LIBRARY DESTINATION ${RUBY_EXTENSIONS_INSTALL_DIR})

INSTALL(DIRECTORY ext/
	DESTINATION ${RUBY_EXTENSIONS_INSTALL_DIR}
	FILES_MATCHING PATTERN "*.h"
		       PATTERN "*.hh"
		       PATTERN "*.hpp" 
	)

# Installation of pkgconfig file
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_pkgconfig_name}.pc DESTINATION lib/pkgconfig)

# Installation
INSTALL(DIRECTORY lib/
    DESTINATION ${RUBY_LIBRARY_INSTALL_DIR}
    FILES_MATCHING PATTERN "*.rb"
    )

##### End generic setup


