#ifndef _FIPA_ACL_MESSAGE_TEST_H_
#define _FIPA_ACL_MESSAGE_TEST_H_

#include <fipa_acl/bitefficient_message.h>

BOOST_AUTO_TEST_SUITE(fipa_message_test_suite)

BOOST_AUTO_TEST_CASE(message_test)
{
    using namespace fipa::acl;

    ACLMessageOutputParser outputParser;
    ACLMessage msg("inform");
    AgentID origin("proxy");
    AgentID receiver("crex_0_CREXCORE");

    msg.setSender(origin);
    msg.addReceiver(receiver);
    msg.addReplyTo(origin);
    msg.setPerformative(fipa::acl::ACLMessage::REQUEST);
    msg.setProtocol(std::string("RIMRES"));
    msg.setLanguage(std::string("test language"));
    msg.setEncoding(std::string("test encoding"));
    msg.setOntology(std::string("test ontology"));
    msg.setReplyWith(std::string("test reply_with"));
    msg.setReplyBy1(std::string("2010-12-23T12:00:37:980"));
    msg.setConversationID(std::string("test conversationID"));
    msg.setContent("test content");

    std::vector<AgentID> agents = msg.getAllReceivers();
    BOOST_CHECK_MESSAGE(agents.size() == 1, "Original msg: receiver agent size is one");

    outputParser.setMessage(msg);
    std::string encodedMsg = outputParser.getBitMessage();

    MessageParser inputParser;
    ACLMessage outputMsg;

    bool success = inputParser.parseData(encodedMsg, outputMsg);
    BOOST_CHECK_MESSAGE(success, "Parsing of encoded message succeeded");

    BOOST_ASSERT(outputMsg.getPerformative() == PerformativeTxt[REQUEST]);
    BOOST_ASSERT(outputMsg.getSender() == msg.getSender());


    BOOST_ASSERT(outputMsg.getAllReceivers() == msg.getAllReceivers());
    agents = outputMsg.getAllReceivers();
    BOOST_ASSERT(agents.size() == 1);

    std::vector<AgentID>::iterator it = agents.begin();
    for(; it != agents.end(); ++it)
    {
        BOOST_ASSERT(it->getName() == receiver.getName());
    }
    
    BOOST_ASSERT(outputMsg.getProtocol() == msg.getProtocol());
    BOOST_ASSERT(outputMsg.getLanguage() == msg.getLanguage());
    BOOST_ASSERT(outputMsg.getEncoding() == msg.getEncoding());
    BOOST_ASSERT(outputMsg.getOntology() == msg.getOntology());
    BOOST_ASSERT(outputMsg.getReplyWith() == msg.getReplyWith());
    BOOST_ASSERT(outputMsg.getReplyBy() == msg.getReplyBy());
    BOOST_ASSERT(outputMsg.getConversationID() == msg.getConversationID());
    BOOST_ASSERT(outputMsg.getContent() == msg.getContent());
}

BOOST_AUTO_TEST_SUITE_END()

#endif 

