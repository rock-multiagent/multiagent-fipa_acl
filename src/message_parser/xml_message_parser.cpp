#include "xml_message_parser.h"
#include "xml_parser.h"
#include <fipa_acl/message_generator/message_format.h>
#include <base/Logging.hpp>

#include <boost/foreach.hpp>

namespace fipa {
namespace acl {

bool XMLMessageParser::parseData(const std::string& storage, ACLMessage& msg) const
{
    TiXmlDocument doc;
    
    // Load the string into XML doc
    const char* parseResult = doc.Parse(storage.c_str());
    if(parseResult != NULL)
    {
        // non-null means error
        std::cout << "Parsing message XML failed: " << parseResult << std::endl;
        LOG_WARN_S << "Parsing message XML failed: " << parseResult;
        return false;
    }
    
    // The main node (fipa-message)
    const TiXmlElement* messageElem = doc.FirstChildElement();
    
    if(messageElem->ValueStr() != "fipa-message")
    {
        std::cout << "Parsing error: XML main node not named 'fipa-message' but " << messageElem->ValueStr() << std::endl;
        LOG_WARN_S << "Parsing error: XML main node not named 'fipa-message' but " << messageElem->ValueStr();
        return false;
    }
    
    // Load communicative act attribute
    try
    {
        msg.setPerformative(messageElem->Attribute("act"));
    }
    catch(const std::exception& e)
    {
        // This indicates the performative is unknown
        std::cout << "Parsing error performative: " << e.what() << std::endl;
        LOG_WARN_S << "Parsing error performative: " << e.what();
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
    const std::string& name = elem->ValueStr();
    
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
            std::cout << "Parsing error receivers: " << e.what() << std::endl;
            LOG_WARN_S << "Parsing error receivers: " << e.what();
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
            std::cout << "Parsing error sender: " << e.what() << std::endl;
            LOG_WARN_S << "Parsing error sender: " << e.what();
            return false;
        }
    }
    else if(name == MessageFieldTxt[CONTENT])
    {
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            std::cout << "Parsing error: content is set but invalid" << std::endl;
            LOG_WARN_S << "Parsing error: content is set but invalid";
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
            std::cout << "Parsing error: language is set but invalid" << std::endl;
            LOG_WARN_S << "Parsing error: language is set but invalid";
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
            std::cout << "Parsing error: encoding is set but invalid" << std::endl;
            LOG_WARN_S << "Parsing error: encoding is set but invalid";
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
            std::cout << "Parsing error: ontology is set but invalid" << std::endl;
            LOG_WARN_S << "Parsing error: ontology is set but invalid";
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
            std::cout << "Parsing error: protocol is set but invalid" << std::endl;
            LOG_WARN_S << "Parsing error: protocol is set but invalid";
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
            std::cout << "Parsing error: reply_with is set but invalid" << std::endl;
            LOG_WARN_S << "Parsing error: reply_with is set but invalid";
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
            std::cout << "Parsing error: in_reply_to is set but invalid" << std::endl;
            LOG_WARN_S << "Parsing error: in_reply_to is set but invalid";
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
            std::cout << "Parsing error: reply_by is set but invalid" << std::endl;
            LOG_WARN_S << "Parsing error: reply_by is set but invalid";
            return false;
        }
        try
        {
            aclMsg.setReplyBy(XMLParser::strToDate(std::string(value)));
        }
        catch(std::exception& e)
        {
            std::cout << "Parsing error reply_by: " << e.what() << std::endl;
            LOG_WARN_S << "Parsing error reply_by: " << e.what();
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
            std::cout << "Parsing error reply_to: " << e.what() << std::endl;
            LOG_WARN_S << "Parsing error reply_to: " << e.what();
            return false;
        }
    }
    else if(name == MessageFieldTxt[CONVERSATION_ID])
    {
        // This overrides a conversation id set before
        const char* value = XMLParser::extractContentOrAttribute(elem);
        if(value == NULL)
        {
            std::cout << "Parsing error: conversation_id is set but invalid" << std::endl;
            LOG_WARN_S << "Parsing error: conversation_id is set but invalid";
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
            std::cout << "Parsing error user_def_param: " << e.what() << std::endl;
            LOG_WARN_S << "Parsing error user_def_param: " << e.what();
            return false;
        }
    }
    
    return true;
}

} // end namespace acl
} // end namespace fipa
