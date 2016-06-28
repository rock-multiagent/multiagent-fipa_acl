#include "xml_envelope_parser.h"
#include "xml_parser.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <base-logging/Logging.hpp>
#include <stdexcept>

namespace fipa {
namespace acl {
        
bool XMLEnvelopeParser::parseData(const std::string& storage, ACLEnvelope& envelope)
{
    TiXmlDocument doc;
    LOG_INFO_S << "XMLEnvelopeParser: starting to parse: " << storage;

    std::string envelopeXML;
    std::string payloadData;

    // check whether we can split the document into envelope and content
    std::string envelopeEndMarker = "</envelope>";
    size_t pos = storage.find(envelopeEndMarker);
    if(pos == std::string::npos)
    {
        LOG_WARN_S << "XMLEnvelopeParser: this is not an XML envelope. Could not find </envelope>";
        return false;
    } else {
        envelopeXML = storage.substr(0, pos + envelopeEndMarker.size());

        size_t payloadPosition = pos + envelopeEndMarker.size();
        if(payloadPosition < storage.length());
        {
            envelope.setPayload(storage.substr(payloadPosition));
        }
    }
    
    // Load the string into XML doc
    const char* parseResult = doc.Parse(envelopeXML.c_str());
    // A non-null parseResult usually indicates an error, but we seem to get that every time.
    if(parseResult != NULL)
    {
        // non-null means some error
        LOG_WARN_S << "Parsing envelope XML failed for (probably the payload): " << parseResult;
    }
    LOG_INFO_S << "XMLEnvelopeParser: basic XML parsing complete.";
    
    // The main node (envelope)
    const TiXmlElement* envelopeElem = doc.FirstChildElement();
    
    if(envelopeElem == NULL)
    {
        LOG_WARN_S << "Parsing error: XML main node not found.";
        return false;
    }
    
    if(envelopeElem->ValueStr() != "envelope")
    {
        LOG_WARN_S << "Parsing error: XML main node not named 'fipa-message' but " << envelopeElem->ValueStr();
        return false;
    }
    
    // Parse all child elements
    const TiXmlElement* pChild = envelopeElem->FirstChildElement();
    
    if(pChild == NULL)
    {
        LOG_WARN_S << "Parsing error: XML first params node not found.";
        return false;
    }
    
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
