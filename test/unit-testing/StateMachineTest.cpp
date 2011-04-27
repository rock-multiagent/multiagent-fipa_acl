/** \file: StateMachineTest.cpp
 *
 * \author: Mircea Cretu Stancu
 *
 * version: 1.0
 */
 
 
//#include <algorithm>
//#include <vector>
#include <iostream>
#include <fstream>
#include "StateMachineTest.h"


//#include <message-generator/ACLMessageOutputParser.h>
//#include <message-parser/message_parser.h>
//#include "../../src/.h"



using namespace std;
namespace fipa { namespace acl {
CPPUNIT_TEST_SUITE_REGISTRATION (StateMachineTest);



void StateMachineTest::setUp()
{
    std::string name1 = std::string("a1");
    std::string name2 = std::string("a2");
    std::string addr1 = std::string("addr1");
    std::string addr2 = std::string("addr2");
    std::string addr3 = std::string("addr3");
    
    a1 = AgentID(name1);
    a1.addAddress(addr1);
    
    a2 = AgentID(name2);
    a2.addAddress(addr3);
}

void StateMachineTest::tearDown()
{
}

void StateMachineTest::RequestProtocolTest()
{
    StateMachine req = StateMachine();
    req.setOwner(a1);
    std::cout<<"\nowner set..\n";
    
    State s1 = State("1");
    s1.setFinal(false);
    State s2 = State ("2");
    State s3 = State ("3");
    State s4 = State ("4");
    State s5 = State ("5");
    s2.setFinal(false);
    s3.setFinal(true);
    s5.setFinal(true);
    s1.setOwningMachine(&req);
    s2.setOwningMachine(&req);
    s3.setOwningMachine(&req);
    s4.setOwningMachine(&req);
    s5.setOwningMachine(&req);
    std::cout<<"states created...\n";
    if (req.addState(s1))
    std::cout<<"state 1 added...\n";
    if (req.addState(s2))
    std::cout<<"state 2 added...\n";
    if (req.addState(s3))
    std::cout<<"state 3 added...\n";
    if (req.addState(s4))
    std::cout<<"state 4 added...\n";
    if (req.addState(s5))
    std::cout<<"state 5 added...\n";
    req.generateDefaultStates();
    std::cout<<"default states added..\n";
    
    req.addRole("initiator");
    req.addRole("B");
     std::vector<AgentMapping>::iterator invit;
     std::cout<<"-----------list of roles currently in the involvedAgents vector:----------\n";
    for (invit = req.involvedAgents.begin(); invit != req.involvedAgents.end(); invit++)
    {
        std::cout<<"role: "<< invit->role<<((invit->check == true)?" set\n":" unset\n");
    }
    
    std::cout<<"roles set..\n";
    Transition t = Transition();
    
    t.setFrom("initiator");
    t.setTo("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::REQUEST]);
    t.setNextStateName("2");
    std::cout<<"transition built..\n";
    if (req.getStateByName(std::string("1")) == NULL) std::cout<<"state search by name failed!!!!!\n";
    req.getStateByName("1")->addTransition(t);
    std::cout<<"transition added..\n";
    
    t.setTo("initiator");
    t.setFrom("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::REFUSE]);
    t.setNextStateName("3");
    
    req.getStateByName("2")->addTransition(t);
    std::cout<<"transition added\n";
    
    t.setTo("initiator");
    t.setFrom("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::AGREE]);
    t.setNextStateName("4");
    req.getStateByName("2")->addTransition(t);
    std::cout<<"transition added\n";
    
    t.setTo("initiator");
    t.setFrom("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::FAILURE]);
    t.setNextStateName("5");
    req.getStateByName("4")->addTransition(t);
    std::cout<<"transition added\n";
    
    t.setTo("initiator");
    t.setFrom("B");
    t.setExpectedPerformative(ACLMessage::perfs[ACLMessage::INFORM]);
    t.setNextStateName("5");
    req.getStateByName("4")->addTransition(t);
    std::cout<<"transition added\n";
      
    req.generateDefaultTransitions();
    req.setInitialState("1");
    
    std::cout<<"state machine created..\n";
    
   
    for (invit = req.involvedAgents.begin(); invit != req.involvedAgents.end(); invit++)
    {
        std::cout<<"role: "<< invit->role<<((invit->check == true)?"set\n":"unset\n");
    }
    
    
    std::vector<ACLMessage> flow;
    flow.clear();
    ACLMessage m1 = ACLMessage(REQUEST);
    //m1.setPerformative(string("test performative"));
    m1.setLanguage(string("test language"));
    m1.setContent(string("test content"));
    m1.setEncoding(string("test encoding"));
    m1.setOntology(string("test ontology"));
    m1.setReplyWith(string("test reply_with"));
    m1.setReplyBy1(string("2010-12-23T12:00:37:980"));
    m1.setInReplyTo(string("test in_reply_to"));
    m1.setConversationID(string("test conversationID"));
    m1.setProtocol(string("test protocol"));
    m1.setSender(a1);
    m1.addReceiver(a2);
    
    flow.push_back(m1);
    m1.setPerformative(ACLMessage::perfs[ACLMessage::AGREE]);
    m1.setLanguage(string("test language"));
    m1.setContent(string("test content"));
    m1.setEncoding(string("test encoding"));
    m1.setOntology(string("test ontology"));
    m1.setReplyWith(string("test reply_with"));
    m1.setReplyBy1(string("2010-12-23T12:00:37:980"));
    m1.setInReplyTo(string("test reply_with"));
    m1.setConversationID(string("test conversationID"));
    m1.setProtocol(string("test protocol"));
    m1.setSender(a2);
    m1.deleteReceiver(a2);
    m1.addReceiver(a1);
    flow.push_back(m1);
    
    m1.setPerformative(ACLMessage::perfs[ACLMessage::INFORM]);
    m1.setLanguage(string("test language"));
    m1.setContent(string("test content"));
    m1.setEncoding(string("test encoding"));
    m1.setOntology(string("test ontology"));
    m1.setReplyWith(string("test reply_with"));
    m1.setReplyBy1(string("2010-12-23T12:00:37:980"));
    std::string inrepto = std::string();
    inrepto.clear();
    m1.setInReplyTo(inrepto);
    m1.setConversationID(string("test conversationID"));
    m1.setProtocol(string("test protocol"));
    m1.setSender(a2);
    
    
    flow.push_back(m1);
    
    std::cout<<"flow of messages built..\n";
    std::cout<<"check if initiator exists returned:\t"<< req.checkIfRoleExists(std::string("initiator"))<< "\n";
    //req.print();
    
    std::vector<ACLMessage>::iterator it = flow.begin();
    std::cout<< it->getPerformative()<<"\n";
    if (req.startMachine(*it)!=0) std::cout<<"failure to start!!!!!!!!!\n";
    else std::cout<<"machine started..\n";
    it++;
    int i=0;
    while(it != flow.end() && !req.isConversationOver())
    {
        int x;
        if ((x = req.consumeMessage(*it)) != 0 ) { std::cout<<"\t\tmessage didn't pass\n"; break; }
        else std::cout<<"\t\t @@@@message passed@@@@"<<x<<"\n";
        
        //std::cout<<(i++)<<"\n";
        it++;
    }
    if (req.isConversationOver()) std::cout<< "\t\t@@@@ worked! @@@@\n";
    
    
}

void StateMachineTest::RequestProtocolTestFromFile()
{
    StateMachineBuilder builder = StateMachineBuilder();
    StateMachine req = builder.getFunctionalStateMachine(std::string("request"));
    req.setOwner(a1);
    std::cout<< "\t owner of machine: " << req.getOwner().getName() <<"\n";
    
    std::cout<<"check if initiator exists returned:\t"<< req.checkIfRoleExists(std::string("initiator"))<< "\n";
    
    //std::vector<ACLMessage> flow = buildRequestMessageFlow();
    
    std::vector<ACLMessage> flow;
    flow.clear();
    flow = buildRequestMessageFlow();
    
    std::cout<<"flow of messages built..\n";
    //req.print();
    
    std::vector<ACLMessage>::iterator it = flow.begin();
    std::cout<< it->getPerformative()<<"\n";
    if (req.startMachine(*it)!=0) std::cout<<"failure to start!!!!!!!!!\n";
    else std::cout<<"machine started..\n";
    it++;
    int i=0;
    while(it != flow.end() && !req.isConversationOver())
    {
        int x;
        if ((x = req.consumeMessage(*it)) != 0 ) { std::cout<<"\t\tmessage didn't pass\n"; break; }
        else std::cout<<"\t\t@@@@ message passed @@@@"<<x<<"\n";
        
        //std::cout<<(i++)<<"\n";
        it++;
    }
    if (req.isConversationOver()) std::cout<< "\t\t@@@@ worked! @@@@\n";
}


std::vector<ACLMessage> StateMachineTest::buildRequestMessageFlow()
{
    std::vector<ACLMessage> flow;
    flow.clear();
    ACLMessage m1 = ACLMessage(REQUEST);
    //m1.setPerformative(string("test performative"));
    m1.setLanguage(string("test language"));
    m1.setContent(string("test content"));
    m1.setEncoding(string("test encoding"));
    m1.setOntology(string("test ontology"));
    m1.setReplyWith(string("test reply_with"));
    m1.setReplyBy1(string("2010-12-23T12:00:37:980"));
    m1.setInReplyTo(string("test in_reply_to"));
    m1.setConversationID(string("test conversationID"));
    m1.setProtocol(string("test protocol"));
    m1.setSender(a1);
    m1.addReceiver(a2);
    
    flow.push_back(m1);
    m1.setPerformative(ACLMessage::perfs[ACLMessage::AGREE]);
    m1.setLanguage(string("test language"));
    m1.setContent(string("test content"));
    m1.setEncoding(string("test encoding"));
    m1.setOntology(string("test ontology"));
    m1.setReplyWith(string("test reply_with"));
    m1.setReplyBy1(string("2010-12-23T12:00:37:980"));
    m1.setInReplyTo(string("test reply_with"));
    m1.setConversationID(string("test conversationID"));
    m1.setProtocol(string("test protocol"));
    m1.setSender(a2);
    m1.deleteReceiver(a2);
    m1.addReceiver(a1);
    flow.push_back(m1);
    
    m1.setPerformative(ACLMessage::perfs[ACLMessage::INFORM]);
    m1.setLanguage(string("test language"));
    m1.setContent(string("test content"));
    m1.setEncoding(string("test encoding"));
    m1.setOntology(string("test ontology"));
    m1.setReplyWith(string("test reply_with"));
    m1.setReplyBy1(string("2010-12-23T12:00:37:980"));
    std::string inrepto = std::string();
    inrepto.clear();
    m1.setInReplyTo(inrepto);
    m1.setConversationID(string("test conversationID"));
    m1.setProtocol(string("test protocol"));
    m1.setSender(a2);
    flow.push_back(m1);
    
    std::cout<<"flow of messages built..\n";
    
    return flow;
}


void StateMachineTest::printMessage( ACLMessage &msg)
{
    cout<<"=================================Printing Message=================================\n";
    cout<<"performative:\t"<< msg.getPerformative()<<endl;
    
    if (!msg.getContent().empty()) 
        cout<<"content:\t"<< msg.getContent()<<endl; 
    if (!msg.getReplyWith().empty()) 
        cout<<"reply with:\t"<< msg.getReplyWith()<<endl;
    //if (!msg.getReplyBy1().empty()) 
        cout<<"reply by1:\t"<< msg.getReplyBy1(1)<<endl;
    if (!msg.getInReplyTo().empty()) 
        cout<<"in reply to:\t"<< msg.getInReplyTo()<<endl;
    if (!msg.getLanguage().empty()) 
        cout<<"language:\t"<< msg.getLanguage()<<endl;
    if (!msg.getEncoding().empty()) 
        cout<<"encoding:\t"<< msg.getEncoding()<<endl;
    if (!msg.getOntology().empty()) 
        cout<<"ontology:\t"<< msg.getOntology()<<endl; 
    if (!msg.getProtocol().empty()) 
        cout<<"protocol:\t"<< msg.getProtocol()<<endl;
    if (!msg.getConversationID().empty()) 
        cout<<"conversation id:\t"<< msg.getConversationID()<<endl;
    cout<<"sender:\n";
    AgentID aid = msg.getSender();
    printAgentID(aid); 
    if (!(msg.getAllReceivers().empty())) 
				  { cout<<"receivers:\n"; 
				    std::vector<AgentID> vec = msg.getAllReceivers();
				    printAgentIDset(vec);}
    if (!msg.getAllReplyTo().empty()) 
			        { cout<<"reply to:\n"; 
				std::vector<AgentID> vec = msg.getAllReplyTo();
				printAgentIDset(vec);}
    if (!msg.getUserdefParams().empty()) 
				 {  vector<UserdefParam> params = msg.getUserdefParams();
				    printUserdefParamset(params); }

}

void StateMachineTest::printAgentIDset( vector<AgentID> &myset)
{
    cout<<"\t==================Agent AID set==================\n";
    vector<AgentID>::iterator it = myset.begin();
    for(it; it != myset.end(); it++)
        printAgentID(*it);
}
void StateMachineTest::printAgentID( AgentID &agent)
{
    cout<<"\t==================Agent AID==================\n";
    if (!agent.getName().empty()) cout<<"\t\tname:\t"<< agent.getName()<<endl;
    if (!(agent.getAddresses().empty()))
    {
        cout<<"\t\tadresses:\t\n";
        vector<string> addr = agent.getAddresses();
        vector<string>::iterator it = addr.begin();
        for(it; it != addr.end(); it++)
	  cout<<"\t\t\t"<< *it<<endl;
    }
    if (!(agent.getResolvers().empty())) {cout<<"\t\tresolvers:\t\n"; 
				vector<AgentID> vec = agent.getResolvers();
				printAgentIDset(vec);}
    if (!(agent.getUserdefParams().empty())) {cout<<"\t\tUser Defined Parameters:\t\n";
				    vector<UserdefParam> params = agent.getUserdefParams();
				    printUserdefParamset(params);}
}

void StateMachineTest::printUserdefParamset( vector<UserdefParam> &params)
{
    cout<<"\t\t==================User Defined Parameters==================\n";
    vector<UserdefParam>::iterator it = params.begin();
    for(it; it != params.end(); it++)
    {
        cout<<"\t\tparam name:\t"<< (*it).getName()<<endl;
        cout<<"\t\tparam value:\t"<< (*it).getValue()<<"\n\n";
    }
}

}}
