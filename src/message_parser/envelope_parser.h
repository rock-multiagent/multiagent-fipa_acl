#ifndef FIPA_ACL_ENVELOPE_PARSER_H
#define FIPA_ACL_ENVELOPE_PARSER_H

#include <string>
#include <fipa_acl/message_generator/acl_envelope.h>

namespace fipa {
namespace acl {

class EnvelopeParser
{

public: 
    bool parseData(const std::string& storage, ACLEnvelope& envelope, representation::Type type);

};

} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_ENVELOPE_PARSER_H
