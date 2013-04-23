#ifndef FIPA_ACL_BITEFFICIENT_ENVELOPE_FORMAT_H
#define FIPA_ACL_BITEFFICIENT_ENVELOPE_FORMAT_H

#include <fipa_acl/message_generator/envelope_format.h>

namespace fipa {
namespace acl {

class BitefficientEnvelopeFormat : public EnvelopeFormat
{

private: 
    /**
     * Encode all external envelopes
     */
    std::string getAllExternalEnvelopes(const ACLEnvelope& envelope) const;

    std::string getExtEnvelope(const ACLBaseEnvelope& envelope) const;

    /**
     * Encode base envelope
     */
    std::string getBaseEnvelope(const ACLBaseEnvelope& envelope) const;

    /**
     * Encode base envelope header
     */
    std::string getBaseEnvelopeHeader(const ACLBaseEnvelope& envelope) const;

    /**
     * Encode envelope parameters
     */
    std::string getParameters(const ACLBaseEnvelope& envelope) const;


    std::string getReceivedObject(const ACLBaseEnvelope& envelope) const;

public:
    /**
     * Applies the format to the envelope
     * \return the formatted envelope object
     */
    std::string apply(const ACLEnvelope& envelope) const;

};


} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_BITEFFICIENT_ENVELOPE_FORMAT_H
