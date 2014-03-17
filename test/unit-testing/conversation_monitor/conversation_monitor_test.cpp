#ifndef CONVERSATION_MONITOR_TEST_H
#define CONVERSATION_MONITOR_TEST_H

// In general you should check boost testing website if you require some help
// http://www.boost.org/docs/libs/1.44_0/libs/test/html/index.html ( or more recent versions)
//
// This file intends to just briefly remind you of some of the possible setups

#include <boost/test/auto_unit_test.hpp>
#include <fipa_acl/conversation_monitor/conversation_monitor.h>
#include "utils.h"

BOOST_AUTO_TEST_SUITE(conversation_monitor_test_suite)

BOOST_AUTO_TEST_CASE(conversation_monitor_test)
{
        using namespace fipa::acl;
        std::string configurationPath = getProtocolPath();
        StateMachineFactory::setProtocolResourceDir(configurationPath);

        fipa::acl::AgentID agent("TEST_AGENT");

	ConversationMonitor* conversationMonitor = new ConversationMonitor(agent);

	std::string testContent0 = "testContent0";
	std::string testCommand0 = "test0";
        std::string testConversationID0 = "testConversationID0";
        std::string testProtocol0= "request";

	std::string testContent1 = "testContent1";
	std::string testCommand1 = "test1";
        std::string testConversationID1 = "testConversationID1";
        std::string testProtocol1= "testProtocol1";
	
	fipa::acl::ACLMessage fipaMsg0(fipa::acl::ACLMessage::REQUEST);
	fipa::acl::ACLMessage fipaMsg1(fipa::acl::ACLMessage::REQUEST);

	fipaMsg0.setContent(testContent0);
        fipaMsg0.setConversationID(testConversationID0);
        fipaMsg0.setProtocol(testProtocol0);

	fipaMsg1.setContent(testContent1);
        fipaMsg1.setConversationID(testConversationID1);
        fipaMsg1.setProtocol(testProtocol1);

        // Test conversation
        std::string owner = "OWNER_AGENT_NAME";
        Conversation conversation(owner);

        // Try again with false protocol - same conversation different protocol
        BOOST_REQUIRE_THROW(conversation.update(fipaMsg1),std::runtime_error);
        BOOST_CHECK(conversation.getProtocol() == "");

        BOOST_REQUIRE_NO_THROW(conversation.update(fipaMsg0));

        BOOST_CHECK(conversation.getConversationId() != "");
        BOOST_CHECK(conversation.getProtocol() == fipaMsg0.getProtocol());

        // False protocol
        BOOST_REQUIRE_THROW( Conversation conversation1(owner, fipaMsg1), std::runtime_error );
        fipaMsg1.setProtocol("request");
        // Use message with correct protocol
        Conversation conversation1(owner, fipaMsg1);
        BOOST_CHECK(conversation1.getProtocol() == fipaMsg1.getProtocol());

        // Test last message
        fipa::acl::ACLMessage lastMessage = conversation.getLastMessage();
        BOOST_CHECK_MESSAGE(fipaMsg0 == lastMessage, "content: " << fipaMsg0.getContent() << " vs. content: " << lastMessage.getContent());

        fipa::acl::ACLMessage lastMessage1 = conversation1.getLastMessage();
        BOOST_CHECK(fipaMsg1 == lastMessage1);
        BOOST_CHECK_MESSAGE(fipaMsg1 == lastMessage1, "content: " << fipaMsg1.getContent() << " vs. content: " << lastMessage1.getContent());

        // Test conversation monitor
	BOOST_REQUIRE_THROW( conversationMonitor->updateConversation(fipaMsg0), conversation::InvalidOperation );
	BOOST_REQUIRE_THROW( conversationMonitor->updateConversation(fipaMsg1), conversation::InvalidOperation );

        BOOST_REQUIRE( conversationMonitor->getOrCreateConversation(fipaMsg0.getConversationID()) );
        BOOST_REQUIRE( conversationMonitor->updateConversation(fipaMsg0) );

        ConversationPtr conversationInternalStart = conversationMonitor->startConversation("test-topic");
        BOOST_REQUIRE( conversationInternalStart );
        testConversationID1 = conversationInternalStart->getConversationId();
        fipaMsg1.setConversationID(testConversationID1);
        BOOST_REQUIRE( conversationMonitor->updateConversation(fipaMsg1) );

        // Check getActiveConversations
        std::vector<std::string> activeConversations = conversationMonitor->getActiveConversations();
        std::vector<std::string>::iterator it;

        it = std::find(activeConversations.begin(), activeConversations.end(), testConversationID0);
        BOOST_CHECK(it != activeConversations.end());

        it = std::find(activeConversations.begin(), activeConversations.end(), testConversationID1);
        BOOST_CHECK(it != activeConversations.end());

        // Check getConversation
        ConversationPtr conversationPtr = conversationMonitor->getConversation(testConversationID0);
        BOOST_CHECK(conversationPtr != 0);

        BOOST_CHECK(conversationPtr->getLastMessage() == fipaMsg0);
        
        std::string conversationId = conversationPtr->getConversationId();
        BOOST_CHECK_MESSAGE(conversationMonitor->removeConversation(conversationId), "Removed " + conversationId);

        BOOST_CHECK(conversationMonitor->getConversation(conversationId) == 0);
        conversationMonitor->cleanup();

        {
	    fipa::acl::ACLMessage fipaMsg0(fipa::acl::ACLMessage::REQUEST);
	    fipa::acl::ACLMessage fipaMsg1(fipa::acl::ACLMessage::REFUSE);

	    fipaMsg0.setContent("testContent0");
            fipaMsg0.setConversationID("test-conv");
            fipaMsg0.setProtocol("request");
            fipaMsg0.setSender( agent );
            fipaMsg0.addReceiver(AgentID("receiver"));

	    fipaMsg1.setContent("testContent1");
            fipaMsg1.setConversationID("test-conv");
            fipaMsg1.setProtocol("request");
            fipaMsg1.setSender( AgentID("receiver") );
            fipaMsg1.addReceiver( agent );

            BOOST_REQUIRE_NO_THROW(conversationMonitor->getOrCreateConversation(fipaMsg0.getConversationID()));
            BOOST_REQUIRE_NO_THROW(conversationMonitor->updateConversation(fipaMsg0));
            BOOST_REQUIRE_NO_THROW(conversationMonitor->updateConversation(fipaMsg1));

            ConversationPtr testConversation = conversationMonitor->getConversation(fipaMsg0.getConversationID());
            BOOST_REQUIRE(testConversation);
            BOOST_REQUIRE(testConversation->hasEnded());

            conversationMonitor->cleanup();
            BOOST_CHECK(conversationMonitor->getConversation(fipaMsg0.getConversationID()) == 0);
        }
}


BOOST_AUTO_TEST_SUITE_END()
#endif

