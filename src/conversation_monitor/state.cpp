#include "state.h"
#include "transition.h"
#include "role.h"
#include "statemachine.h"

#include <iostream>
#include <stdexcept>
#include <base/logging.h>
#include <sstream>

namespace fipa {
namespace acl {

const StateId State::NOT_UNDERSTOOD = "__internal_state:not_understood__";
const StateId State::CONVERSATION_CANCELLING = "__internal_state:conversation_cancelling__";
const StateId State::CONVERSATION_CANCEL_SUCCESS = "__internal_state:conversation_cancel_success__";
const StateId State::CONVERSATION_CANCEL_FAILURE = "__internal_state:conversation_cancel_failure__";
const StateId State::UNDEFINED_ID = "__undefined__";

State::State() 
    : mId(State::UNDEFINED_ID)
    , mIsFinal(false)
{
}

State::State(const StateId& uid) 
    : mId(uid)
    , mIsFinal(false)
{
}

Transition State::addTransition(const Transition& t)
{
    std::vector<Transition>::const_iterator it;
    it = std::find(mTransitions.begin(), mTransitions.end(), t);
    if( it == mTransitions.end())
    {
        Transition transition = t;
        transition.setSourceState(mId);
        mTransitions.push_back(transition);
        return transition;
    } else {
        return *it;
    }
}

void State::generateDefaultTransitions()
{
    // Not adding any transition for final states
    if(isFinal())
    {
        return; 
    }

    if(mTransitions.empty())
    {
        throw std::runtime_error("Invalid state: non final state without transition defined");
    }

    std::vector<Transition> transitions(mTransitions);

    std::vector<Transition>::const_iterator it = transitions.begin();
    for (; it != transitions.end();++it)
    {
        // we don't generate a not-understood transition for not-understood message...
        if ( it->getPerformative() == ACLMessage::NOT_UNDERSTOOD ) 
        {
            continue;
        } else {
            // Add the not understood transition for every matching transition
            default_transition::NotUnderstood transitionSender = default_transition::NotUnderstood(it->getSenderRole(), it->getReceiverRole(), mId);
            addTransition(*dynamic_cast<Transition*>(&transitionSender));

            default_transition::NotUnderstood transitionReceiver = default_transition::NotUnderstood(it->getReceiverRole(), it->getSenderRole(), mId);
            addTransition(*dynamic_cast<Transition*>(&transitionReceiver));
        }

        if ( it->getPerformative() == ACLMessage::CANCEL )
        {
            continue;
        } else {
            // Add the cancel transition -- bidirectional regarding receiver/sender role
            default_transition::ConversationCancelling transitionSender = default_transition::ConversationCancelling(it->getSenderRole(), it->getReceiverRole(), mId);
            addTransition(*dynamic_cast<Transition*>(&transitionSender));

            default_transition::ConversationCancelling transitionReceiver = default_transition::ConversationCancelling(it->getReceiverRole(), it->getSenderRole(), mId);
            addTransition(*dynamic_cast<Transition*>(&transitionReceiver));
        }
    }
}

StateId State::transitionTarget(const ACLMessage &msg, const MessageArchive& archive)
{
    // TODO: consider embedded statemachines
    if(!mEmbeddedStateMachines.empty())
    {
        // iterator and try to consume message
        std::vector<StateMachine>::iterator it = mEmbeddedStateMachines.begin();
        for(; it != mEmbeddedStateMachines.end(); ++it)
        {
            if(it->consumeMessage(msg))
            {
                break;
            }
        }

        //
        // when using state, make sure all embedded State machines are in final state 
        // before leaving the current state
    }

    std::vector<Transition>::iterator it = mTransitions.begin();
    for (; it != mTransitions.end(); ++it)
    {
        const ACLMessage& initiatingMsg = archive.getCorrespondingInitiatorMessage(msg);
        if (it->triggers(msg, initiatingMsg, mRoleMapping)) 
        {
            return it->getTargetStateId();
        }
    }

    throw std::runtime_error("Message does not match to state");

}

std::vector<StateMachine> State::getEmbeddedStatemachines() const
{ 
    return mEmbeddedStateMachines;
}

std::string State::toString() const
{
    std::stringstream state;
    state << "state id: '" << mId << "', final: '" << mIsFinal << "'\n";
    std::vector<Transition>::const_iterator it = mTransitions.begin();
    for(; it != mTransitions.end(); ++it)
    {
        state << "\t" << it->toString() << "\n";
    }
    return state.str();
}

FinalState::FinalState(const StateId& id)
    : State(id)
{
    setFinal(true);
}

UndefinedState::UndefinedState() 
    : State(State::UNDEFINED_ID)
{}

namespace default_state {

ConversationCancelling::ConversationCancelling() 
    : State(State::CONVERSATION_CANCELLING)
{
    // Add the cancel transitions -> success or failure
    default_transition::ConversationCancelSuccess transitionSuccess;
    addTransition(*dynamic_cast<Transition*>(&transitionSuccess));

    default_transition::ConversationCancelFailure transitionFailure;
    addTransition(*dynamic_cast<Transition*>(&transitionFailure));
}

}

} // end of acl
} // end of fipa
