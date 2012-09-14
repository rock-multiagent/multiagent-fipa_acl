/** \file: StateMachineTest.h
 * \author: Thomas Roehr, thomas.roehr@dfki.de
 * version: 1.0
 */
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <fipa_acl/fipa_acl.h>
#include <fipa_acl/conversation_monitor.h>
BOOST_AUTO_TEST_SUITE(conversation_monitor_suite)

std::string getProtocolPath()
{
    char buffer[1024];
    BOOST_REQUIRE_MESSAGE( readlink("/proc/self/exe", buffer, 1024) != -1, "Retrieving current execution path");
    std::string str(buffer);
    std::string executionDir = str.substr(0, str.rfind('/'));
    // Assuming we have do a build into build/ parallel to src/ 
    std::string configurationPath = executionDir + "/../../../../configuration/protocols";
    return configurationPath;
}

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
    BOOST_REQUIRE(t.getSourceStateId() == sourceStateId);
    BOOST_REQUIRE(t.getTargetStateId() == targetStateId);

    Transition t0(senderRole, receiverRole, performative, sourceStateId, targetStateId);

    BOOST_REQUIRE(t0.getSenderRole() == senderRole);
    BOOST_REQUIRE(t0.getReceiverRole() == receiverRole);
    BOOST_REQUIRE(t0.getPerformative() == performative);
    BOOST_REQUIRE(t0.getSourceStateId() == sourceStateId);
    BOOST_REQUIRE(t0.getTargetStateId() == targetStateId);

    ACLMessage inputMsg(performative);
    ACLMessage validatorMsg(performative);
    RoleMapping roleMapping;

    AgentID self(".*");
    AgentID other(".*");
    // Set roles
    roleMapping.addRole(senderRole);
    roleMapping.addRole(receiverRole);
    roleMapping.addExpectedAgent(senderRole, self);
    roleMapping.addExpectedAgent(receiverRole, other);
    // Failing since list of receivers is empty
    BOOST_REQUIRE_THROW( t.validateMessage(inputMsg, validatorMsg, roleMapping), std::runtime_error );
    inputMsg.addReceiver(other);
    // Check whether current message conforms to roleMapping -- should not matter since all fields 
    // are empty
    BOOST_REQUIRE( t.validateMessage(inputMsg, validatorMsg, roleMapping));

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

    //--------------- Testing flags

    roleMapping.clearExpectedAgents(senderRole);
    roleMapping.clearExpectedAgents(receiverRole);
    //
    // Should fail, since the roles sender and receiver are not set
    ACLMessage input;
    ACLMessage validator;
    BOOST_REQUIRE_THROW( t.validateMessage(input, validator, roleMapping, validation::RECEIVERS), std::runtime_error );

    BOOST_REQUIRE( t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::NONE) );

    // Test performative
    inputMsg.setPerformative(ACLMessage::INFORM);
    t.setPerformative(ACLMessage::INFORM);
    validatorMsg.setPerformative(ACLMessage::REQUEST);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::PERFORMATIVE) );
    // Transition's performative is relevant, not the validatorMsg one
    t.setPerformative(ACLMessage::REQUEST);
    BOOST_REQUIRE(!t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::PERFORMATIVE) );

    t.setPerformative(ACLMessage::INFORM);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::PERFORMATIVE) );

    std::string testString = "test";
    std::string testOtherString = "test-other";

    AgentID senderAgentID("senderAgent");
    AgentID receiverAgentID("receiverAgent");
    // Test sender

    inputMsg.setSender(senderAgentID);
    roleMapping.addExpectedAgent(t.getSenderRole(), receiverAgentID);
    BOOST_REQUIRE(! t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::SENDER));

    roleMapping.clearExpectedAgents(t.getSenderRole());
    roleMapping.addExpectedAgent(t.getSenderRole(),senderAgentID);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::SENDER));
    roleMapping.clearExpectedAgents(t.getSenderRole());

    // Test receiver
    roleMapping.addExpectedAgent(t.getReceiverRole(), receiverAgentID);
    roleMapping.addExpectedAgent(t.getReceiverRole(), senderAgentID);
    // Event though the list of receiver agent might be a subset of the expected ones
    // the statemachine has to make sure that before a state transition takes place 
    // all potential receivers have been dealt with
    inputMsg.clearReceivers();
    inputMsg.addReceiver(senderAgentID);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::RECEIVERS));

    inputMsg.addReceiver(senderAgentID);
    roleMapping.addExpectedAgent(t.getReceiverRole(), senderAgentID);

    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::RECEIVERS));
    roleMapping.clearExpectedAgents(t.getReceiverRole());

    // Test ConversationID flag
    inputMsg.setConversationID(testString);
    validatorMsg.setConversationID(testOtherString);
    BOOST_REQUIRE(! t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::CONVERSATION_ID) );

    validatorMsg.setConversationID(testString);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::CONVERSATION_ID) );
    validatorMsg.setConversationID(testOtherString);

    // Test protocol flag
    inputMsg.setProtocol(testString);
    validatorMsg.setProtocol(testOtherString);
    BOOST_REQUIRE(! t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::PROTOCOL) );

    validatorMsg.setProtocol(testString);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::PROTOCOL) );
    validatorMsg.setProtocol(testOtherString);

    // Test encoding flag
    inputMsg.setEncoding(testString);
    validatorMsg.setEncoding(testOtherString);
    BOOST_REQUIRE(! t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::ENCODING) );

    validatorMsg.setEncoding(testString);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::ENCODING) );
    validatorMsg.setEncoding(testOtherString);

    // Test Language flag
    inputMsg.setLanguage(testString);
    validatorMsg.setLanguage(testOtherString);
    BOOST_REQUIRE(! t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::LANGUAGE) );

    validatorMsg.setLanguage(testString);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::LANGUAGE) );
    validatorMsg.setLanguage(testOtherString);

    // Test Ontology flag
    inputMsg.setOntology(testString);
    validatorMsg.setOntology(testOtherString);
    BOOST_REQUIRE(! t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::ONTOLOGY) );

    validatorMsg.setOntology(testString);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::ONTOLOGY) );
    validatorMsg.setOntology(testOtherString);

    // Test InReplyTo flag
    inputMsg.setInReplyTo(testString);
    validatorMsg.setInReplyTo(testOtherString);
    BOOST_REQUIRE(! t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::IN_REPLY_TO) );

    validatorMsg.setInReplyTo(testString);
    BOOST_REQUIRE(t.validateMessage(inputMsg, validatorMsg, roleMapping, validation::IN_REPLY_TO) );
    validatorMsg.setInReplyTo(testOtherString);

}

BOOST_AUTO_TEST_CASE(state_test)
{
    using namespace fipa::acl;
    
    StateId stateId = "test-state";
    StateId otherStateId = "other-state";
    State state(stateId);

    BOOST_REQUIRE(state.getId() == stateId);

    state.setId(otherStateId);
    BOOST_REQUIRE(state.getId() == otherStateId);

    BOOST_REQUIRE(!state.isFinal());
    // non-final and no transitions defined - invalid state
    BOOST_REQUIRE_THROW(state.generateDefaultTransitions(), std::runtime_error);

    RoleId senderRoleId = "sender";
    RoleId receiverRoleId = "receiver";
    Role senderRole(senderRoleId);
    Role receiverRole(receiverRoleId);
    Transition t(senderRole, receiverRole, ACLMessage::INFORM, stateId, otherStateId);
    state.addTransition(t);
    state.generateDefaultTransitions();

    // Test for default transitions being added
    std::vector<Transition> transitions = state.getTransitions();
    BOOST_REQUIRE(transitions.size() > 1);

    // Verify final state
    FinalState finalState(stateId);
    BOOST_REQUIRE(finalState.isFinal());

    transitions = finalState.getTransitions();
    BOOST_REQUIRE(transitions.empty());
}


BOOST_AUTO_TEST_CASE(statemachine_reader_test)
{
    using namespace fipa::acl;

    std::string configurationPath = getProtocolPath();

    StateMachineReader reader;
    BOOST_CHECK_MESSAGE(1, "NOTE: Assuming a this test resides in a build directory parallel to the folder configuration/");
    BOOST_REQUIRE_NO_THROW(reader.loadSpecification(configurationPath + "/inform"));
    BOOST_REQUIRE_NO_THROW(reader.loadSpecification(configurationPath + "/dutchAuction"));
    BOOST_REQUIRE_NO_THROW(reader.loadSpecification(configurationPath + "/subscribe"));

    StateMachine statemachine = reader.loadSpecification(configurationPath + "/inform");

}

BOOST_AUTO_TEST_CASE(statemachine_test)
{
    using namespace fipa::acl;

    std::string configurationPath = getProtocolPath();
    StateMachineFactory::setProtocolResourceDir(configurationPath);
    {
        StateMachine inform = StateMachineFactory::getStateMachine("inform");

        ACLMessage msg(ACLMessage::REQUEST);
        BOOST_REQUIRE_THROW(inform.consumeMessage(msg), std::runtime_error);
        BOOST_REQUIRE(!inform.inFailureState());

        inform = StateMachineFactory::getStateMachine("inform");
        ACLMessage informMsg(ACLMessage::INFORM);
        BOOST_REQUIRE_NO_THROW(inform.consumeMessage(informMsg));
        BOOST_REQUIRE(!inform.inFailureState());
    }

    {
        // Trying request
        // request ->
        // final <- refuse
        // <- agree
        // <- failure/inform
        StateMachine request = StateMachineFactory::getStateMachine("request");
        AgentID self("self");
        AgentID other("other");
        request.setSelf(self);
        {
            ACLMessage msg(ACLMessage::REQUEST);
            msg.setSender(self);
            msg.addReceiver(other);
            BOOST_REQUIRE(request.getCurrentStateId() == "1");
            BOOST_REQUIRE_NO_THROW(request.consumeMessage(msg));
            BOOST_REQUIRE(request.getCurrentStateId() == "2");
            BOOST_REQUIRE(!request.inFinalState());
            BOOST_REQUIRE(!request.inFailureState());
        }

        {
            ACLMessage msg(ACLMessage::REFUSE);
            msg.setSender(other);
            msg.addReceiver(self);
            msg.setLanguage("language");
            BOOST_REQUIRE_THROW(request.consumeMessage(msg), std::runtime_error);
            BOOST_REQUIRE(request.getCurrentStateId() == "2");
            BOOST_REQUIRE(!request.inFailureState());
        }

        {
            ACLMessage msg(ACLMessage::REFUSE);
            msg.setSender(self);
            msg.addReceiver(other);
            BOOST_REQUIRE_THROW(request.consumeMessage(msg), std::runtime_error);
            BOOST_REQUIRE(request.getCurrentStateId() == "2");
            BOOST_REQUIRE(!request.inFailureState());
        }

        {
            ACLMessage msg(ACLMessage::REFUSE);
            msg.setSender(other);
            msg.addReceiver(self);
            BOOST_REQUIRE_NO_THROW(request.consumeMessage(msg));
            BOOST_REQUIRE(request.getCurrentStateId() == "3");
            BOOST_REQUIRE(request.inFinalState());
            BOOST_REQUIRE(!request.inFailureState());
        }
    }

    {
        // Trying request
        // request ->
        // final <- refuse
        // <- agree
        // <- failure/inform
        StateMachine request = StateMachineFactory::getStateMachine("request");
        AgentID self("self");
        AgentID other("other");
        request.setSelf(self);
        {
            ACLMessage msg(ACLMessage::REQUEST);
            msg.setSender(self);
            msg.addReceiver(other);
            BOOST_REQUIRE_NO_THROW(request.consumeMessage(msg));
            BOOST_REQUIRE(request.getCurrentStateId() == "2");
            BOOST_REQUIRE(!request.inFinalState());
            BOOST_REQUIRE(!request.inFailureState());
        }

        {
            ACLMessage msg(ACLMessage::AGREE);
            msg.setSender(other);
            msg.addReceiver(self);
            BOOST_REQUIRE_NO_THROW(request.consumeMessage(msg));
            BOOST_REQUIRE(request.getCurrentStateId() == "4");
            BOOST_REQUIRE(!request.inFinalState());
            BOOST_REQUIRE(!request.inFailureState());
        }

        {
            ACLMessage msg(ACLMessage::INFORM);
            msg.setSender(other);
            msg.addReceiver(self);
            BOOST_REQUIRE_NO_THROW(request.consumeMessage(msg));
            BOOST_REQUIRE(request.getCurrentStateId() == "5");
            BOOST_REQUIRE(request.inFinalState());
            BOOST_REQUIRE(!request.inFailureState());
        }
    }

    {
        // Trying request
        // request ->
        // final <- refuse
        // <- agree
        // <- failure/inform
        StateMachine request = StateMachineFactory::getStateMachine("request");
        AgentID self("self");
        AgentID other("other");
        AgentID foreign("foreign");
        request.setSelf(self);
        {
            ACLMessage msg(ACLMessage::REQUEST);
            msg.setSender(self);
            msg.addReceiver(other);
            BOOST_REQUIRE_NO_THROW(request.consumeMessage(msg));
            BOOST_REQUIRE(request.getCurrentStateId() == "2");
            BOOST_REQUIRE(!request.inFinalState());
            BOOST_REQUIRE(!request.inFailureState());
        }

        {
            ACLMessage msg(ACLMessage::FAILURE);
            msg.setSender(foreign);
            msg.addReceiver(self);
            BOOST_REQUIRE_NO_THROW(request.consumeMessage(msg));
            default_state::GeneralFailure failureState;
            BOOST_REQUIRE(request.getCurrentStateId() == failureState.getId());
            BOOST_REQUIRE(request.inFinalState());
            BOOST_REQUIRE(request.inFailureState());
        }
    }

    {
        // Trying request
        // request ->
        // final <- refuse
        // <- agree
        // <- failure/inform
        StateMachine request = StateMachineFactory::getStateMachine("request");
        AgentID self("self");
        AgentID other("other");
        AgentID foreign("foreign");
        request.setSelf(self);
        {
            ACLMessage msg(ACLMessage::REQUEST);
            msg.setSender(self);
            msg.addReceiver(other);
            BOOST_REQUIRE_NO_THROW(request.consumeMessage(msg));
            BOOST_REQUIRE(request.getCurrentStateId() == "2");
            BOOST_REQUIRE(!request.inFinalState());
            BOOST_REQUIRE(!request.inFailureState());
        }

        {
            ACLMessage msg(ACLMessage::FAILURE);
            msg.setSender(foreign);
            msg.addReceiver(other);
            BOOST_REQUIRE_THROW(request.consumeMessage(msg), std::runtime_error);
            BOOST_REQUIRE(request.getCurrentStateId() == "2");
            BOOST_REQUIRE(!request.inFinalState());
            BOOST_REQUIRE(!request.inFailureState());
        }
    }

}
BOOST_AUTO_TEST_SUITE_END()

