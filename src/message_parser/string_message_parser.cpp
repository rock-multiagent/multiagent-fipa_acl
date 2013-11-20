#include "string_message_parser.h"

#include "grammar/grammar_string_message.h"
#include <base/Logging.hpp>

namespace fipa {
namespace acl {

bool StringMessageParser::parseData(const std::string& storage, ACLMessage& msg)
{
    typedef fipa::acl::grammar::string::Message<std::string::const_iterator> string_message_grammar;
    string_message_grammar grammar;

    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();
    return parse(iter, end, grammar, msg);
}

} // end namespace acl
} // end namespace fipa