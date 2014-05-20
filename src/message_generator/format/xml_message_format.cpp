#include "xml_message_format.h"
#include "xml_format.h"

#include <boost/foreach.hpp>

namespace fipa {
namespace acl {


std::string XMLMessageFormat::apply(const ACLMessage& aclMsg) const
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
    
    TiXmlElement * messageElem = new TiXmlElement( "fipa-message" );
    // Apply communictaive act and conversation id
    messageElem->SetAttribute("act", aclMsg.getPerformative());
    messageElem->SetAttribute("conversation-id", aclMsg.getConversationID());
    doc.LinkEndChild( messageElem );
    
    BOOST_FOREACH(TiXmlElement* elem, getParameters(aclMsg))
    {
        messageElem->LinkEndChild(elem);
    }
    
    TiXmlPrinter printer;
    printer.SetIndent("");
    printer.SetLineBreak("");

    doc.Accept( &printer );
    
    return printer.Str();
}

std::vector<TiXmlElement*> XMLMessageFormat::getParameters(const ACLMessage& aclMsg) const
{
    using namespace MessageField;
    std::vector<TiXmlElement*> vec;
    
    const AgentIDList& receivers = aclMsg.getAllReceivers();
    if(!receivers.empty())
    {
        TiXmlElement* receiversElem = new TiXmlElement(MessageFieldTxt[RECEIVER]);
        // Message uses name id and url href
        BOOST_FOREACH(TiXmlElement* elem, XMLFormat::getAgentIDSequence(receivers, true, true))
        {
            receiversElem->LinkEndChild(elem);
        }
        vec.push_back(receiversElem);
    }
    
    const AgentID& sender = aclMsg.getSender();
    if(sender.isValid())
    {
        TiXmlElement* senderElem = new TiXmlElement(MessageFieldTxt[SENDER]);
        // Message uses name id and url href
        senderElem->LinkEndChild(XMLFormat::getAgentID(sender, true, true));
        vec.push_back(senderElem);
    }

    if(!aclMsg.getContentPtr()->empty())
    {
        TiXmlElement* contentElem = new TiXmlElement(MessageFieldTxt[CONTENT]);
        contentElem->LinkEndChild(new TiXmlText(*aclMsg.getContentPtr()));
        vec.push_back(contentElem);
    }
    
    const std::string& language = aclMsg.getLanguage();
    if(!language.empty())
    {
        TiXmlElement* languageElem = new TiXmlElement(MessageFieldTxt[LANGUAGE]);
        languageElem->LinkEndChild(new TiXmlText(language));
        vec.push_back(languageElem);
    }

    const std::string& encoding = aclMsg.getEncoding();
    if(!encoding.empty())
    {
        TiXmlElement* encodingElem = new TiXmlElement(MessageFieldTxt[ENCODING]);
        encodingElem->LinkEndChild(new TiXmlText(encoding));
        vec.push_back(encodingElem);
    }

    const std::string& ontology = aclMsg.getOntology();
    if(!ontology.empty())
    {
        TiXmlElement* ontologyElem = new TiXmlElement(MessageFieldTxt[ONTOLOGY]);
        ontologyElem->LinkEndChild(new TiXmlText(ontology));
        vec.push_back(ontologyElem);
    }
    
    const std::string& protocol = aclMsg.getProtocol();
    if(!protocol.empty())
    {
        TiXmlElement* protocolElem = new TiXmlElement(MessageFieldTxt[PROTOCOL]);
        protocolElem->LinkEndChild(new TiXmlText(protocol));
        vec.push_back(protocolElem);
    }

    const std::string& replyWith = aclMsg.getReplyWith();
    if(!replyWith.empty())
    {
        TiXmlElement* replyWithElem = new TiXmlElement(MessageFieldTxt[REPLY_WITH]);
        replyWithElem->LinkEndChild(new TiXmlText(replyWith));
        vec.push_back(replyWithElem);
    }

    const std::string& inReplyTo = aclMsg.getInReplyTo();
    if(!inReplyTo.empty())
    {
        TiXmlElement* inReplyToElem = new TiXmlElement(MessageFieldTxt[IN_REPLY_TO]);
        inReplyToElem->LinkEndChild(new TiXmlText(inReplyTo));
        vec.push_back(inReplyToElem);
    }

    base::Time replyBy = aclMsg.getReplyBy();
    if(!replyBy.isNull())
    {
        TiXmlElement* replyByElem = new TiXmlElement(MessageFieldTxt[REPLY_BY]);
        replyByElem->SetAttribute("time", XMLFormat::dateToStr(replyBy));
        vec.push_back(replyByElem);
    }

    const AgentIDList& replyTo = aclMsg.getAllReplyTo();
    if(!replyTo.empty())
    {
        TiXmlElement* replyToElem = new TiXmlElement(MessageFieldTxt[REPLY_TO]);
        // Message uses name id and url href
        BOOST_FOREACH(TiXmlElement* elem, XMLFormat::getAgentIDSequence(replyTo, true, true))
        {
            replyToElem->LinkEndChild(elem);
        }
        vec.push_back(replyToElem);
    }

    const std::string& conversationId = aclMsg.getConversationID();
    if(!conversationId.empty())
    {
        TiXmlElement* conversationIdElem = new TiXmlElement(MessageFieldTxt[CONVERSATION_ID]);
        conversationIdElem->LinkEndChild(new TiXmlText(conversationId));
        vec.push_back(conversationIdElem);
    }

    BOOST_FOREACH(TiXmlElement* elem, XMLFormat::getUserdefinedParameters(aclMsg.getUserdefParams()))
    {
        vec.push_back(elem);
    }
    
    return vec;
}

} // end namespace acl
} // end namespace fipa
