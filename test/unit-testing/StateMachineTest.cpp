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
using namespace fipa::acl;

CPPUNIT_TEST_SUITE_REGISTRATION (StateMachineTest);

void StateMachineTest::setUp()
{
	
	/*
	string addr1 = string("addr1");
	string addr2 = string("addr2");
	string addr3 = string("addr3");
	
	
	
	m1.setPerformative(string("test performative"));
	m1.setLanguage(string("test language"));
	m1.setContent(string("test content"));
	m1.setEncoding(string("test encoding"));
	m1.setOntology(string("test ontology"));
	m1.setReplyWith(string("test reply_with"));
	m1.setReplyBy1(string("2010-12-23T12:00:37:980"));
	m1.setInReplyTo(string("test in_reply_to"));
	m1.setConversationID(string("test conversationID"));
	m1.setProtocol(string("test protocol"));
	
	m2.setPerformative(string("test performative"));
	m2.setLanguage(string("test language"));
	m2.setContent(string("test content"));
	m2.setEncoding(string("test encoding"));
	m2.setOntology(string("test ontology"));
	m2.setReplyWith(string("test reply_with"));
	m2.setReplyBy1(string("2010-12-23T12:00:37:980"));
	m2.setInReplyTo(string("test in_reply_to"));
	m2.setConversationID(string("test conversationID"));
	m2.setProtocol(string("test protocol"));
	
	a1.setName(string("agent1"));
	a1.addAdress(addr1);
	a1.addAdress(addr2);
	a1.addAdress(addr3);
	a1.addUserdefParam(p1);
	a1.addUserdefParam(p3);
	
	
	a3.setName(string("agent3 5"));
	a4.setName(string("agentfour"));
	a5.setName(string("agent3 5"));
	
	a3.addAdress(addr1);
	a4.addAdress(addr1);
	a5.addAdress(addr1);
	
	a2 = a1;
	a5.addResolver(a1);
	a3.addResolver(a2); // a1 = a2(should)
	
	m1.setSender(a4);
	m2.setSender(a4);
	
	m1.addReceiver(a3);
	m2.addReceiver(a3);
	m1.addReceiver(a4);
	m2.addReceiver(a4);
	*/
	
	
}

void StateMachineTest::tearDown()
{
}

void StateMachineTest::RequestProtocolTest()
{
    
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

