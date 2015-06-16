#ifndef FIPA_ACL_ENVELOPE_FORMAT_H
#define FIPA_ACL_ENVELOPE_FORMAT_H

#include <fipa_acl/message_generator/acl_envelope.h>
#include <boost/shared_ptr.hpp>

namespace fipa {
namespace acl {

class EnvelopeFormat
{
public:
    virtual ~EnvelopeFormat() {}

    virtual std::string apply(const ACLEnvelope& envelope) const = 0;

};

typedef boost::shared_ptr<EnvelopeFormat> EnvelopeFormatPtr;

} // namespace acl
} // namespace fipa

#endif // FIPA_ACL_ENVELOPE_FORMAT_H
