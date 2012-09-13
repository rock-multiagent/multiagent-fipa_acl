/*
big TODO(for me - not relevant right now but for the general case): line 203(behaviour in case a conversation is successfully cancelled);

note for future tweak: make it possible that once a machine has been built, if we try to start it with a message and it fails,
		   make it possible to be able to attempt to start it again during runtime(the same machine - currently it needs rebuilding)

note for future: the "in reply to" policy needs to be defined somehow (not necessarily important for this stage); we need to define 
	       in which contexts the "in reply to" field must be filled in in order to correctly validate a message; 
	       
	       current policy is: if the field is not filled in then it means it should not be(thus considerred valid)
	       
	       another intuitive policy might be(not yet sure whether it stands for the general case): check all the messages 
	       from preceding state and if there if there is no message TO the current sender with a "in reply to" field filled in, 
	       then it should not be filled in
*/
#include "statemachine.h"
#include "state.h"
#include "transition.h"

#include <iostream>
#include <algorithm>
#include <set>
#include <stdexcept>
#include <base/logging.h>

namespace fipa {
namespace acl {

void StateMachine::validate() const
{
    // Testing whether there are any undefined state referred to 
    // Selection of unique state ids
    std::set<StateId> referencedStates;
    std::map<StateId, State>::const_iterator statesIt = mStates.begin();

    for(; statesIt != mStates.end(); ++statesIt)
    {
        const State& state = statesIt->second;
        std::vector<Transition> transitions = state.getTransitions();
        std::vector<Transition>::const_iterator transitionsIt = transitions.begin();

        for(; transitionsIt != transitions.end(); ++transitionsIt)
        {
            referencedStates.insert(transitionsIt->getTargetStateId());
            referencedStates.insert(transitionsIt->getSourceStateId());
        }
    }

    std::set<StateId>::const_iterator referencedStatesIt = referencedStates.begin();
    for(; referencedStatesIt != referencedStates.end(); ++referencedStatesIt)
    {
        statesIt = mStates.find(*referencedStatesIt);
        if(statesIt == mStates.end())
        {
            std::string msg = "Unknown state '" + *referencedStatesIt + "' referred to in statemachine";
            LOG_ERROR("%s", msg.c_str());
            throw std::runtime_error(msg);
        }
    }
}

void StateMachine::reset()
{
    mCurrentStateId = mInitialStateId;
}

void StateMachine::addState(const State& state)
{
    std::map<StateId, State>::const_iterator it = mStates.find(state.getId());
    if(it != mStates.end())
    {
        std::string msg = "State '" + state.getId() + "' already defined";
        LOG_ERROR("%s", msg.c_str());
        throw std::runtime_error(msg);
    } else {
        mStates[state.getId()] = state;
    }
}

void StateMachine::updateRoles()
{
    std::map<StateId, State>::iterator it = mStates.begin();
    for(; it != mStates.end(); ++it)
    {
        std::vector<Transition> transitions = it->second.getTransitions();
        std::vector<Transition>::const_iterator transitionIt = transitions.begin();
        for(; transitionIt != transitions.end(); ++transitionIt)
        {
            mRoleMapping.addRole(transitionIt->getSenderRole());
            mRoleMapping.addRole(transitionIt->getReceiverRole());
        }
    }

}

void StateMachine::generateDefaultTransitions()
{
    std::map<StateId, State>::iterator it = mStates.begin();
    for(; it != mStates.end(); ++it)
    {
        // applying not-understood and cancel make only sense after having started a conversation
        if(it->first != mInitialStateId)
        {
            it->second.generateDefaultTransitions();
        }
    }
}

void StateMachine::generateDefaultStates()
{
    try {
        // adding the default states 
        default_state::NotUnderstood notUnderstood;
        addState(*dynamic_cast<State*>(&notUnderstood));

        default_state::ConversationCancelling cancelling;
        addState(*dynamic_cast<State*>(&cancelling));

        default_state::ConversationCancelSuccess cancelSuccess;
        addState(*dynamic_cast<State*>(&cancelSuccess));

        default_state::ConversationCancelFailure cancelFailure;
        addState(*dynamic_cast<State*>(&cancelFailure));
    } catch (const std::runtime_error& e)
    {
        LOG_FATAL("%s", e.what());
        // The states should be unqiue
        assert(false);
    }
}

const State& StateMachine::getCurrentState() const
{
    std::map<StateId, State>::const_iterator it = mStates.find(mCurrentStateId);
    assert(it != mStates.end());
    return it->second;
}

void StateMachine::setSelf(const AgentID& self)
{
    mRoleMapping.setSelf(self);
}

void StateMachine::updateRoleMapping(const ACLMessage& msg, const Transition& transition)
{
    mRoleMapping.addExpectedAgent(transition.getSenderRole(), msg.getSender());
    AgentIDList receivers = msg.getAllReceivers();
    AgentIDList::const_iterator it = receivers.begin();
    for(; it != receivers.end(); ++it)
    {
        mRoleMapping.addExpectedAgent(transition.getReceiverRole(), *it);
    }
}

void StateMachine::consumeMessage(const ACLMessage& msg)
{
    const State& currentState = getCurrentState();

    const Transition& transition = currentState.getTransition(msg, mMessageArchive, mRoleMapping);
    updateRoleMapping(msg, transition);
    mMessageArchive.addMessage(msg);
}

bool StateMachine::inFinalState() const
{
    const State& currentState = getCurrentState();
    return currentState.isFinal();
}

bool StateMachine::cancelled() const
{
    return mCurrentStateId == State::CONVERSATION_CANCEL_SUCCESS;
}

std::string StateMachine::toString() const
{
    std::stringstream statemachine;
    std::map<StateId, State>::const_iterator it = mStates.begin();
    for(; it != mStates.end(); ++it)
    {
        statemachine << it->second.toString();
    }
    return statemachine.str();
}

} // end of acl
} // end of fipa
