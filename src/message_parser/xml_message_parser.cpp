#include "xml_message_parser.h"
#include "xml_parser.h"
#include <fipa_acl/message_generator/message_format.h>

#include <boost/foreach.hpp>

namespace fipa {
namespace acl {

bool XMLMessageParser::parseData(const std::string& storage, ACLMessage& msg) const
{
    TiXmlDocument doc;
    
    // Load the string into XML doc
    const char* parseResult = doc.Parse(storage.c_str(), 0);  // FIXME encoding?
    if(parseResult != NULL)
    {
        // non-null means error
        return false;
    }
    
    // The main node (fipa-message)
    const TiXmlElement* messageElem = doc.FirstChildElement();
    
    // Load communicative act attribute
    try
    {
        msg.setPerformative(messageElem->Attribute("act"));
    }
    catch(const std::exception& e)
    {
        // This indicates the performative is unknown
        return false;
    }
    
    // If the conversation id is here, save it
    const std::string* convID = messageElem->Attribute(std::string("conversation-id"));
    if(convID != NULL)
    {
        msg.setConversationID(*convID);
    }
    
    // Parse all child elements
    const TiXmlElement * pChild;
    for ( pChild = messageElem->FirstChildElement(); pChild != 0; pChild = pChild->NextSiblingElement()) 
    {
        if(!parseParameter(msg, pChild))
        {
            return false;
        }
    }
    
    return true;
}

bool XMLMessageParser::parseParameter(ACLMessage& aclMsg, const TiXmlElement* elem) const
{
    using namespace MessageField;
    
    const char* nameP = elem->GetText();
    if(nameP == NULL)
    {
        // not FIPA compliant
        return false;
    }
    const std::string name (nameP);
    
    if(name == MessageFieldTxt[RECEIVER])
    {
        // This is the only field where FIPA allows multiple occurences, so we must not override anything
        // Therefore, we use addReceiver instead of setAllReceivers
        // AgentIDList
        try
        {
            const AgentIDList aidl = XMLParser::parseAgentIDSequence(elem);
            BOOST_FOREACH(AgentID aid, aidl)
            {
                aclMsg.addReceiver(aid);
            }
        }
        catch(std::exception& e)
        {
            return false;
        }
    }
    else if(name == MessageFieldTxt[SENDER])
    {
        // AgentID
        try
        {
            aclMsg.setSender(XMLParser::parseAgentID(elem));
        }
        catch(std::exception& e)
        {
            return false;
        }
    }
    else if(name == MessageFieldTxt[CONTENT])
    {
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            return false;
        }
        aclMsg.setContent(std::string(value));
        return true;
    }
    else if(name == MessageFieldTxt[LANGUAGE])
    {
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            return false;
        }
        aclMsg.setLanguage(std::string(value));
        return true;
    }
    else if(name == MessageFieldTxt[ENCODING])
    {
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            return false;
        }
        aclMsg.setEncoding(std::string(value));
        return true;
    }
    else if(name == MessageFieldTxt[ONTOLOGY])
    {
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            return false;
        }
        aclMsg.setOntology(std::string(value));
        return true;
    }
    else if(name == MessageFieldTxt[PROTOCOL])
    {
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            return false;
        }
        aclMsg.setProtocol(std::string(value));
        return true;
    }
    else if(name == MessageFieldTxt[REPLY_WITH])
    {
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            return false;
        }
        aclMsg.setReplyWith(std::string(value));
        return true;
    }
    else if(name == MessageFieldTxt[IN_REPLY_TO])
    {
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            return false;
        }
        aclMsg.setInReplyTo(std::string(value));
        return true;
    }
    else if(name == MessageFieldTxt[REPLY_BY])
    {
        // Time. The value is in the attribute "time"
        const char* value = elem->Attribute("time");
        if(value == NULL)
        {
            return false;
        }
        try
        {
            aclMsg.setReplyBy(XMLParser::strToDate(std::string(value)));
        }
        catch(std::exception& e)
        {
            return false;
        }
    }
    else if(name == MessageFieldTxt[REPLY_TO])
    {
        // AgentIDList
        try
        {
            aclMsg.setAllReplyTo(XMLParser::parseAgentIDSequence(elem));
        }
        catch(std::exception& e)
        {
            return false;
        }
    }
    else if(name == MessageFieldTxt[CONVERSATION_ID])
    {
        // This overrides a conversation id set before
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            return false;
        }
        aclMsg.setConversationID(std::string(value));
        return true;
    }
    else if(name.substr(2) == "X-")
    {
        // It's a user defined parameter
        try
        {
            aclMsg.addUserdefParam(XMLParser::parseUserdefinedParameter(elem));
        }
        catch(std::exception& e)
        {
            return false;
        }
    }
    
    return true;
}

} // end namespace acl
} // end namespace fipa
