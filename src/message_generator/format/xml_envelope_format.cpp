#include "xml_envelope_format.h"
#include "xml_format.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace fipa {
namespace acl {

std::string XMLEnvelopeFormat::apply(const ACLEnvelope& envelope) const
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
    
    TiXmlElement * envelopeElem = new TiXmlElement( "envelope" );
    doc.LinkEndChild( envelopeElem );
    
    // Index for the params
    int index = 1;
    // Append base envelope
    TiXmlElement* elem = getBaseEnvelope(envelope.getBaseEnvelope());
    elem->SetAttribute("index", index++);
    envelopeElem->LinkEndChild(elem);
    // And all extra envelopes
    BOOST_FOREACH(TiXmlElement* elem, getAllExternalEnvelopes(envelope))
    {
        elem->SetAttribute("index", index++);
        envelopeElem->LinkEndChild(elem);
    }
    
    TiXmlPrinter printer;
    // TODO no line break and no indentation for compact result

    doc.Accept( &printer );
    return printer.Str() + envelope.getPayload();
}

std::vector< TiXmlElement* > XMLEnvelopeFormat::getAllExternalEnvelopes(const ACLEnvelope& envelope) const
{
    std::vector<TiXmlElement*> vec;
    const ACLBaseEnvelopeList& list = envelope.getExtraEnvelopes();
    if(!list.empty())
    {
        ACLBaseEnvelopeList::const_iterator cit = list.begin();
        for(; cit != list.end(); ++cit)
        {
            vec.push_back(getBaseEnvelope(*cit));
        }
    }
    return vec;
}

TiXmlElement* XMLEnvelopeFormat::getBaseEnvelope(const ACLBaseEnvelope& envelope) const
{
    TiXmlElement* element = new TiXmlElement("params");
    // Append children parameters
    BOOST_FOREACH(TiXmlElement* elem, getParameters(envelope))
    {
        element->LinkEndChild(elem);
    }
    return element;
}

std::vector<TiXmlElement*> XMLEnvelopeFormat::getParameters(const ACLBaseEnvelope& envelope) const
{
    std::vector<TiXmlElement*> vec;
    
    if(envelope.contains(envelope::TO))
    {
        TiXmlElement* to = new TiXmlElement("to");
        // Envelopes does not use name id or url href
        BOOST_FOREACH(TiXmlElement* elem, XMLFormat::getAgentIDSequence(envelope.getTo(), false, false))
        {
            to->LinkEndChild(elem);
        }
        vec.push_back(to);
    }

    if(envelope.contains(envelope::FROM))
    {
        TiXmlElement* from = new TiXmlElement("from");
        // Envelopes does not use name id or url href
        from->LinkEndChild(XMLFormat::getAgentID(envelope.getFrom(), false, false));
        vec.push_back(from);
    }

    if(envelope.contains(envelope::COMMENTS) && envelope.getComments() != "")
    {
        TiXmlElement* comments = new TiXmlElement("comments");
        comments->LinkEndChild(new TiXmlText(envelope.getComments()));
        vec.push_back(comments);
    }
    
    if(envelope.contains(envelope::ACL_REPRESENTATION) && envelope.getACLRepresentationString() != "")
    {
        TiXmlElement* aclRep = new TiXmlElement("acl-representation");
        aclRep->LinkEndChild(new TiXmlText(envelope.getACLRepresentationString()));
        vec.push_back(aclRep);
    }

    if(envelope.contains(envelope::PAYLOAD_LENGTH))
    {
        TiXmlElement* payloadLength = new TiXmlElement("payload-length");
        payloadLength->LinkEndChild(new TiXmlText(boost::lexical_cast<std::string>(envelope.getPayloadLength())));
        vec.push_back(payloadLength);
    }

    if(envelope.contains(envelope::PAYLOAD_ENCODING) && envelope.getPayloadEncoding() != "")
    {
        TiXmlElement* payloadEncoding= new TiXmlElement("payload-encoding");
        payloadEncoding->LinkEndChild(new TiXmlText(envelope.getPayloadEncoding()));
        vec.push_back(payloadEncoding);
    }
    
    if(envelope.contains(envelope::DATE))
    {
        vec.push_back(XMLFormat::getDate(envelope.getDate()));
    }
    
    // We don't support ENCRYPTED field, otherwise this would go here

    if(envelope.contains(envelope::INTENDED_RECEIVERS))
    {
        TiXmlElement* intendedReceivers = new TiXmlElement("intended-receiver");
        // Envelopes does not use name id or url href
        BOOST_FOREACH(TiXmlElement* elem, XMLFormat::getAgentIDSequence(envelope.getIntendedReceivers(), false, false))
        {
            intendedReceivers->LinkEndChild(elem);
        }
        vec.push_back(intendedReceivers);
    }

    if(envelope.contains(envelope::RECEIVED_OBJECT))
    {
        vec.push_back(XMLFormat::getReceivedObject(envelope.getReceivedObject()));
    }
    
    if(envelope.contains(envelope::USERDEFINED_PARAMETERS))
    {
        BOOST_FOREACH(TiXmlElement* elem, XMLFormat::getUserdefinedParameters(envelope.getUserdefinedParameters()))
        {
            vec.push_back(elem);
        }
    }
    
    return vec;
}

} // end namespace acl
} // end namespace fipa
