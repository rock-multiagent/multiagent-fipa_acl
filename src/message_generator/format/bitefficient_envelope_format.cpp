#include "bitefficient_envelope_format.h"
#include "bitefficient_format.h"
#include <arpa/inet.h>
#include <limits>

#define FIPA_BASE_MSG_ID char(0xfe)
#define FIPA_EXT_MSG_ID char(0xfd)

namespace fipa {
namespace acl {

std::string BitefficientEnvelopeFormat::apply(const ACLEnvelope& envelope) const
{
    std::string msg = getAllExternalEnvelopes(envelope) + getBaseEnvelope(envelope.getBaseEnvelope()) + envelope.getPayload();

    return msg;
}

std::string BitefficientEnvelopeFormat::getAllExternalEnvelopes(const ACLEnvelope& envelope) const
{
    std::string extEnvelopes;
    const ACLBaseEnvelopeList& list = envelope.getExtraEnvelopes();
    if(!list.empty())
    {
        ACLBaseEnvelopeList::const_iterator cit = list.begin();
        for(; cit != list.end(); ++cit)
        {
            extEnvelopes += getExtEnvelope(*cit);
        }
    }
    return extEnvelopes;
}

std::string BitefficientEnvelopeFormat::getBaseEnvelope(const ACLBaseEnvelope& envelope) const
{
    std::string baseEnvelope;
    baseEnvelope += getBaseEnvelopeHeader(envelope);
    baseEnvelope += getParameters(envelope);
    baseEnvelope += BitefficientFormat::getEOFCollection();
    return baseEnvelope;
}

std::string BitefficientEnvelopeFormat::getParameters(const ACLBaseEnvelope& envelope) const
{
    std::string baseEnvelope;
    if(envelope.contains(envelope::TO))
    {
        baseEnvelope += char(0x02) + BitefficientFormat::getAgentIDSequence(envelope.getTo()); 
    }

    if(envelope.contains(envelope::FROM))
    {
        baseEnvelope += char(0x03) + BitefficientFormat::getAgentID(envelope.getFrom());
    }

    if(envelope.contains(envelope::ACL_REPRESENTATION))
    {
        baseEnvelope += char(0x04) + BitefficientFormat::getACLRepresentation(envelope.getACLRepresentation());
    }

    if(envelope.contains(envelope::COMMENTS))
    {
        baseEnvelope += char(0x05) + BitefficientFormat::getNullTerminatedString(envelope.getComments());
    }

    if(envelope.contains(envelope::PAYLOAD_LENGTH))
    {
        baseEnvelope += char(0x06) + BitefficientFormat::getBinNumber(envelope.getPayloadLength());
    }

    if(envelope.contains(envelope::PAYLOAD_ENCODING))
    {
        baseEnvelope += char(0x07) + BitefficientFormat::getNullTerminatedString(envelope.getPayloadEncoding());
    }

    if(envelope.contains(envelope::INTENDED_RECEIVERS))
    {
        baseEnvelope += char(0x09) + BitefficientFormat::getAgentIDSequence(envelope.getIntendedReceivers());
    }

    if(envelope.contains(envelope::RECEIVED_OBJECT))
    {
        baseEnvelope += char(0x0a) + BitefficientFormat::getReceivedObject(envelope.getReceivedObject());
    }
    
    if(envelope.contains(envelope::TRANSPORT_BEHAVIOUR))
    {
        baseEnvelope += char(0x0b) + BitefficientFormat::getBinStringNoCodetable(envelope.getTransportBehaviour()); 
    }
    return baseEnvelope;
}

std::string BitefficientEnvelopeFormat::getBaseEnvelopeHeader(const ACLBaseEnvelope& envelope) const
{
    std::string representation = BitefficientFormat::getACLRepresentation(envelope.getACLRepresentation());
    std::string dateTimeToken = BitefficientFormat::getBinDateTimeToken(envelope.getDate());

    uint32_t length = 2 /** MSG_IG **/ + 2 /** for size field len16 **/ + 2 /** acl representation **/ + dateTimeToken.size();
    std::string header;
    header += FIPA_BASE_MSG_ID;

    uint16_t envLen = htons( (uint16_t) length); 
    header.append(reinterpret_cast<const char*>(&envLen), sizeof(uint16_t));
    header += representation;
    header += dateTimeToken;

    return header;
}

std::string BitefficientEnvelopeFormat::getExtEnvelope(const ACLBaseEnvelope& envelope) const
{
    std::string parameters = getParameters(envelope);
    std::string receivedObject = getReceivedObject(envelope); 

    uint32_t length = 2 /** MSG_ID **/ + 2 /** for size field len16 **/ + parameters.size() + 1 /** End of envelope **/;
    std::string extEnvelope = "";
    extEnvelope += FIPA_EXT_MSG_ID;
    if(length > std::numeric_limits<uint16_t>::max() )
    {
        length += 4; // for jumbo size
        extEnvelope.append(reinterpret_cast<const char*>((uint16_t) 0), sizeof(uint16_t));
        uint32_t envLen = htonl( length );
        extEnvelope.append(reinterpret_cast<const char*>(&envLen), sizeof(uint32_t));
    } else {
        uint16_t envLen = htons( length );
        extEnvelope.append(reinterpret_cast<const char*>(&envLen), sizeof(uint16_t));
    }
    extEnvelope += receivedObject;
    extEnvelope += parameters;
    extEnvelope += BitefficientFormat::getEOFCollection();
    return extEnvelope; 
}

std::string BitefficientEnvelopeFormat::getReceivedObject(const ACLBaseEnvelope& envelope) const
{
    const ReceivedObject& receivedObject = envelope.getReceivedObject();
    std::string encoded;
    // By
    encoded += BitefficientFormat::getNullTerminatedString(receivedObject.getBy());
    // Date
    encoded += BitefficientFormat::getBinDateTimeToken(receivedObject.getDate());
    // From
    URL from = receivedObject.getFrom();
    if(!from.empty())
    {
        encoded += char(0x02) + BitefficientFormat::getNullTerminatedString(from);
    }
    // Id
    ID id = receivedObject.getId();
    if(!id.empty())
    {
        encoded += char(0x03) + BitefficientFormat::getNullTerminatedString(id);
    }
    // Via
    Via via = receivedObject.getVia();
    if(!via.empty())
    {
        encoded += char(0x04) + BitefficientFormat::getNullTerminatedString(via);
    }

    encoded += BitefficientFormat::getEOFCollection();
    return encoded;
}



} // end namespace acl
} // end namespace fipa
