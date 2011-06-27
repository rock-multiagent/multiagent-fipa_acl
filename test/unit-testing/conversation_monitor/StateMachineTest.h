/** \file: StateMachineTest.h
 * \author: Mircea Cretu Stancu
 * version: 1.0
 */

#ifndef _STATEMACHINETEST_H_
#define _STATEMACHINETEST_H_

#include <iostream>
#include <fipa_acl/bitefficient_message.h>
#include <fipa_acl/conversation_monitor.h>
#include "StateMachineTestUtils.h"

BOOST_AUTO_TEST_SUITE(conversation_monitor_suite)

BOOST_AUTO_TEST_CASE(request_protocol_test)
{
    using namespace fipa::acl;
    std::string name1 = std::string("a1");
    std::string name2 = std::string("a2");
    std::string addr1 = std::string("addr1");
    std::string addr2 = std::string("addr2");
    std::string addr3 = std::string("addr3");
    
    AgentID a1 = AgentID(name1);
    a1.addAddress(addr1);
    
    AgentID a2 = AgentID(name2);
    a2.addAddress(addr3);

    StateMachine* req = new StateMachine();
    req->setOwner(a1);
    
    State s1 = State("1");
    s1.setFinal(false);
    State s2 = State ("2");
    State s3 = State ("3");
    State s4 = State ("4");
    State s5 = State ("5");

    s2.setFinal(false);
    s3.setFinal(true);
    s5.setFinal(true);

    s1.setOwningMachine(req);
    s2.setOwningMachine(req);
    s3.setOwningMachine(req);
    s4.setOwningMachine(req);
    s5.setOwningMachine(req);

    BOOST_CHECK_MESSAGE(req->addState(s1), "state 1 added");
    BOOST_CHECK_MESSAGE(req->addState(s2), "state 2 added");
    BOOST_CHECK_MESSAGE(req->addState(s3), "state 2 added");
    BOOST_CHECK_MESSAGE(req->addState(s4), "state 2 added");
    BOOST_CHECK_MESSAGE(req->addState(s5), "state 2 added");
    req->generateDefaultStates();
    
    req->addRole("initiator");
    req->addRole("B");
    std::vector<AgentMapping>::iterator invit;
    BOOST_CHECK_MESSAGE(1==1,"List of roles currently in the involvedAgents vector:");
    for (invit = req->getInvolvedAgents().begin(); invit != req->getInvolvedAgents().end(); invit++)
    {
        std::cout<<"role: "<< invit->role<<((invit->check == true)?" set\n":" unset\n");
    }
    BOOST_CHECK_MESSAGE(1==1,"roles set");

    Transition t = Transition();
    
    t.setFrom("initiator");
    t.setTo("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::REQUEST]);
    t.setNextStateName("2");
    std::cout<<"transition built..\n";

    BOOST_CHECK_MESSAGE(req->getStateByName(std::string("1")) != NULL, "State search by name");

    req->getStateByName("1")->addTransition(t);
    
    t.setTo("initiator");
    t.setFrom("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::REFUSE]);
    t.setNextStateName("3");
    
    req->getStateByName("2")->addTransition(t);
    
    t.setTo("initiator");
    t.setFrom("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::AGREE]);
    t.setNextStateName("4");
    req->getStateByName("2")->addTransition(t);
    
    t.setTo("initiator");
    t.setFrom("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::FAILURE]);
    t.setNextStateName("5");
    req->getStateByName("4")->addTransition(t);
    
    t.setTo("initiator");
    t.setFrom("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::INFORM]);
    t.setNextStateName("5");
    req->getStateByName("4")->addTransition(t);
      
    req->generateDefaultTransitions();
    req->setInitialState("1");
    
    BOOST_CHECK_MESSAGE(1==1,"state machine created");
   
    for (invit = req->getInvolvedAgents().begin(); invit != req->getInvolvedAgents().end(); invit++)
    {
        std::cout<<"role: "<< invit->role<<((invit->check == true)?"set\n":"unset\n");
    }
    
    
    std::vector<ACLMessage> flow;
    flow.clear();
    ACLMessage m1 = ACLMessage(REQUEST);
    //m1.setPerformative(std::string("test performative"));
    m1.setLanguage(std::string("test language"));
    m1.setContent(std::string("test content"));
    m1.setEncoding(std::string("test encoding"));
    m1.setOntology(std::string("test ontology"));
    m1.setReplyWith(std::string("test reply_with"));
    m1.setReplyBy1(std::string("2010-12-23T12:00:37:980"));
    m1.setInReplyTo(std::string("test in_reply_to"));
    m1.setConversationID(std::string("test conversationID"));
    m1.setProtocol(std::string("test protocol"));
    m1.setSender(a1);
    m1.addReceiver(a2);
    
    flow.push_back(m1);
    m1.setPerformative(ACLMessage::perfs[ACLMessage::AGREE]);
    m1.setLanguage(std::string("test language"));
    m1.setContent(std::string("test content"));
    m1.setEncoding(std::string("test encoding"));
    m1.setOntology(std::string("test ontology"));
    m1.setReplyWith(std::string("test reply_with"));
    m1.setReplyBy1(std::string("2010-12-23T12:00:37:980"));
    m1.setInReplyTo(std::string("test reply_with"));
    m1.setConversationID(std::string("test conversationID"));
    m1.setProtocol(std::string("test protocol"));
    m1.setSender(a2);
    m1.deleteReceiver(a2);
    m1.addReceiver(a1);
    flow.push_back(m1);
    
    m1.setPerformative(ACLMessage::perfs[ACLMessage::INFORM]);
    m1.setLanguage(std::string("test language"));
    m1.setContent(std::string("test content"));
    m1.setEncoding(std::string("test encoding"));
    m1.setOntology(std::string("test ontology"));
    m1.setReplyWith(std::string("test reply_with"));
    m1.setReplyBy1(std::string("2010-12-23T12:00:37:980"));
    std::string inrepto = std::string();
    inrepto.clear();
    m1.setInReplyTo(inrepto);
    m1.setConversationID(std::string("test conversationID"));
    m1.setProtocol(std::string("test protocol"));
    m1.setSender(a2);
    
    
    flow.push_back(m1);
    
    std::cout<<"flow of messages built..\n";
    BOOST_CHECK_MESSAGE(req->checkIfRoleExists(std::string("initiator")),"check if initiator exists");
    
    std::vector<ACLMessage>::iterator it = flow.begin();
    std::cout<< it->getPerformative()<<"\n";

    while(it != flow.end() && !req->isConversationOver())
    {
        BOOST_CHECK_MESSAGE(req->consumeMessage(*it) == 0, "Consume message"); 
        it++;
    }

    BOOST_CHECK_MESSAGE(req->isConversationOver(),"Conversation validated");
    
    
}

BOOST_AUTO_TEST_CASE(request_protocol_test_from_file)
{

    using namespace fipa::acl;

    std::string name1 = std::string("a1");
    std::string name2 = std::string("a2");
    std::string addr1 = std::string("addr1");
    std::string addr2 = std::string("addr2");
    std::string addr3 = std::string("addr3");

    AgentID a1 = AgentID(name1);
    a1.addAddress(addr1);
    
    AgentID a2 = AgentID(name2);
    a2.addAddress(addr3);

    StateMachineBuilder::setProtocolResourceDir(".");
    StateMachineBuilder builder;

    try {
        StateMachine req = builder.getFunctionalStateMachine(std::string("blabla"));
        BOOST_FAIL("Instanciating with incorrect protocol does not throw as expected");
    } catch(const std::runtime_error& e)
    {
        BOOST_CHECK_MESSAGE(1==1, "Instanciating with incorrect protocol throws");
    }

    try {
        StateMachine req = builder.getFunctionalStateMachine(std::string("request"));
        req.setOwner(a1);

        BOOST_CHECK(req.getOwner().getName() == name1);

        BOOST_CHECK(req.checkIfRoleExists(std::string("initiator")));
        std::vector<ACLMessage> flow = buildRequestMessageFlow(a1, a2);
        std::vector<ACLMessage>::iterator it = flow.begin();

        while(it != flow.end() && !req.isConversationOver())
        {
            BOOST_CHECK_MESSAGE(req.consumeMessage(*it) == 0, "Consume message");
            it++;
        }
        BOOST_CHECK_MESSAGE(req.isConversationOver(), "Conversation is over");
    } catch(const std::runtime_error& e)
    {
        BOOST_FAIL("Get functional state machine for protocol 'request' failed");
    }

}

BOOST_AUTO_TEST_SUITE_END()

#endif // _STATEMACHINE_TEST_H_

