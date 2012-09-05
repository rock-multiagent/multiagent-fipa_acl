/** \file: StateMachineTest.h
 * \author: Thomas Roehr, thomas.roehr@dfki.de
 * version: 1.0
 */
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <fipa_acl/fipa_acl.h>
#include <fipa_acl/conversation_monitor.h>
BOOST_AUTO_TEST_SUITE(conversation_monitor_suite)

BOOST_AUTO_TEST_CASE(transition_test)
{
    using namespace fipa::acl;

    Transition t;
    Role senderRole("sender");
    Role receiverRole("receiver"); 
    ACLMessage::Performative performative = ACLMessage::REQUEST;
    StateId sourceStateId = "sourceState";
    StateId targetStateId = "targetState";

    t.setSenderRole(senderRole);
    t.setReceiverRole(receiverRole);
    t.setPerformative(performative);
    t.setSourceState(sourceStateId);
    t.setTargetState(targetStateId);

    BOOST_REQUIRE(t.getSenderRole() == senderRole);
    BOOST_REQUIRE(t.getReceiverRole() == receiverRole);
    BOOST_REQUIRE(t.getPerformative() == performative);
    BOOST_REQUIRE(t.getSourceState() == sourceStateId);
    BOOST_REQUIRE(t.getTargetState() == targetStateId);

    ACLMessage inputMsg(performative);
    ACLMessage validatorMsg(performative);
    RoleMapping roleMapping;
    // Should fail, since the roles sender and receiver are not set
    BOOST_REQUIRE_THROW( t.validateMessage(inputMsg, validatorMsg, roleMapping), std::runtime_error );

    AgentID self(".*");
    AgentID other(".*");
    // Set roles
    roleMapping.addRole(senderRole);
    roleMapping.addRole(receiverRole);
    roleMapping.addExpectedAgent(senderRole, self);
    roleMapping.addExpectedAgent(receiverRole, other);
    // Check whether current message conforms to roleMapping -- should not matter since all fields 
    // are empty
    BOOST_REQUIRE( t.validateMessage(inputMsg, validatorMsg, roleMapping) );



    // Prepare the current role mapping -- as it should be, i.e. expected
    roleMapping.setSelf(self);
    roleMapping.addRole(senderRole);
    roleMapping.addRole(receiverRole);
    roleMapping.addExpectedAgent(senderRole, self);
    roleMapping.addExpectedAgent(receiverRole, other);

    // Prepare the current messages sender and receiver
    inputMsg.setSender(self);
    inputMsg.addReceiver(other);

    // sender and receiver are taken from the roleMapping, i.e. sender receiver field is ignored from the 
    // validator message
    BOOST_REQUIRE( t.validateMessage(inputMsg, validatorMsg, roleMapping) );

}
BOOST_AUTO_TEST_SUITE_END()

