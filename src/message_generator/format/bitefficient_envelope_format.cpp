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
    std::string msg = /*getAllExternalEnvelopes(envelope) + getBaseEnvelope(envelope) +*/ envelope.getPayload();

    return msg;
}

std::string BitefficientEnvelopeFormat::getAllExternalEnvelopes(const ACLEnvelope& envelope) const
{
    ACLBaseEnvelopeList list;// = envelope.getExtraEnvelopes();
    ACLBaseEnvelopeList::const_iterator cit = list.begin();
    std::string extEnvelopes;
    for(; cit != list.end(); ++cit)
    {
        extEnvelopes += getExtEnvelope(*cit);
    }

    return extEnvelopes;
}

std::string BitefficientEnvelopeFormat::getBaseEnvelope(const ACLBaseEnvelope& envelope) const
{
    return "";
}

std::string BitefficientEnvelopeFormat::getBaseEnvelopeHeader(const ACLBaseEnvelope& envelope) const
{
    std::string representation = BitefficientFormat::getACLRepresentation(envelope.getACLRepresentation());
    std::string dataTimeToken = BitefficientFormat::getBinDateTimeToken(envelope.getDate());

    uint32_t length = 2 /** MSG_IG **/ + 2 /** for size field len16 **/ + 2 /** acl representation **/ + dataTimeToken.size();
    std::string header = "";
    header += FIPA_BASE_MSG_ID;

    uint16_t envLen = htons( (uint16_t) length); 
    header.append(reinterpret_cast<const char*>(&envLen), sizeof(uint16_t));
    header += representation;
    header += dataTimeToken;

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

std::string BitefficientEnvelopeFormat::getParameters(const ACLBaseEnvelope& envelope) const
{
    // NOT IMPLEMENTED
    return "";
}

std::string BitefficientEnvelopeFormat::getReceivedObject(const ACLBaseEnvelope& envelope) const
{
    const ReceivedObject& receivedObject = envelope.getReceivedObject();
    return BitefficientFormat::getNullTerminatedString(receivedObject.getBy()) + BitefficientFormat::getBinDateTimeToken(receivedObject.getDate());
}



} // end namespace acl
} // end namespace fipa
