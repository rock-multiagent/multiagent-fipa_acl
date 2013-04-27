#ifndef FIPA_ACL_ENVELOPE_GENERATOR_H
#define FIPA_ACL_ENVELOPE_GENERATOR_H

#include <fipa_acl/message_generator/envelope_format.h>
#include <fipa_acl/message_generator/types.h>

namespace fipa {
namespace acl {

class EnvelopeGenerator
{
    static std::map<representation::Type, EnvelopeFormatPtr> msFormats;
public:

    /**
     * Create an envelope object of a certain al representation (format), e.g. 
     * bitefficient, string or xml
     * \return Envelope including message content
     */
    static std::string create(const ACLEnvelope& msg, const representation::Type& acl_representation);

};

} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_ENVELOPE_GENERATOR_H
