#include "xml_parser.h"
#include <stdexcept>

namespace fipa {
namespace acl {

const AgentID XMLParser::parseAgentID(const TiXmlElement* aidElem)
{
    if(aidElem->ValueStr() != "agent-identifier")
    {
        throw std::runtime_error("Parsing error: node not named 'agent-identifier' but " + aidElem->ValueStr());
    }
    
    AgentID aid;
    
    const TiXmlElement * pChild = aidElem->FirstChildElement();
    // The first child is the name
    aid.setName(parseName(pChild));
    
    for ( pChild = pChild->NextSiblingElement(); pChild != 0; pChild = pChild->NextSiblingElement()) 
    {
        // The remaining childs can be resolvers, addresses, or user-defined parameters
        const std::string& text = pChild->ValueStr();
        if(text == "addresses")
        {
            // Each child element is an URL
            const TiXmlElement * pInnerChild;
            for ( pInnerChild = pChild->FirstChildElement(); pInnerChild != 0; pInnerChild = pInnerChild->NextSiblingElement()) 
            {
                aid.addAddress(parseURL(pInnerChild));
            }
        }
        else if(text == "resolvers")
        {
            // It's a list of agent ids
            aid.setResolvers(parseAgentIDSequence(pChild));
        }
        else if(text.substr(2) == "X-")
        {
            aid.addUserdefParam(parseUserdefinedParameter(pChild));
        }
    }
    return aid;
}

const AgentIDList XMLParser::parseAgentIDSequence(const TiXmlElement* aidlElem)
{
    AgentIDList list;
    // All children are AgentIDs
    const TiXmlElement * pChild;
    for ( pChild = aidlElem->FirstChildElement(); pChild != 0; pChild = pChild->NextSiblingElement()) 
    {
        list.push_back(parseAgentID(pChild));
    }
    return list;
}

const base::Time XMLParser::parseDate(const TiXmlElement* dateElem)
{
    const char* date = dateElem->GetText();
    if(date == NULL)
    {
        throw std::runtime_error("ill-formed date element");
    }
    return strToDate(std::string(date));
}

const std::string XMLParser::parseName(const TiXmlElement* nameElem)
{
    if(nameElem->ValueStr() != "name")
    {
        throw std::runtime_error("Parsing error: node not named 'name' but " + nameElem->ValueStr());
    }
    
    // Name can be the value or in an "id" or "refid" attribute
    const char* p = nameElem->GetText();
    if(p != NULL)
    {
        return std::string(p);
    }
    p = nameElem->Attribute("id");
    if(p != NULL)
    {
        return std::string(p);
    }
    p = nameElem->Attribute("refid");
    if(p != NULL)
    {
        return std::string(p);
    }
    throw std::runtime_error("ill-formed name element");
}

const ReceivedObject XMLParser::parseReceivedObject(const TiXmlElement* receivedObjectElem)
{
    ReceivedObject ro;
    UserdefinedParameterList params;
    
    // Each element here can also have an "value" attribute
    
    const TiXmlElement * pChild = receivedObjectElem->FirstChildElement();
    // The first child is received-by
    const std::string* byValue = pChild->Attribute(std::string("value"));
    if(byValue != NULL)
    {
        ro.setBy(*byValue);
    }
    else
    {
        // Then it must be url child
        const std::string byUrl = parseURL(pChild->FirstChildElement());
        ro.setBy(byUrl);
    }
    
    for ( pChild = pChild->NextSiblingElement(); pChild != 0; pChild = pChild->NextSiblingElement()) 
    {
        // The remaining childs can be received-from, received-date, received-id, received-via, and user-defined
        const std::string& text = pChild->ValueStr();
        if(text == "received-from")
        {
            const std::string* fromValue = pChild->Attribute(std::string("value"));
            if(fromValue != NULL)
            {
                ro.setFrom(*fromValue);
            }
            else
            {
                // Then it must be url child
                const std::string fromUrl = parseURL(pChild->FirstChildElement());
                ro.setFrom(fromUrl);
            }
        }
        else if(text == "received-date")
        {
            const std::string* date = pChild->Attribute(std::string("value"));
            if(date == NULL)
            {
                throw std::runtime_error("ill-formed received-date element");
            }
            ro.setDate(strToDate(*date));
        }
        else if(text == "received-id")
        {
            const std::string* id = pChild->Attribute(std::string("value"));
            if(id == NULL)
            {
                throw std::runtime_error("ill-formed received-id element");
            }
            ro.setId(*id);
        }
        else if(text == "received-via")
        {
            const std::string* via = pChild->Attribute(std::string("value"));
            if(via == NULL)
            {
                throw std::runtime_error("ill-formed received-via element");
            }
            ro.setId(*via);
        }
        else if(text.substr(2) == "X-")
        {
            params.push_back(parseUserdefinedParameter(pChild));
        }
    }
    // Since there is no "addUserdefParam" in ReceivedObject
    ro.setUserdefinedParameters(params);
    
    return ro;
}

const std::string XMLParser::parseURL(const TiXmlElement* urlElem)
{
    if(urlElem->ValueStr() != "url")
    {
        throw std::runtime_error("Parsing error: node not named 'url' but " + urlElem->ValueStr());
    }
    
    // URL can be the value or in an href attribute
    const char* url = extractContentOrAttribute(urlElem);
    if(url == NULL)
    {
        throw std::runtime_error("ill-formed URL element");
    }
    return std::string(url);
}

const UserdefParam XMLParser::parseUserdefinedParameter(const TiXmlElement* paramElem)
{
    const char* href = paramElem->Attribute("href");
    const char* value = paramElem->GetText();
    // Throw an exception if either is NULL
    if(href == NULL || value == NULL)
    {
        throw std::runtime_error("ill-formed user defined parameter");
    }
    // cut "X-"
    return UserdefParam(std::string(href).substr(2), std::string(value));
    
}

const base::Time XMLParser::strToDate(const std::string& dateStr)
{
    // XXX this does not parse relative times (+/- as first char), or UTC Z at the end.
    try
    {
        return base::Time::fromString(dateStr, base::Time::Milliseconds, "%Y%m%dT%H%M%S");
    }
    catch(std::exception& e)
    {
        // Maybe JADE's "Z"?
        return base::Time::fromString(dateStr, base::Time::Milliseconds, "%Y%m%dZ%H%M%S");
    }
}

const char* XMLParser::extractContentOrAttribute(const TiXmlElement* elem, const char* identifier)
{
    const char* text = elem->GetText();
    if(text != NULL)
    {
        return text;
    }
    return elem->Attribute(identifier);
}

} // end namespace acl
} // end namespace fipa
