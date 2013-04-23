#ifndef FIPA_ACL_ENVELOPE_FORMAT_H
#define FIPA_ACL_ENVELOPE_FORMAT_H

#include <fipa_acl/message_generator/acl_envelope.h>

namespace fipa {
namespace acl {

class EnvelopeFormat
{
public:
    virtual std::string apply(const ACLEnvelope& envelope) const = 0;

};

} // namespace acl
} // namespace fipa

#endif // FIPA_ACL_ENVELOPE_FORMAT_H
