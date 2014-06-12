#ifndef CONVERSATION_TEST_H
#define CONVERSATION_TEST_H


// In general you should check boost testing website if you require some help
// http://www.boost.org/docs/libs/1.44_0/libs/test/html/index.html ( or more recent versions)
//
// This file intends to just briefly remind you of some of the possible setups

#include <boost/test/auto_unit_test.hpp>
#include <fipa_acl/conversation_monitor/conversation.h>
#include "utils.h"

BOOST_AUTO_TEST_SUITE(conversation_test_suite)

BOOST_AUTO_TEST_CASE(brokering_positive_test)
{
    std::cout << "brokering_positive_test" << std::endl;
    using namespace fipa::acl;
    std::string configurationPath = getProtocolPath();
    StateMachineFactory::setProtocolResourceDir(configurationPath);

    AgentID initiator ("initiator");
    AgentID broker ("broker");
    AgentID serviceProvider0 ("serviceProvider0");
    AgentID serviceProvider1 ("serviceProvider1");
    AgentID serviceProvider2 ("serviceProvider2");
    
    // Test 1: 1x inform
    Conversation c1 (initiator.getName(), "conv");
    // I -> B (proxy)
    ACLMessage msg1_1(ACLMessage::PROXY);
    msg1_1.setConversationID("conv");
    msg1_1.setSender(initiator);
    msg1_1.addReceiver(broker);
    msg1_1.setProtocol("brokering"); //XXX this should be "fipa-brokering"
    std::cout << "C1 MSG1" << std::endl;
    c1.update(msg1_1);
    // B -> I (agree)
    ACLMessage msg1_2(ACLMessage::AGREE);
    msg1_2.setConversationID("conv");
    msg1_2.setSender(broker);
    msg1_2.addReceiver(initiator);
    msg1_2.setProtocol("brokering");
    std::cout << "C1 MSG2" << std::endl;
    c1.update(msg1_2);
    // Set number of planned sub conversations
    c1.setNumberOfSubConversations(1);
    // MSG3 deleted!
    // B -> S (inform)
    ACLMessage msg1_4(ACLMessage::INFORM);
    msg1_4.setConversationID("conv");
    msg1_4.setSender(broker);
    msg1_4.addReceiver(serviceProvider0);
    msg1_4.setProtocol("inform");
    std::cout << "C1 MSG4" << std::endl;
    c1.update(msg1_4);
    BOOST_CHECK_MESSAGE(!c1.hasEnded(), "Test 1: conversation ended too early after msg 4.");
    // B -> I (inform)
    ACLMessage msg1_4a(ACLMessage::INFORM);
    msg1_4a.setConversationID("conv");
    msg1_4a.setSender(broker);
    msg1_4a.addReceiver(initiator);
    msg1_4a.setProtocol("brokering");
    std::cout << "C1 MSG4a" << std::endl;
    c1.update(msg1_4a);
    // The conversation should have ended
    BOOST_CHECK_MESSAGE(c1.hasEnded(), "Test 1: conversation did not end.");
    
    // Test2: 2x inform
    Conversation c2 (initiator.getName(), "conv");
    c2.update(msg1_1);
    c2.update(msg1_2);
    // Set number of planned sub conversations
    c2.setNumberOfSubConversations(2);
    c2.update(msg1_4);
    BOOST_CHECK_MESSAGE(!c2.hasEnded(), "Test 2: conversation ended too early after msg 4.");
    // B -> S1 (inform)
    ACLMessage msg2_5(ACLMessage::INFORM);
    msg2_5.setConversationID("conv");
    msg2_5.setSender(broker);
    msg2_5.addReceiver(serviceProvider1);
    msg2_5.setProtocol("inform");
    std::cout << "C2 MSG5" << std::endl;
    c2.update(msg2_5);
    c2.update(msg1_4a);
    // The conversation should have ended
    BOOST_CHECK_MESSAGE(c2.hasEnded(), "Test 2: conversation did not end.");
    
    // Test3: 1x request
    Conversation c3 (initiator.getName(), "conv");
    c3.update(msg1_1);
    c3.update(msg1_2);
    // Set number of planned sub conversations
    c3.setNumberOfSubConversations(1);
    // B -> S (request)
    ACLMessage msg3_4(ACLMessage::REQUEST);
    msg3_4.setConversationID("conv");
    msg3_4.setSender(broker);
    msg3_4.addReceiver(serviceProvider0);
    msg3_4.setProtocol("request");
    std::cout << "C3 MSG4" << std::endl;
    c3.update(msg3_4);
    BOOST_CHECK_MESSAGE(!c3.hasEnded(), "Test 3: conversation ended too early after msg 4.");
    // S -> B (agree)
    ACLMessage msg3_5(ACLMessage::AGREE);
    msg3_5.setConversationID("conv");
    msg3_5.setSender(serviceProvider0);
    msg3_5.addReceiver(broker);
    msg3_5.setProtocol("request");
    std::cout << "C3 MSG5" << std::endl;
    c3.update(msg3_5);
    c3.update(msg1_4a);
    BOOST_CHECK_MESSAGE(!c3.hasEnded(), "Test 3: conversation ended too early after msg 5.");
    // S -> B (inform)
    ACLMessage msg3_6(ACLMessage::INFORM);
    msg3_6.setConversationID("conv");
    msg3_6.setSender(serviceProvider0);
    msg3_6.addReceiver(broker);
    msg3_6.setProtocol("request");
    std::cout << "C3 MSG6" << std::endl;
    c3.update(msg3_6);
    BOOST_CHECK_MESSAGE(!c3.hasEnded(), "Test 3: conversation ended too early after msg 6.");
    // B -> I (inform)
    ACLMessage msg3_7(ACLMessage::INFORM);
    msg3_7.setConversationID("conv");
    msg3_7.setSender(broker);
    msg3_7.addReceiver(initiator);
    msg3_7.setProtocol("brokering");
    std::cout << "C3 MSG7" << std::endl;
    c3.update(msg3_7);
    BOOST_CHECK_MESSAGE(c3.hasEnded(), "Test 3: conversation did not end.");
    
    // Test 4: 3x request
    Conversation c4 (initiator.getName(), "conv");
    c4.update(msg1_1);
    c4.update(msg1_2);
    // Set number of planned sub conversations
    c4.setNumberOfSubConversations(3);
    c4.update(msg3_4);
    c4.update(msg3_5);
    c4.update(msg3_6);
    // Now start another subprotocol after the first has ended
    // B -> S1 (request)
    ACLMessage msg4_7(ACLMessage::REQUEST);
    msg4_7.setConversationID("conv");
    msg4_7.setSender(broker);
    msg4_7.addReceiver(serviceProvider1);
    msg4_7.setProtocol("request");
    std::cout << "C4 MSG7" << std::endl;
    c4.update(msg4_7);
    // S1 -> B (agree)
    ACLMessage msg4_8(ACLMessage::AGREE);
    msg4_8.setConversationID("conv");
    msg4_8.setSender(serviceProvider1);
    msg4_8.addReceiver(broker);
    msg4_8.setProtocol("request");
    std::cout << "C4 MSG8" << std::endl;
    c4.update(msg4_8);
    // And yet another while the last is still running
    // B -> S2 (request)
    ACLMessage msg4_9(ACLMessage::REQUEST);
    msg4_9.setConversationID("conv");
    msg4_9.setSender(broker);
    msg4_9.addReceiver(serviceProvider2);
    msg4_9.setProtocol("request");
    std::cout << "C4 MSG9" << std::endl;
    c4.update(msg4_9);
    // S2 -> B (agree)
    ACLMessage msg4_10(ACLMessage::AGREE);
    msg4_10.setConversationID("conv");
    msg4_10.setSender(serviceProvider2);
    msg4_10.addReceiver(broker);
    msg4_10.setProtocol("request");
    std::cout << "C4 MSG10" << std::endl;
    c4.update(msg4_10);
    c4.update(msg1_4a);
    // And end both
    // S1 -> B (inform)
    ACLMessage msg4_11(ACLMessage::INFORM);
    msg4_11.setConversationID("conv");
    msg4_11.setSender(serviceProvider1);
    msg4_11.addReceiver(broker);
    msg4_11.setProtocol("request");
    std::cout << "C4 MSG11" << std::endl;
    c4.update(msg4_11);
    // S2 -> B (inform)
    ACLMessage msg4_12(ACLMessage::INFORM);
    msg4_12.setConversationID("conv");
    msg4_12.setSender(serviceProvider2);
    msg4_12.addReceiver(broker);
    msg4_12.setProtocol("request");
    std::cout << "C4 MSG12" << std::endl;
    c4.update(msg4_12);
    BOOST_CHECK_MESSAGE(!c4.hasEnded(), "Test 4: conversation ended too early after msg 12.");
    // And end the whole conversation
    c4.update(msg3_7);
    BOOST_CHECK_MESSAGE(c4.hasEnded(), "Test 4: conversation did not end.");
    
    // Test 5 failure-no-match
    Conversation c5 (initiator.getName(), "conv");
    c5.update(msg1_1);
    c5.update(msg1_2);
    // B -> I (failure)
    ACLMessage msg5_4(ACLMessage::FAILURE);
    msg5_4.setConversationID("conv");
    msg5_4.setSender(broker);
    msg5_4.addReceiver(initiator);
    msg5_4.setProtocol("brokering");
    std::cout << "C5 MSG4" << std::endl;
    c5.update(msg5_4);
    BOOST_CHECK_MESSAGE(c5.hasEnded(), "Test 5: conversation did not end.");
    
    // Test 6 failure-proxy
    Conversation c6 (initiator.getName(), "conv");
    c6.update(msg1_1);
    c6.update(msg1_2);
    // Set number of planned sub conversations
    c6.setNumberOfSubConversations(1);
    c6.update(msg3_4);
    // S -> B (refuse)
    ACLMessage msg6_5(ACLMessage::REFUSE);
    msg6_5.setConversationID("conv");
    msg6_5.setSender(serviceProvider0);
    msg6_5.addReceiver(broker);
    msg6_5.setProtocol("request");
    std::cout << "C6 MSG5" << std::endl;
    c6.update(msg6_5);
    BOOST_CHECK_MESSAGE(!c6.hasEnded(), "Test 6: conversation ended too early after msg 5.");
    c6.update(msg5_4);
    BOOST_CHECK_MESSAGE(c6.hasEnded(), "Test 6: conversation did not end.");
    
    // Test 7 failure-brokering
    Conversation c7 (initiator.getName(), "conv");
    c7.update(msg1_1);
    c7.update(msg1_2);
    // Set number of planned sub conversations
    c7.setNumberOfSubConversations(1);
    c7.update(msg3_4);
    c7.update(msg3_5);
    c7.update(msg1_4a);
    // S -> B (failure)
    ACLMessage msg7_6(ACLMessage::FAILURE);
    msg7_6.setConversationID("conv");
    msg7_6.setSender(serviceProvider0);
    msg7_6.addReceiver(broker);
    msg7_6.setProtocol("request");
    std::cout << "C7 MSG6" << std::endl;
    c7.update(msg7_6);
    BOOST_CHECK_MESSAGE(!c7.hasEnded(), "Test 7: conversation ended too early after msg 5.");
    c7.update(msg5_4);
    BOOST_CHECK_MESSAGE(c7.hasEnded(), "Test 7: conversation did not end.");
}

BOOST_AUTO_TEST_CASE(brokering_negative_test)
{
    std::cout << "brokering_negative_test" << std::endl;
    using namespace fipa::acl;
    AgentID initiator ("initiator");
    AgentID broker ("broker");
    AgentID serviceProvider0 ("serviceProvider0");
    AgentID serviceProvider1 ("serviceProvider1");
    
    // Test 1 subprotocol not started
    Conversation c1 (initiator.getName(), "conv");
    // I -> B (proxy)
    ACLMessage msg1_1(ACLMessage::PROXY);
    msg1_1.setConversationID("conv");
    msg1_1.setSender(initiator);
    msg1_1.addReceiver(broker);
    msg1_1.setProtocol("brokering");
    std::cout << "C1 MSG1" << std::endl;
    c1.update(msg1_1);
    // B -> I (agree)
    ACLMessage msg1_2(ACLMessage::AGREE);
    msg1_2.setConversationID("conv");
    msg1_2.setSender(broker);
    msg1_2.addReceiver(initiator);
    msg1_2.setProtocol("brokering");
    std::cout << "C1 MSG2" << std::endl;
    c1.update(msg1_2);
    // Set number of planned sub conversations
    c1.setNumberOfSubConversations(1);
    // Now a subprotocol should be started, but it is not
    // B -> I (inform)
    ACLMessage msg1_3(ACLMessage::INFORM);
    msg1_3.setConversationID("conv");
    msg1_3.setSender(broker);
    msg1_3.addReceiver(initiator);
    msg1_3.setProtocol("brokering");
    std::cout << "C1 MSG3" << std::endl;
    c1.update(msg1_3);
    // B -> I (inform)
    ACLMessage msg1_4(ACLMessage::INFORM);
    msg1_4.setConversationID("conv");
    msg1_4.setSender(broker);
    msg1_4.addReceiver(initiator);
    msg1_4.setProtocol("brokering");
    std::cout << "C1 MSG4" << std::endl;
    // This would be an proxied reply message, which is not allowed if no protocol was started.
    BOOST_REQUIRE_THROW( c1.update(msg1_4), conversation::ProtocolException );
    
    // Test 2 multiple forwarded replies
    Conversation c2 (initiator.getName(), "conv");
    c2.update(msg1_1);
    c2.update(msg1_2);
    // Set number of planned sub conversations
    c2.setNumberOfSubConversations(1);
    // B -> S (request)
    ACLMessage msg2_4(ACLMessage::REQUEST);
    msg2_4.setConversationID("conv");
    msg2_4.setSender(broker);
    msg2_4.addReceiver(serviceProvider0);
    msg2_4.setProtocol("request");
    std::cout << "C2 MSG4" << std::endl;
    c2.update(msg2_4);
    // S -> B (agree)
    ACLMessage msg2_5(ACLMessage::AGREE);
    msg2_5.setConversationID("conv");
    msg2_5.setSender(serviceProvider0);
    msg2_5.addReceiver(broker);
    msg2_5.setProtocol("request");
    std::cout << "C2 MSG5" << std::endl;
    c2.update(msg2_5);
    c2.update(msg1_3);
    // Forward agree
    // B -> I (agree)
    ACLMessage msg2_6(ACLMessage::AGREE);
    msg2_6.setConversationID("conv");
    msg2_6.setSender(broker);
    msg2_6.addReceiver(initiator);
    msg2_6.setProtocol("brokering");
    std::cout << "C2 MSG6" << std::endl;
    c2.update(msg2_6);
    // S -> B (inform)
    ACLMessage msg2_7(ACLMessage::INFORM);
    msg2_7.setConversationID("conv");
    msg2_7.setSender(serviceProvider0);
    msg2_7.addReceiver(broker);
    msg2_7.setProtocol("request");
    std::cout << "C2 MSG7" << std::endl;
    c2.update(msg2_7);
    // Forward inform, that is one too many
    // B -> I (inform)
    ACLMessage msg2_8(ACLMessage::INFORM);
    msg2_8.setConversationID("conv");
    msg2_8.setSender(broker);
    msg2_8.addReceiver(initiator);
    msg2_8.setProtocol("brokering");
    std::cout << "C2 MSG8" << std::endl;
    BOOST_REQUIRE_THROW( c2.update(msg2_8), conversation::ProtocolException );
    
    // Test 3 reply for proxied inform
    Conversation c3 (initiator.getName(), "conv");
    c3.update(msg1_1);
    c3.update(msg1_2);
    // Set number of planned sub conversations
    c3.setNumberOfSubConversations(1);
    // B -> S (inform)
    ACLMessage msg3_4(ACLMessage::INFORM);
    msg3_4.setConversationID("conv");
    msg3_4.setSender(broker);
    msg3_4.addReceiver(serviceProvider0);
    msg3_4.setProtocol("inform");
    std::cout << "C3 MSG4" << std::endl;
    c3.update(msg3_4);
    c3.update(msg1_3);
    // Forward inform, which is undesired
    ACLMessage msg3_5(ACLMessage::INFORM);
    msg3_5.setConversationID("conv");
    msg3_5.setSender(broker);
    msg3_5.addReceiver(initiator);
    msg3_5.setProtocol("brokering");
    std::cout << "C3 MSG5" << std::endl;
    BOOST_REQUIRE_THROW( c2.update(msg3_5), conversation::ProtocolException );
    
    // Test 4 forwarded reply to the wrong person
    Conversation c4 (initiator.getName(), "conv");
    c4.update(msg1_1);
    c4.update(msg1_2);
    // Set number of planned sub conversations
    c4.setNumberOfSubConversations(1);
    c4.update(msg2_4);
    c4.update(msg2_5);
    c4.update(msg1_3);
    // msg 6 omitted!
    c4.update(msg2_7);
    // Forward inform to the wrong person
    // B -> I (inform)
    ACLMessage msg4_8(ACLMessage::INFORM);
    msg4_8.setConversationID("conv");
    msg4_8.setSender(broker);
    msg4_8.addReceiver(serviceProvider1);
    msg4_8.setProtocol("brokering");
    std::cout << "C4 MSG8" << std::endl;
    BOOST_REQUIRE_THROW( c2.update(msg4_8), conversation::ProtocolException );
    
    // Test 5 subprotocol started too early
    Conversation c5 (initiator.getName(), "conv");
    c5.update(msg1_1);
    BOOST_REQUIRE_THROW( c5.update(msg2_4), conversation::ProtocolException );
}

BOOST_AUTO_TEST_CASE(with_single_subprotocol_and_not_proxied_test)
{
    std::cout << "with_single_subprotocol_and_not_proxied_test" << std::endl;
    using namespace fipa::acl;
    AgentID initiator ("initiator");
    AgentID propagator ("propagator");
    AgentID serviceProvider0 ("serviceProvider0");
    AgentID serviceProvider1 ("serviceProvider1");
    
    // Test 1 (negative) subprotocol started twice
    Conversation c1 (initiator.getName(), "conv");
    // I -> P (propagate)
    ACLMessage msg1_1(ACLMessage::PROPAGATE);
    msg1_1.setConversationID("conv");
    msg1_1.setSender(initiator);
    msg1_1.addReceiver(propagator);
    msg1_1.setProtocol("with_single_subprotocol_and_not_proxied");
    std::cout << "C1 MSG1" << std::endl;
    c1.update(msg1_1);
    // Set number of planned sub conversations
    c1.setNumberOfSubConversations(1);
    // P -> S (request)
    ACLMessage msg1_2(ACLMessage::REQUEST);
    msg1_2.setConversationID("conv");
    msg1_2.setSender(propagator);
    msg1_2.addReceiver(serviceProvider0);
    msg1_2.setProtocol("request");
    std::cout << "C1 MSG2" << std::endl;
    c1.update(msg1_2);
    // S -> P (agree)
    ACLMessage msg1_3(ACLMessage::AGREE);
    msg1_3.setConversationID("conv");
    msg1_3.setSender(serviceProvider0);
    msg1_3.addReceiver(propagator);
    msg1_3.setProtocol("request");
    std::cout << "C1 MSG3" << std::endl;
    c1.update(msg1_3);
    BOOST_CHECK_MESSAGE(!c1.hasEnded(), "Test 1: conversation ended too early after msg 3.");
    // S -> P (inform)
    ACLMessage msg1_4(ACLMessage::INFORM);
    msg1_4.setConversationID("conv");
    msg1_4.setSender(serviceProvider0);
    msg1_4.addReceiver(propagator);
    msg1_4.setProtocol("request");
    std::cout << "C1 MSG4" << std::endl;
    c1.update(msg1_4);
    BOOST_CHECK_MESSAGE(c1.hasEnded(), "Test 1: conversation did not end.");
    // P -> I (inform)
    ACLMessage msg1_5(ACLMessage::INFORM);
    msg1_5.setConversationID("conv");
    msg1_5.setSender(propagator);
    msg1_5.addReceiver(initiator);
    msg1_5.setProtocol("with_single_subprotocol_and_not_proxied");
    std::cout << "C1 MSG5" << std::endl;
    // Forwarding a reply is not desired in this protocol, it should throw an exception
    BOOST_REQUIRE_THROW( c1.update(msg1_5), conversation::ProtocolException );
}

BOOST_AUTO_TEST_SUITE_END()
#endif

