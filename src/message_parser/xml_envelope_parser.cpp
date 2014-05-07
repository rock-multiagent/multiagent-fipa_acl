#include "xml_envelope_parser.h"
#include "xml_parser.h"
#include <boost/lexical_cast.hpp>

namespace fipa {
namespace acl {
        
bool XMLEnvelopeParser::parseData(const std::string& storage, ACLEnvelope& envelope)
{
    TiXmlDocument doc;
    
    // TODO in 3 classes: also check xml names everywhere
    
    // Load the string into XML doc
    // FIXME this will also load the payload and fail!
    const char* parseResult = doc.Parse(storage.c_str());
    if(parseResult != NULL)
    {
        // non-null means error
        std::cout << "xml parser failed: " << parseResult << std::endl;
        //return false;
    }
    
    // The main node (envelope)
    const TiXmlElement* envelopeElem = doc.FirstChildElement();
    
    // Parse all child elements
    const TiXmlElement* pChild = envelopeElem->FirstChildElement();
    
    // XXX We assume the params have raising indices, starting with one.
    // This is actually not necesarily true.
    
    // The first params is the Base envelope
    try
    {
        envelope.setBaseEnvelope(parseParameters(pChild));
    }
    catch(std::exception& e)
    {
        std::cout << "base envelope parser failed: " << e.what() << std::endl;
        return false;
    }
    
    // All remaining params are extra envelopes
    for ( pChild = pChild->NextSiblingElement(); pChild != 0; pChild = pChild->NextSiblingElement()) 
    {
        try
        {
            envelope.addExtraEnvelope(parseParameters(pChild));
        }
        catch(std::exception& e)
        {
            std::cout << "extra envelope parser failed: " << e.what() << std::endl;
            return false;
        }
    }
    // TODO parse the payload.
    
    return true;
}

const ACLBaseEnvelope XMLEnvelopeParser::parseParameters(const TiXmlElement* paramsElem) const
{
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
            envelope.setFrom(XMLParser::parseAgentID(pChild));
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
        else if(name.substr(2) == "X-")
        {
            params.push_back(XMLParser::parseUserdefinedParameter(pChild));
        }
    }
    envelope.setUserdefinedParameters(params);
    
    return envelope;
}

} // end namespace acl
} // end namespace fipa
