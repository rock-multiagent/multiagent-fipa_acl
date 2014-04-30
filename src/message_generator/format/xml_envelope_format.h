#ifndef FIPA_ACL_XML_ENVELOPE_FORMAT_H
#define FIPA_ACL_XML_ENVELOPE_FORMAT_H

#include <fipa_acl/message_generator/envelope_format.h>
#include "tinyxml.h"

namespace fipa {
namespace acl {

class XMLEnvelopeFormat : public EnvelopeFormat
{

private:
    // TODO constanize
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


    TiXmlElement* getReceivedObject(const ReceivedObject& receivedObject) const;
    TiXmlElement* getDate(const base::Time& date) const;
    const std::string dateToStr(const base::Time& date) const;
    TiXmlElement* getAgentID(const AgentID& aid) const;
    std::vector< TiXmlElement* > getAgentIDSequence(const AgentIDList& aidl) const;
    std::vector< TiXmlElement* > getUserdefinedParameters(const UserdefinedParameterList& params) const;
public:
    /**
     * Applies the format to the envelope
     * \return the formatted envelope object
     */
    std::string apply(const ACLEnvelope& envelope) const;

};


} // end namespace acl
} // end namespace fipa

#endif // FIPA_ACL_XML_ENVELOPE_FORMAT_H
