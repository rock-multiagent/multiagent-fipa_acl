/** \file: ACLMessageTest.cpp
 *
 * \author: Mircea Cretu Stancu
 *
 * version: 1.0
 */
 
 
#include "ACLMessageTest.h"
#include <vector>


using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION (BugTest);

void BugTest::setUp()
{
	a = new Bug(); 
	a->Color = Red;
	b = new Bug();
	b->Color = Red;
	c = new Bug();
	
	c->state = 0;
	c->Color = Red;
	c->direction = 1;
	c->has_food = true;
	c->pos = Pos(1,1);
	c->resting = 14;
	
  instr = Instruction();
	instr.type = std::string("move");
	Bug::red_brain.push_back(instr);
	
	
}

void BugTest::tearDown()
{
	delete a;
	delete b;
	delete c;
}

void BugTest::instructTest()
{
     CPPUNIT_ASSERT_EQUAL(c->instruct().type, std::string("move"));
}
void BugTest::positionTest()
{
     CPPUNIT_ASSERT_EQUAL((c->position()).X_pos,1);
     CPPUNIT_ASSERT_EQUAL((c->position()).Y_pos,1);
}
void BugTest::set_posTest()
{
     b->pos = Pos(1,1);
     a->set_pos(Pos(1,1));
     CPPUNIT_ASSERT_EQUAL((a->pos).X_pos,(b->pos).X_pos);
     CPPUNIT_ASSERT_EQUAL((a->pos).Y_pos,(b->pos).Y_pos);
}
void BugTest::set_stateTest()
{
     b->state = 0;
     a->set_state(0);
     CPPUNIT_ASSERT_EQUAL(b->state,a->state);
}
void BugTest::set_restingTest()
{
     b->resting = 14;
     a->set_resting(14);
     CPPUNIT_ASSERT_EQUAL(a->resting,b->resting);
     
}
void BugTest::set_directionTest()
{
     b->direction = 1;
     a->set_direction(1);
     CPPUNIT_ASSERT_EQUAL(a->direction,b->direction);
}
void BugTest::set_has_foodTest()
{
     b->has_food = true;
     a->set_has_food(true);
     CPPUNIT_ASSERT_EQUAL(a->has_food,b->has_food);
}
void BugTest::get_colorTest()
{
     CPPUNIT_ASSERT_EQUAL(c->get_color(),0);
}
void BugTest::get_stateTest()
{
     CPPUNIT_ASSERT_EQUAL(c->get_state(),0);
}
void BugTest::get_restingTest()
{
     CPPUNIT_ASSERT_EQUAL(c->get_resting(),14);
}
void BugTest::get_directionTest()
{
         CPPUNIT_ASSERT_EQUAL(c->get_direction(),1);
}
void BugTest::get_has_foodTest()
{
     CPPUNIT_ASSERT_EQUAL(c->get_has_food(),true);
}