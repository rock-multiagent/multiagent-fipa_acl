/** \file: ACLMessageTest.h
 *
 * \author: Mircea Cretu Stancu
 *
 * version: 1.0
 */

#ifndef _BUGTEST_H_
#define _BUGTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>


using namespace std;

class BugTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE (BugTest);
	
	CPPUNIT_TEST (instructTest);
 	CPPUNIT_TEST (positionTest);
	CPPUNIT_TEST (set_posTest);
	CPPUNIT_TEST (set_stateTest);
	CPPUNIT_TEST (set_restingTest);
	CPPUNIT_TEST (set_directionTest);
	CPPUNIT_TEST (set_has_foodTest);
	CPPUNIT_TEST (get_colorTest);
	CPPUNIT_TEST (get_stateTest);
	CPPUNIT_TEST (get_restingTest);
	CPPUNIT_TEST (get_directionTest);
	CPPUNIT_TEST (get_has_foodTest);
	
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp();
		void tearDown();
	
	protected:
		void instructTest();
		void positionTest();
		void set_posTest();
		void set_stateTest();
		void set_restingTest();
		void set_directionTest();
		void set_has_foodTest();
		void get_colorTest();
		void get_stateTest();
		void get_restingTest();
		void get_directionTest();
		void get_has_foodTest();

	private:
		Bug *a, *b, *c;
		//Pos x;
		Instruction instr;

};

#endif