#include "xml_envelope_format.h"
#include "xml_format.h"
#include "../../../../../install/include/base/Time.hpp"
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
    // Append all extra envelopes
    BOOST_FOREACH(TiXmlElement* elem, getAllExternalEnvelopes(envelope))
    {
        elem->SetAttribute("index", index++);
        envelopeElem->LinkEndChild(elem);
    }
    // And base envelope
    TiXmlElement* elem = getBaseEnvelope(envelope.getBaseEnvelope());
    elem->SetAttribute("index", index++);
    envelopeElem->LinkEndChild(elem);
    
    TiXmlPrinter printer;
    // TODO no line break and no indentation for compact result

    doc.Accept( &printer );
    // TODO also include msg payload
    // envelope.getPayload();
    
    return printer.Str();
}

std::vector<TiXmlElement*> XMLEnvelopeFormat::getAllExternalEnvelopes(const ACLEnvelope& envelope) const
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
        BOOST_FOREACH(TiXmlElement* elem, getAgentIDSequence(envelope.getTo()))
        {
            to->LinkEndChild(elem);
        }
        vec.push_back(to);
    }

    if(envelope.contains(envelope::FROM))
    {
        TiXmlElement* from = new TiXmlElement("from");
        from->LinkEndChild(getAgentID(envelope.getFrom()));
        vec.push_back(from);
    }

    if(envelope.contains(envelope::COMMENTS))
    {
        TiXmlElement* comments = new TiXmlElement("comments");
        comments->LinkEndChild(new TiXmlText(envelope.getComments()));
        vec.push_back(comments);
    }
    
    if(envelope.contains(envelope::ACL_REPRESENTATION))
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

    if(envelope.contains(envelope::PAYLOAD_ENCODING))
    {
        TiXmlElement* payloadEncoding= new TiXmlElement("payload-encoding");
        payloadEncoding->LinkEndChild(new TiXmlText(envelope.getPayloadEncoding()));
        vec.push_back(payloadEncoding);
    }
    
    if(envelope.contains(envelope::DATE))
    {
        vec.push_back(getDate(envelope.getDate()));
    }
    
    // We don't support ENCRYPTED field, otherwise this would go here

    if(envelope.contains(envelope::INTENDED_RECEIVERS))
    {
        TiXmlElement* intendedReceivers = new TiXmlElement("intended-receiver");
        BOOST_FOREACH(TiXmlElement* elem, getAgentIDSequence(envelope.getIntendedReceivers()))
        {
            intendedReceivers->LinkEndChild(elem);
        }
        vec.push_back(intendedReceivers);
    }

    if(envelope.contains(envelope::RECEIVED_OBJECT))
    {
        vec.push_back(getReceivedObject(envelope.getReceivedObject()));
    }
    
    if(envelope.contains(envelope::USERDEFINED_PARAMETERS))
    {
        BOOST_FOREACH(TiXmlElement* elem, getUserdefinedParameters(envelope.getUserdefinedParameters()))
        {
            vec.push_back(elem);
        }
    }
    
    return vec;
}

TiXmlElement* XMLEnvelopeFormat::getDate(const base::Time& date) const
{
    TiXmlElement* dateElem = new TiXmlElement("date");
    dateElem->LinkEndChild(new TiXmlText(dateToStr(date)));
    return dateElem;
}

const std::string XMLEnvelopeFormat::dateToStr(const base::Time& date) const
{
    return date.toString(base::Time::Milliseconds, "%Y%m%dT%H%M%S"); // FIXME this includes a colon between s and ms
}


std::vector< TiXmlElement* > XMLEnvelopeFormat::getAgentIDSequence(const AgentIDList& aidl) const
{
    std::vector<TiXmlElement*> vec;
    BOOST_FOREACH(AgentID aid, aidl)
    {
        vec.push_back(getAgentID(aid));
    }
    return vec;
}

TiXmlElement* XMLEnvelopeFormat::getAgentID(const AgentID& aid) const
{
    TiXmlElement* aidElem = new TiXmlElement("agent-identifier");
    
    TiXmlElement* nameElem = new TiXmlElement("name");
    nameElem->LinkEndChild(new TiXmlText(aid.getName()));
    aidElem->LinkEndChild(nameElem);
    
    std::vector<std::string> addresses = aid.getAddresses();
    if(!addresses.empty())
    {
        TiXmlElement* addressesElem = new TiXmlElement("addresses");
        BOOST_FOREACH(std::string address, addresses)
        {
            TiXmlElement* urlElem = new TiXmlElement("url");
            urlElem->LinkEndChild(new TiXmlText(address));
            addressesElem->LinkEndChild(urlElem);
        }
        aidElem->LinkEndChild(addressesElem);
    }
    
    Resolvers resolvers = aid.getResolvers();
    if(!resolvers.empty())
    {
        TiXmlElement* resolversElem = new TiXmlElement("resolvers");
        BOOST_FOREACH(TiXmlElement* resolverElem, getAgentIDSequence(resolvers))
        {
            resolversElem->LinkEndChild(resolverElem);
        }
        aidElem->LinkEndChild(resolversElem);
    }
    
    BOOST_FOREACH(TiXmlElement* elem, getUserdefinedParameters(aid.getUserdefParams()))
    {
        aidElem->LinkEndChild(elem);
    }
    
    return aidElem;
}

TiXmlElement* XMLEnvelopeFormat::getReceivedObject(const ReceivedObject& receivedObject) const
{
    // TODO by/from definition uses value, example uses attribute "value"
    
    TiXmlElement* recvElem = new TiXmlElement( "received" );
    
    TiXmlElement* recvByElem = new TiXmlElement("received-by");
    recvByElem->LinkEndChild(new TiXmlText(receivedObject.getBy()));
    recvElem->LinkEndChild(recvByElem);
    
    if(receivedObject.getFrom() != "")
    {
        TiXmlElement* recvFromElem = new TiXmlElement("received-from");
        recvFromElem->LinkEndChild(new TiXmlText(receivedObject.getFrom()));
        recvElem->LinkEndChild(recvFromElem);
    }
    
    TiXmlElement* recvDateElem = new TiXmlElement("received-date");
    recvDateElem->SetAttribute("value", dateToStr(receivedObject.getDate()));
    recvElem->LinkEndChild(recvDateElem);
    
    if(receivedObject.getId() != "")
    {
        TiXmlElement* recvIdElem = new TiXmlElement("received-id");
        recvIdElem->SetAttribute("value", receivedObject.getId());
        recvElem->LinkEndChild(recvIdElem);
    }
    
    if(receivedObject.getVia() != "")
    {
        TiXmlElement* recvViaElem = new TiXmlElement("received-via");
        recvViaElem->SetAttribute("value", receivedObject.getVia());
        recvElem->LinkEndChild(recvViaElem);
    }
    
    BOOST_FOREACH(TiXmlElement* elem, getUserdefinedParameters(receivedObject.getUserdefinedParameters()))
    {
        recvElem->LinkEndChild(elem);
    }
    
    return recvElem;
}

std::vector< TiXmlElement* > XMLEnvelopeFormat::getUserdefinedParameters(const UserdefinedParameterList& params) const
{
    std::vector<TiXmlElement*> vec;
    BOOST_FOREACH(UserdefParam userdefParam, params)
    {
        TiXmlElement* userdefParamElem = new TiXmlElement("X-" + userdefParam.getName());
        userdefParamElem->LinkEndChild(new TiXmlText(userdefParam.getValue()));
        vec.push_back(userdefParamElem);
    }
    return vec;
}

} // end namespace acl
} // end namespace fipa
