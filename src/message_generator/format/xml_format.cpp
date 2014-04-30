#include "xml_format.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string/erase.hpp>

namespace fipa {
namespace acl {

    // TODO use getUrl
    
TiXmlElement* XMLFormat::getDate(const base::Time& date)
{
    TiXmlElement* dateElem = new TiXmlElement("date");
    dateElem->LinkEndChild(new TiXmlText(dateToStr(date)));
    return dateElem;
}

const std::string XMLFormat::dateToStr(const base::Time& date)
{
    std::string dateStr = date.toString(base::Time::Milliseconds, "%Y%m%dT%H%M%S");
    boost::erase_all(dateStr, ":"); // Erase the colon
    return dateStr;
}

TiXmlElement* XMLFormat::getURL(const std::string& url, bool useHref)
{
    TiXmlElement* urlElem = new TiXmlElement("url");
    if(useHref)
    {
        urlElem->SetAttribute("href", url);
    }
    else
    {
        urlElem->LinkEndChild(new TiXmlText(url));
    }
    return urlElem;
}

TiXmlElement* XMLFormat::getName(const std::string& name, bool useId)
{
    TiXmlElement* nameElem = new TiXmlElement("name");
    if(useId)
    {
        // TODO what is the difference between id and refid
        nameElem->SetAttribute("id", name);
    }
    else
    {
        nameElem->LinkEndChild(new TiXmlText(name));
    }
    return nameElem;
}

std::vector< TiXmlElement* > XMLFormat::getAgentIDSequence(const AgentIDList& aidl, bool useNameId, bool useUrlHref)
{
    std::vector<TiXmlElement*> vec;
    BOOST_FOREACH(AgentID aid, aidl)
    {
        vec.push_back(getAgentID(aid, useNameId, useUrlHref));
    }
    return vec;
}

TiXmlElement* XMLFormat::getAgentID(const AgentID& aid, bool useNameId, bool useUrlHref)
{
    TiXmlElement* aidElem = new TiXmlElement("agent-identifier");
    
    TiXmlElement* nameElem = getName(aid.getName(), useNameId);
    aidElem->LinkEndChild(nameElem);
    
    std::vector<std::string> addresses = aid.getAddresses();
    if(!addresses.empty())
    {
        TiXmlElement* addressesElem = new TiXmlElement("addresses");
        BOOST_FOREACH(std::string address, addresses)
        {
            TiXmlElement* urlElem = getURL(address, useUrlHref);
            addressesElem->LinkEndChild(urlElem);
        }
        aidElem->LinkEndChild(addressesElem);
    }
    
    Resolvers resolvers = aid.getResolvers();
    if(!resolvers.empty())
    {
        TiXmlElement* resolversElem = new TiXmlElement("resolvers");
        BOOST_FOREACH(TiXmlElement* resolverElem, getAgentIDSequence(resolvers, useNameId, useUrlHref))
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

TiXmlElement* XMLFormat::getReceivedObject(const ReceivedObject& receivedObject)
{
    // TODO by/from definition uses url, example uses attribute "value"
    
    TiXmlElement* recvElem = new TiXmlElement( "received" );
    
    // As received objects are only used by envelopes, which have URLs without
    // href attribute, getURL must be called with useHref=false
    TiXmlElement* recvByElem = new TiXmlElement("received-by");
    recvByElem->LinkEndChild(getURL(receivedObject.getBy(), false));
    recvElem->LinkEndChild(recvByElem);
    
    if(receivedObject.getFrom() != "")
    {
        TiXmlElement* recvFromElem = new TiXmlElement("received-from");
        recvFromElem->LinkEndChild(getURL(receivedObject.getFrom(), false));
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

std::vector< TiXmlElement* > XMLFormat::getUserdefinedParameters(const UserdefinedParameterList& params)
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
