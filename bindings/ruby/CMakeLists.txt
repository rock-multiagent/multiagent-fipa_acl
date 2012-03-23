include(RockRuby)
include_directories(${CMAKE_BINARY_DIR}/include)
#rock_find_pkgconfig(BASE_TYPES REQUIRED base-lib)
rock_ruby_rice_extension(fipamessage_ext ext/fipa_message.cpp)
target_link_libraries(fipamessage_ext fipa_acl)
rock_ruby_library(fipa_acl)

