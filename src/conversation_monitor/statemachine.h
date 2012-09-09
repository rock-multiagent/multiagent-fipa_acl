/**
 *
 * \file statemachine.h
 * \author Mircea Cretu Stancu
 * \brief describes the structure and operation of a state machine
 * 
 * \version 1.0
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
     * Get current state
     */
    const State& getCurrentState() const;

    /**
     * Update the role mapping
     * \param msg Message
     * \param state State that was successfully left
     */
    void updateRoleMapping(const ACLMessage& msg, const Transition& transition);

public:
    /**
     * Set the current state
     * \param stateId State to which the state machine should be set
     */
    void setCurrentStateId(const StateId& stateId) { mCurrentStateId = stateId; }

    /**
     * Get the current state
     * \return state id
     */
    StateId getCurrentStateId() const { return mCurrentStateId; }

    /**
     * Set self agents id -- can only be called once per state machine
     */
    void setSelfAgentId(const AgentID& self);

    /**
     * Consume a message
     * If this is the first message it will initialize the roles for the first state
     * \param msg Message which should be consumed
     * \param agent that is self
     * \throws std::runtime_error if message could not be consumed
     */
    void consumeMessage(const ACLMessage& msg);

    /**
     * Check if the state machine is in a final state, i.e. the conversation has ended
     * \returns true if the current state of the state machine is final
     */
    bool inFinalState() const;

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
