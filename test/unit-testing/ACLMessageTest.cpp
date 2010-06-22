/** \file: ACLMessageTest.cpp
 *
 * \author: Mircea Cretu Stancu
 *
 * version: 1.0
 */
 
 
//#include <algorithm>
//#include <vector>
#include <iostream>
#include <fstream>
#include "ACLMessageTest.h"

#include <message-generator/ACLMessageOutputParser.h>
#include <message-parser/message_parser.h>



using namespace std;
using namespace fipa::acl;

CPPUNIT_TEST_SUITE_REGISTRATION (ACLMessageTest);

void ACLMessageTest::setUp()
{
	
	string addr1 = string("addr1");
	string addr2 = string("addr2");
	string addr3 = string("addr3");
	
	p1.setName(string("param"));
	p1.setValue(string("!@#$%^&*("));
	p2 = p1;
	p3.setName(string("param3"));
	p3.setValue(string("value3"));
	p4.setName(string("param4"));
	p4.setValue(string("value4"));
	
	m1.setPerformative(string("test performative"));
	m1.setLanguage(string("test language"));
	m1.setContent(string("test content"));
	m1.setEncoding(string("test encoding"));
	m1.setOntology(string("test ontology"));
	m1.setReplyWith(string("test reply_with"));
	m1.setReplyBy1(string("test reply_by1"));
	m1.setInReplyTo(string("test in_reply_to"));
	m1.setConversationID(string("test conversationID"));
	m1.setProtocol(string("test protocol"));
	
	m2.setPerformative(string("test performative"));
	m2.setLanguage(string("test language"));
	m2.setContent(string("test content"));
	m2.setEncoding(string("test encoding"));
	m2.setOntology(string("test ontology"));
	m2.setReplyWith(string("test reply_with"));
	m2.setReplyBy1(string("test reply_by1"));
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
	a4.setName(string("agent4"));
	a5.setName(string("agent3 5"));
	
	a3.addAdress(addr1);
	a4.addAdress(addr1);
	a5.addAdress(addr1);
	
	a3.addUserdefParam(p1);
	
	a4.addUserdefParam(p4);
	a5.addUserdefParam(p2); // p1 = p2(should)
	
	a2 = a1;
	a5.addResolver(a1);
	a3.addResolver(a2); // a1 = a2(should)
	
	m1.setSender(a4);
	m2.setSender(a4);
	
	m1.addReceiver(a3);
	m2.addReceiver(a3);
	m1.addReceiver(a4);
	m2.addReceiver(a4);
	m1.addUserdefParam(p1);
	m2.addUserdefParam(p1);
	
	
	
	
	
    
	
	
}

void ACLMessageTest::tearDown()
{
	//delete m1;	delete m2;	delete m3;
	//delete a1;	/*delete a2;	delete a3;	*/ delete a4;	//delete a5;
	//delete p1;	delete p2;	delete p3;	delete p4;
}

void ACLMessageTest::MessageEqTest()
{
    CPPUNIT_ASSERT_EQUAL((m1 == m2), true);
    m1.addReceiver(a3);
    CPPUNIT_ASSERT_EQUAL((m1 == m2), true);
    m1.addReplyTo(a2);
    m2.addReplyTo(a1);
    CPPUNIT_ASSERT_EQUAL((m1 == m2), true);
    m1.addReceiver(a1);
    CPPUNIT_ASSERT_EQUAL((m1 == m2), false);
    m3 = m1;
    CPPUNIT_ASSERT_EQUAL((m3 == m2), false);
    m2.addReceiver(a2);
    CPPUNIT_ASSERT_EQUAL((m1 == m2), true);
    
    
}

void ACLMessageTest::AgentEqTest()
{
    
    CPPUNIT_ASSERT_EQUAL((a1 == a2),true);
    CPPUNIT_ASSERT_EQUAL((a5==a3),true);
    a3.addUserdefParam(p3);
    CPPUNIT_ASSERT_EQUAL((a5==a3),false);
    a5.addUserdefParam(p3);
    CPPUNIT_ASSERT_EQUAL((a5==a3),true);
    //AgentAID::setResCompDepth(5);
    CPPUNIT_ASSERT_EQUAL(resDepthEqual(a5,a3,5),true);
    //AgentAID::setResCompDepth(1);
    a5.deleteResolver(a1); 
    a2.addResolver(a4);
    a5.addResolver(a2);
    CPPUNIT_ASSERT_EQUAL((a5 == a3),true);
    a5.addResolver(a2);
    CPPUNIT_ASSERT_EQUAL((a5 == a3),true);
    //AgentAID::setResCompDepth(5);
    CPPUNIT_ASSERT_EQUAL(resDepthEqual(a5,a3,5),false);
    a5.addResolver(a1);
    CPPUNIT_ASSERT_EQUAL((a5 == a3),false);
    std::vector<AgentAID> vec = a5.getResolvers();
    std::vector<AgentAID> ::iterator it = vec.begin();
    while(it!=vec.end()) {a5.deleteResolver((*it)); it++;}
    a3 = a5;
    CPPUNIT_ASSERT_EQUAL((a5 == a3),true);
    
    
}

void ACLMessageTest::ForMemLeakTest()
{
    int i;
    ACLMessage mes[STRESS_NR];
    a2.addResolver(a4);
    a1.addResolver(a2);
    a2.addResolver(a3);
    a3.addResolver(a5);
    
    
    for (i = 0; i< STRESS_NR; i++)
    {
        
        mes[i].addReceiver(a1); mes[i].addReceiver(a2); mes[i].addReceiver(a3);
        mes[i].addReplyTo(a2); mes[i].addReplyTo(a3); mes[i].addReplyTo(a4);
        mes[i].addUserdefParam(p3); mes[i].addUserdefParam(p2); mes[i].addUserdefParam(p4);
    }
    CPPUNIT_ASSERT_EQUAL(true,true);// just a test to create many messages and observe the memory leaks the generate
}

void ACLMessageTest::EncDecodeTest()
{
    ACLMessage m5 = ACLMessage(string("test performative"));
    
    m5.setPerformative(string("test performative"));
    m5.setLanguage(string("test language"));
    m5.setContent(string("test content"));
    m5.setEncoding(string("test encoding"));
    m5.setOntology(string("test ontology"));
    m5.setReplyWith(string("test reply with"));
    m5.setReplyBy1(string("2010062201010"));
    m5.setInReplyTo(string("test in reply to"));
    m5.setConversationID(string("test conversationID"));
    m5.setProtocol(string("test protocol"));
    
    m5.setSender(a4);
    
    //printMessage(m5);
    
    char name[] = "TestMessage23.txt";
    
    /*filebuf fb;
    fb.open ("TestMessage23.txt",ios::in);
    istream src(&fb);

    //ifstream src = ifstream(name, ios_base::in);
    std::string storage; 
	
	// due to special characters don't use std::copy here
	char buffer;
	while(true)
	{
	   src.get(buffer);
	   if(!src.eof())
		   storage += buffer;
	   else
		   break;
	}
	*/
    cout<<"\n";
    ACLMessageOutputParser out;
    out.setMessage(m5);
    MessageParser parser;
    if (!out.printParsedMessage(string("testMessage.txt"))) cout << "ERROR GENERATING..\n";
    //cout << out.getBitMessage() <<"\n";
    
    
    ACLMessage restored;
    if ( ! parser.parseData(out.getBitMessage(),restored)) cout << "ERROR PARSING...\n";
    cout<<"\n";
    //cout<<m1.getProtocol()<<"\t\t"<<restored.getProtocol()<<"\n\n\n\n\n";
    //printMessage (m1);
    printMessage(restored);
    //CPPUNIT_ASSERT_EQUAL((m1 == restored),true);
}

void ACLMessageTest::printMessage( ACLMessage &msg)
{
    cout<<"=================================Printing Message=================================\n";
    cout<<"performative:\t"<< msg.getPerformative()<<endl;
    
    if (!msg.getContent().empty()) 
        cout<<"content:\t"<< msg.getContent()<<endl; 
    if (!msg.getReplyWith().empty()) 
        cout<<"reply with:\t"<< msg.getReplyWith()<<endl;
    if (!msg.getReplyBy1().empty()) 
        cout<<"reply by1:\t"<< msg.getReplyBy1()<<endl;
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

void ACLMessageTest::printAgentAIDset( vector<AgentAID> &myset)
{
    cout<<"\t==================Agent AID set==================\n";
    vector<AgentAID>::iterator it = myset.begin();
    for(it; it != myset.end(); it++)
        printAgentAID(*it);
}
void ACLMessageTest::printAgentAID( AgentAID &agent)
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

void ACLMessageTest::printUserdefParamset( vector<UserdefParam> &params)
{
    cout<<"\t\t==================User Defined Parameters==================\n";
    vector<UserdefParam>::iterator it = params.begin();
    for(it; it != params.end(); it++)
    {
        cout<<"\t\tparam name:\t"<< (*it).getName()<<endl;
        cout<<"\t\tparam value:\t"<< (*it).getValue()<<"\n\n";
    }
}

