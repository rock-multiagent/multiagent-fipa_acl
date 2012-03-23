include(RockRuby)
include_directories(${CMAKE_BINARY_DIR}/include)
#rock_find_pkgconfig(BASE_TYPES REQUIRED base-lib)
rock_ruby_rice_extension(fipamessage_ruby ext/fipa_message.cpp)
target_link_libraries(fipamessage_ruby fipa_acl)
set(FILES fipa-message.rb
    fipa-message/core.rb
    )

rock_ruby_library(fipa-message ${FILES})

