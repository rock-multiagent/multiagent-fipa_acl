/** \file: ACLMessageTest.cpp
 *
 * \author: Mircea Cretu Stancu
 *
 * version: 1.0
 */
 
 
#include "ACLMessageTest.h"
#include "../../src/ACLMessageOutputParser.h"



using namespace std;
using namespace fipa::acl;

CPPUNIT_TEST_SUITE_REGISTRATION (ACLMessageTest);

void ACLMessageTest::setUp()
{
	/*
	m1 = new ACLMessage();
	m2 = new ACLMessage();
	m3 = new ACLMessage();
	*/
	a1 = new AgentAID();
	//a2 = new AgentAID();
	//a3 = new AgentAID();
	a4 = new AgentAID();
	//a5 = new AgentAID();
	/*
	p1 = new UserdefParam();
	p2 = new UserdefParam();
	p3 = new UserdefParam();
	p4 = new UserdefParam();
	
	p1->setName(string("param"));
	p1->setValue(string("!@#$%^&*("));
	*p2 = *p1;
	p3->setName(string("param3"));
	p3->setValue(string("value3"));
	p4->setName(string("param4"));
	p4->setValue(string("value4"));
	
	m1->setPerformative(string("test performative"));
	m1->setLanguage(string("test language"));
	m1->setContent(string("test content"));
	m1->setEncoding(string("test encoding"));
	m1->setOntology(string("test ontology"));
	m1->setReplyWith(string("test reply_with"));
	m1->setReplyBy1(string("test reply_by1"));
	m1->setInReplyTo(string("test in_reply_to"));
	m1->setConversationID(string("test conversationID"));
	m1->setProtocol(string("test protocol"));
	*/
	a1->setName(string("agent1"));
	a1->addAdress(string("addr1"));
	//a1->addAdress(string("addr2"));
	//a1->addAdress(string("addr3"));
	//a1->addUserdefParam(p1);
	//a1->addUserdefParam(p3);
	
	/*
	a3->setName(string("agent3 5"));
	a4->setName(string("agent4"));
	a5->setName(string("agent3 5"));
	
	a3->addAdress(string("addr1"));
	a4->addAdress(string("addr1"));
	a3->addAdress(string("addr1"));
	
	a3->addUserdefParam(p1);
	
	a4->addUserdefParam(p4);
	a5->addUserdefParam(p2);
	
	//*a2 = *a1;
	a5->addResolver(a1);
	a3->addResolver(a2);
	*/
	a1->addResolver(a4);
	
	
	
    
	
	
}

void ACLMessageTest::tearDown()
{
	//delete m1;	delete m2;	delete m3;
	delete a1;	/*delete a2;	delete a3;	*/ delete a4;	//delete a5;
	//delete p1;	delete p2;	delete p3;	delete p4;
}

void ACLMessageTest::AgentEqTest()
{
    /*
    CPPUNIT_ASSERT_EQUAL((*a1 == *a2),true);
    CPPUNIT_ASSERT_EQUAL(((*a5)==(*a3)),true);
    a3->addUserdefParam(p3);
    CPPUNIT_ASSERT_EQUAL(((*a5)==(*a3)),false);
    a5->addUserdefParam(p3);
    CPPUNIT_ASSERT_EQUAL(((*a5)==(*a3)),true);
    AgentAID::setResCompDepth(5);
    CPPUNIT_ASSERT_EQUAL(((*a5)==(*a3)),true);
    AgentAID::setResCompDepth(1);
    (*a5->getResolvers()->begin())->addResolver(a4);
    CPPUNIT_ASSERT_EQUAL(((*a5) == (*a3)),true);
    AgentAID::setResCompDepth(2);
    CPPUNIT_ASSERT_EQUAL((*a5==*a3),false);
    */
    
}
