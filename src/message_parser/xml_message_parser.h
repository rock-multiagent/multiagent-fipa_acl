#ifndef FIPAACL_XML_MESSAGE_PARSER_H
#define FIPAACL_XML_MESSAGE_PARSER_H

#include <fipa_acl/message_parser/message_parser.h>

namespace fipa {
namespace acl {

class XMLMessageParser : public MessageParserImplementation
{
    bool parseData(const std::string& storage, ACLMessage& msg); 
};

} // end namespace acl
} // end namespace fipa

#endif // FIPAACL_XML_MESSAGE_PARSER_H
