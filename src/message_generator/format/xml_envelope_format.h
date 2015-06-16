#ifndef FIPA_ACL_XML_ENVELOPE_FORMAT_H
#define FIPA_ACL_XML_ENVELOPE_FORMAT_H

#include <fipa_acl/message_generator/envelope_format.h>
#include "tinyxml.h"

namespace fipa {
namespace acl {

class XMLEnvelopeFormat : public EnvelopeFormat
{

private:
    /**
     * Encode all external envelopes
     */
    std::vector< TiXmlElement* > getAllExternalEnvelopes(const ACLEnvelope& envelope) const;
    /**
     * Encode base envelope
     */
    TiXmlElement* getBaseEnvelope(const ACLBaseEnvelope& envelope) const;
    /**
     * Encode envelope parameters
     */
    std::vector< TiXmlElement* > getParameters(const ACLBaseEnvelope& envelope) const;
public:

    virtual ~XMLEnvelopeFormat() {}

    /**
     * Applies the format to the envelope
     * \return the formatted envelope object
     */
    std::string apply(const ACLEnvelope& envelope) const;

};


} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_XML_ENVELOPE_FORMAT_H
