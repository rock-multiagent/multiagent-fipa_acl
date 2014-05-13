#include "xml_envelope_parser.h"
#include "xml_parser.h"
#include <boost/lexical_cast.hpp>
#include <base/Logging.hpp>
#include <stdexcept>

namespace fipa {
namespace acl {
        
bool XMLEnvelopeParser::parseData(const std::string& storage, ACLEnvelope& envelope)
{
    TiXmlDocument doc;
    
    // Load the string into XML doc
    const char* parseResult = doc.Parse(storage.c_str());
    // A non-null parseResult usually indicates an error, but we seem to get that every time.
    if(parseResult != NULL)
    {
        // non-null means some error
        LOG_WARN_S << "Parsing envelope XML failed for (probably the payload): " << parseResult;
    }
    
    // The main node (envelope)
    const TiXmlElement* envelopeElem = doc.FirstChildElement();
    
    if(envelopeElem->ValueStr() != "envelope")
    {
        LOG_WARN_S << "Parsing error: XML main node not named 'fipa-message' but " << envelopeElem->ValueStr();
        return false;
    }
    
    // Parse all child elements
    const TiXmlElement* pChild = envelopeElem->FirstChildElement();
    
    // We assume the params have raising indices, starting with one.
    
    
    int paramsIndex = 1;
    // The first params is the Base envelope
    try
    {
        envelope.setBaseEnvelope(parseParameters(pChild, paramsIndex++));
    }
    catch(std::exception& e)
    {
        LOG_WARN_S << "Parsing error base envelope: " << e.what();
        return false;
    }
    
    // All remaining params are extra envelopes
    for ( pChild = pChild->NextSiblingElement(); pChild != 0; pChild = pChild->NextSiblingElement()) 
    {
        try
        {
            envelope.addExtraEnvelope(parseParameters(pChild, paramsIndex++));
        }
        catch(std::exception& e)
        {
            LOG_WARN_S << "Parsing error extra envelope: " << e.what();
            return false;
        }
    }
    // Parse the payload.
    // Get the latest values for payload-length
    PayloadLength len =  envelope.flattened().getPayloadLength();
    if(len > storage.length())
    {
        LOG_WARN_S << "Parsing error: payload length (" << len << ") bigger than storage length (" << storage.length() << ")";
        return false;
    }
    envelope.setPayload(storage.substr(storage.length() - len));
    
    return true;
}

const ACLBaseEnvelope XMLEnvelopeParser::parseParameters(const TiXmlElement* paramsElem, int paramsIndex) const
{
    if(paramsElem->ValueStr() != "params")
    {
        throw std::runtime_error("Parsing error: params node not named 'params' but " + paramsElem->ValueStr());
    }
    if(*(paramsElem->Attribute(std::string("index"))) != boost::lexical_cast<std::string>(paramsIndex))
    {
        throw std::runtime_error("Parsing error: params index attribute should be " +
            boost::lexical_cast<std::string>(paramsIndex) + " but is " + paramsElem->Attribute("index"));
    }
    
    ACLBaseEnvelope envelope;
    UserdefinedParameterList params;
    
    // Parse all child elements
    const TiXmlElement * pChild;
    for ( pChild = paramsElem->FirstChildElement(); pChild != 0; pChild = pChild->NextSiblingElement()) 
    {
        // Get the name of the element
        const std::string& name = pChild->ValueStr();
        
        if(name == "to")
        {
            envelope.setTo(XMLParser::parseAgentIDSequence(pChild));
        }
        else if(name == "from")
        {
            // There must be one child: the aid element
            const TiXmlElement* aid = pChild->FirstChildElement();
            if(aid == NULL)
            {
                throw std::runtime_error("Parsing error: from has no first child");
            }
            
            envelope.setFrom(XMLParser::parseAgentID(aid));
        }
        else if(name == "comments")
        {
            envelope.setComments(std::string(pChild->GetText()));
        }
        else if(name == "acl-representation")
        {
            envelope.setACLRepresentation(std::string(pChild->GetText()));
        }
        else if(name == "payload-length")
        {
            // TODO does this work?
            envelope.setPayloadLength(boost::lexical_cast<PayloadLength>(std::string(pChild->GetText())));
        }
        else if(name == "payload-encoding")
        {
            envelope.setPayloadEncoding(std::string(pChild->GetText()));
        }
        else if(name == "date")
        {
            envelope.setDate(XMLParser::parseDate(pChild));
        }
        else if(name == "intended-receiver")
        {
            envelope.setIntendedReceivers(XMLParser::parseAgentIDSequence(pChild));
        }
        else if(name == "received")
        {
            envelope.setReceivedObject(XMLParser::parseReceivedObject(pChild));
        }
        else if(name == "user-defined")
        {
            params.push_back(XMLParser::parseUserdefinedParameter(pChild));
        }
        else
        {
            throw std::runtime_error("Parsing error: unknown node: " + name);
        }
    }
    envelope.setUserdefinedParameters(params);
    
    return envelope;
}

} // end namespace acl
} // end namespace fipa
