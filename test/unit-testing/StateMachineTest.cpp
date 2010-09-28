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
    
    a1 = AgentAID(name1);
    a1.addAdress(addr1);
    
    a2 = AgentAID(name2);
    a2.addAdress(addr3);
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
    
    /*
    std::vector<State>::iterator sit;
    std::vector<Transition>::iterator tit;
    std::vector<State> *st = &req.states;
    std::vector<Transition> *tr;
    std::cout<<"\n\n\n";
    for (sit = st->begin(); sit != st->end(); sit++)
    {
        tr = &(*sit).transitions;
        for (tit = tr->begin(); tit != tr->end(); tit++)
        {
	  std::cout<<"-------------------------------------------\n";
	  std::cout<< tit->from<<"\t"<< tit->to <<"\t"<< tit->expectedPerf <<"\t"<< tit->nextStateName<<"\n";
	  std::cout<< tit->owningState->uid<<"\n";
	  std::cout<<"-------------------------------------------\n";
        }
    }
    */
   
    for (invit = req.involvedAgents.begin(); invit != req.involvedAgents.end(); invit++)
    {
        std::cout<<"role: "<< invit->role<<((invit->check == true)?"set\n":"unset\n");
    }
    
    
    std::vector<ACLMessage> flow;
    flow.clear();
    ACLMessage m1 = ACLMessage(ACLMessage::REQUEST);
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
    m1.setPerformative(ACLMessage::perfs[ACLMessage::REFUSE]);
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
    m1.addReceiver(a1);
    flow.push_back(m1);
    
    std::cout<<"flow of messages built..\n";
     std::cout<<"check if initiator exists returned:\t"<< req.checkIfRoleExists(std::string("initiator"))<< "\n";
    
    std::vector<ACLMessage>::iterator it = flow.begin();
    if (req.startMachine(*it)!=0) std::cout<<"failure to start!!!!!!!!!\n";
    else std::cout<<"machine started..\n";
   
    int i=0;
    while(it != flow.end() && !req.isConversationOver() && i<10)
    {
        int x;
        if ((x = req.consumeMessage(*it)) != 0 ) std::cout<<"\t\tmessage didn't pass\n";
        else std::cout<<"\t\t"<<x<<"\n";
        
        //std::cout<<(i++)<<"\n";
        if ((i++) == 9) std::cout<<"exited because number of steps exceeded max...\n";
        it++;
    }
    
    
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
    AgentAID aid = msg.getSender();
    printAgentAID(aid); 
    if (!(msg.getAllReceivers().empty())) 
				  { cout<<"receivers:\n"; 
				    std::vector<AgentAID> vec = msg.getAllReceivers();
				    printAgentAIDset(vec);}
    if (!msg.getAllReplyTo().empty()) 
			        { cout<<"reply to:\n"; 
				std::vector<AgentAID> vec = msg.getAllReplyTo();
				printAgentAIDset(vec);}
    if (!msg.getUserdefParams().empty()) 
				 {  vector<UserdefParam> params = msg.getUserdefParams();
				    printUserdefParamset(params); }

}

void StateMachineTest::printAgentAIDset( vector<AgentAID> &myset)
{
    cout<<"\t==================Agent AID set==================\n";
    vector<AgentAID>::iterator it = myset.begin();
    for(it; it != myset.end(); it++)
        printAgentAID(*it);
}
void StateMachineTest::printAgentAID( AgentAID &agent)
{
    cout<<"\t==================Agent AID==================\n";
    if (!agent.getName().empty()) cout<<"\t\tname:\t"<< agent.getName()<<endl;
    if (!(agent.getAdresses().empty()))
    {
        cout<<"\t\tadresses:\t\n";
        vector<string> addr = agent.getAdresses();
        vector<string>::iterator it = addr.begin();
        for(it; it != addr.end(); it++)
	  cout<<"\t\t\t"<< *it<<endl;
    }
    if (!(agent.getResolvers().empty())) {cout<<"\t\tresolvers:\t\n"; 
				vector<AgentAID> vec = agent.getResolvers();
				printAgentAIDset(vec);}
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
