#include "string_message_format.h"
#include "string_format.h"

namespace fipa {
namespace acl {


std::string StringMessageFormat::apply(const ACLMessage& aclMsg) const
{
    using namespace MessageField;

    std::string msg = "(";
//    msg += getMessageType();
//    msg += getMessageParameter();

    const AgentID& sender = aclMsg.getSender();
    if(sender.isValid())
    {
        msg += ":" + MessageFieldTxt[SENDER] + StringFormat::getAgentIdentifier(sender);
    }

    const AgentIDList& receivers = aclMsg.getAllReceivers();
    if(!receivers.empty())
    {
        msg += ":" + MessageFieldTxt[RECEIVER] + StringFormat::getAgentIdentifierSet(receivers);
    }

    if(!aclMsg.getContentPtr()->empty())
    {
        msg += ":" + MessageFieldTxt[CONTENT] + *aclMsg.getContentPtr();
    }

    const std::string& replyWith = aclMsg.getReplyWith();
    if(!replyWith.empty())
    {
        msg += ":" + MessageFieldTxt[REPLY_WITH] + StringFormat::getExpression(replyWith);
    }


    //base::Time replyBy = aclMsg.getReplyBy();
    //if(!replyBy.isNull())
    //{
    //    msg += ":" + MessageFieldTxt[REPLY_WITH + " " + replyWith + " ";
    //}

    const AgentIDList& replyTo = aclMsg.getAllReplyTo();
    if(!replyTo.empty())
    {
        msg += ":" + MessageFieldTxt[IN_REPLY_TO] + StringFormat::getAgentIdentifierSet(replyTo);
    }

    const std::string& language = aclMsg.getLanguage();
    if(!language.empty())
    {
        msg += ":" + MessageFieldTxt[LANGUAGE] + StringFormat::getExpression(language);
    }

    const std::string& encoding = aclMsg.getEncoding();
    if(!encoding.empty())
    {
        msg += ":" + MessageFieldTxt[ENCODING] + StringFormat::getExpression(encoding);
    }

    const std::string& ontology = aclMsg.getOntology();
    if(!ontology.empty())
    {
        msg += ":" + MessageFieldTxt[ONTOLOGY] + StringFormat::getExpression(ontology);
    }

    const std::string& protocol = aclMsg.getProtocol();
    if(!protocol.empty())
    {
        msg += ":" + MessageFieldTxt[PROTOCOL] + StringFormat::getWord(protocol);
    }

    const std::string& conversationId = aclMsg.getConversationID();
    if(!conversationId.empty())
    {
        msg += ":" + MessageFieldTxt[CONVERSATION_ID] + StringFormat::getExpression(conversationId);
    }
    msg += ")";

    return msg;
}


} // end namespace acl
} // end namespace fipa
