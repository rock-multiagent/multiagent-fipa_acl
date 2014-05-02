#ifndef FIPA_ACL_XML_ENVELOPE_PARSER_H
#define FIPA_ACL_XML_ENVELOPE_PARSER_H

#include <string>
#include <fipa_acl/message_generator/acl_envelope.h>
#include <fipa_acl/message_generator/types.h>
#include <fipa_acl/message_parser/envelope_parser.h>

namespace fipa {
namespace acl {

class XMLEnvelopeParser : public EnvelopeParserImplementation
{

public: 
    bool parseData(const std::string& storage, ACLEnvelope& envelope);
};

} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_XML_ENVELOPE_PARSER_H
