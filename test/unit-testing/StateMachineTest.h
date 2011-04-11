/** \file: StateMachineTest.h
 *
 * \author: Mircea Cretu Stancu
 *
 * version: 1.0
 */

#ifndef _STATEMACHINETEST_H_
#define _STATEMACHINETEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <message-generator/ACLMessage.h>
#include "../../src/StateMachine.h"
#include "../../src/State.h"
#include "../../src/Transition.h"
#include "../../src/StateMachineBuilder.h"

using namespace std;

namespace fipa { namespace acl {
class StateMachineTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE (StateMachineTest);
	
	
	//CPPUNIT_TEST (RequestProtocolTest);
	CPPUNIT_TEST (RequestProtocolTestFromFile);
	
	
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp();
		void tearDown();
	
	protected:
		
	    void RequestProtocolTest();
	    void RequestProtocolTestFromFile();
	    
	    std::vector<ACLMessage> buildRequestMessageFlow();
	    void printMessage( ACLMessage &msg);
	    void printAgentID( AgentID &agent);
	    void printAgentIDset( vector<AgentID> &myset);
	    void printUserdefParamset( vector<UserdefParam> &params);

	private:
		//StateMachine req;
		AgentID a1,a2;

};

}}

#endif
