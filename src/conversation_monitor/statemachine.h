/**
 *
 * \file statemachine.h
 * \author Mircea Cretu Stancu
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief describes the structure and operation of a state machine
 */

#ifndef _FIPAACL_CONVMONITOR_STATEMACHINE_H_
#define _FIPAACL_CONVMONITOR_STATEMACHINE_H_

#include <fipa_acl/bitefficient_message.h>
#include <fipa_acl/conversation_monitor/state.h>
#include <fipa_acl/conversation_monitor/message_archive.h>
#include <vector>

namespace fipa {
namespace acl {

class MessageArchive;

/**
 * \struct EmbeddedProtocolMapping
 * \brief When embedding protocols, agents participating in one protocol can be mapped to agents participating in the
 * embedded one.
 */
struct EmbeddedProtocolMapping
{
    std::string from;
    Role fromRole;
    std::string to;
    Role toRole;
    /**
     * Convert to string
     */
    std::string toString() const;
};

/**
 * \struct EmbeddedStateMachine
 * \brief An embedded state machine with mappings
 */
struct EmbeddedStateMachine
{
    std::string from;
    Role fromRole;
    std::string stateMachineFile;
    // Cardinality: true means more than 1 Subprotocol can be started at that point
    bool multiple;
    std::vector<EmbeddedProtocolMapping> mappings;
    /**
     * Convert to string
     */
    std::string toString() const;
};

class StateMachine
{
    friend class StateMachineReader; 

    // Initial state of the state machine, i.e. equals current state when starting 
    // the state machine
    StateId mInitialStateId;

    // Current state of the statemachine
    StateId mCurrentStateId;

    /**
     * States that belong to this state machine
     */
    std::map<StateId, State> mStates;

    /**
     * Rolemapping for this state machine
     */
    RoleMapping mRoleMapping;

    /**
     * Message archive
     */
    MessageArchive mMessageArchive;

    /**
     * Generate the default state for this state machine
     */
    void generateDefaultStates();

    /**
     * Generate the default state for this state machine
     */
    void generateDefaultTransitions();

protected:
    /**
     * Check wether the state and transition in this state machine are valid
     * \throws std::runtime_error if the machine cannot be properly validated
     */
    void validate() const;

    /**
     * Reset the current state to the initial state
     */
    void reset();

    /**
     * Set the initial state
     * \param stateId State to which the state machine should be set
     */
    void setInitialState(const StateId& stateId) { mInitialStateId = stateId; }

    /**
     * Add a state to the state machine -- the state might already contain transition definitions
     * in order to check for properly defined transitions, validate needs to be called
     */
    void addState(const State& state);

    /**
     * Given all states update the roles
     */
    void updateRoles();

    /**
     * Update the role mapping
     * \param msg Message
     * \param transition Transition which is associated with the given message
     */
    void updateRoleMapping(const ACLMessage& msg, const Transition& transition);

public:
    /**
     * Get the states mapping
     * \return states container
     */
    std::map<StateId, State> getStates() const { return mStates; }
    
    /**
     * Get the RoleMapping
     */
    const RoleMapping& getRoleMapping() const { return mRoleMapping; }
    
    /**
     * Get current state
     * \throws std::runtime_error if statemachine has not been properly initialized
     */
    const State& getCurrentState() const;

    void setCurrentStateId(const StateId& stateId) { mCurrentStateId = stateId; }

    /**
     * Get the current state
     * \return state id
     */
    StateId getCurrentStateId() const { return mCurrentStateId; }

    /**
     * Get the id initial state
     * \return id of the initial state
     */
    StateId getInitialStateId() const { return mInitialStateId; }


    /**
     * Set self agents id -- can only be called once per state machine
     */
    void setSelf(const AgentID& self);

    /**
     * Consume a message
     * If this is the first message it will initialize the roles for the first state
     * \param msg Message which should be consumed
     * \throws std::runtime_error if message could not be consumed
     */
    void consumeMessage(const ACLMessage& msg);

    /**
     * Check if the state machine is in a final state, i.e. the conversation has ended
     * \returns true if the current state of the state machine is final
     */
    bool inFinalState() const;

    /**
     * Check if state machine is in a known failure state
     * \return true if the current state of the machine is in a (known) error state
     */
    bool inFailureState() const; 

    /**
     * Check wether the conversation has been successfully cancelled
     * \return true if the conversation was successfully cancelled
     */
    bool cancelled() const;

    /**
     * Convert statemachine to string
     */
    std::string toString() const;
};

} // end of acl
} // end of fipa

#endif
