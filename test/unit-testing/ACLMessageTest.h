/** \file: ACLMessageTest.h
 *
 * \author: Mircea Cretu Stancu
 *
 * version: 1.0
 */

#ifndef _ACLMESSAGETEST_H_
#define _ACLMESSAGETEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

//#include <message-generator/ACLMessage.h>

#include "../../src/ACLMessage.h"

#define STRESS_NR 20

using namespace std;
using namespace fipa::acl;

class ACLMessageTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE (ACLMessageTest);
	
	CPPUNIT_TEST (MessageEqTest);
	CPPUNIT_TEST (AgentEqTest);
 	CPPUNIT_TEST (ForMemLeakTest);
	//CPPUNIT_TEST (assignTest);
	
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp();
		void tearDown();
	
	protected:
		
	    void AgentEqTest();
	    void MessageEqTest();
	    void ForMemLeakTest();
	    void printMessage( ACLMessage &msg);
	    void printAgentAID( AgentAID &agent);
	    void printAgentAIDset( vector<AgentAID> &myset);
	    void printUserdefParamset( vector<UserdefParam> &params);

	private:
		ACLMessage m1, m2, m3;
		UserdefParam p1,p2,p3,p4;
		AgentAID a1,a2,a3,a4,a5;

};

#endif