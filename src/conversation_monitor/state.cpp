#include "state.h"
#include "transition.h"
#include "role.h"
#include "message_archive.h"
#include "statemachine.h"

#include <boost/assign/list_of.hpp>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <base/logging.h>

namespace fipa {
namespace acl {

const StateId State::NOT_UNDERSTOOD = "__internal_state:not_understood__";
const StateId State::CONVERSATION_CANCELLING = "__internal_state:conversation_cancelling__";
const StateId State::CONVERSATION_CANCEL_SUCCESS = "__internal_state:conversation_cancel_success__";
const StateId State::CONVERSATION_CANCEL_FAILURE = "__internal_state:conversation_cancel_failure__";
const StateId State::UNDEFINED_ID = "__undefined__";
const StateId State::GENERAL_FAILURE_STATE = "__internal_state:general_failure__";

std::vector<StateId> State::msDefaultStates = boost::assign::list_of
    (State::NOT_UNDERSTOOD)
    (State::CONVERSATION_CANCELLING)
    (State::CONVERSATION_CANCEL_SUCCESS)
    (State::CONVERSATION_CANCEL_FAILURE)
    (State::UNDEFINED_ID)
    (State::GENERAL_FAILURE_STATE)
    ;

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

        if( it->getPerformative() == ACLMessage::FAILURE)
        {
            continue;
        } else {
            // Add default failure transition -- which only applies for self as receiver
            default_transition::GeneralFailure transitionReceiver = default_transition::GeneralFailure(mId);
            addTransition(*dynamic_cast<Transition*>(&transitionReceiver));
        }
    }
}

bool State::isDefaultState() const
{
    std::vector<StateId>::const_iterator cit = std::find(msDefaultStates.begin(), msDefaultStates.end(), mId);
    return cit != msDefaultStates.end();
}

const Transition& State::getTransition(const ACLMessage &msg, const MessageArchive& archive, const RoleMapping& roleMapping) const
{
    // TODO: consider embedded statemachines

    std::vector<Transition>::const_iterator it = mTransitions.begin();
    for (; it != mTransitions.end(); ++it)
    {
        // TODO: better use the directly corresponding one
        // but this should be ok for now
        if(!archive.hasMessages())
        {
            // Initiating message, i.e. validation should only apply to performative
            if (it->getPerformative() == ACLMessage::performativeFromString(msg.getPerformative()) )
            {
                return *it;
            }
        } else {
            const ACLMessage& initiatingMsg = archive.getInitiatingMessage();
            if (it->triggers(msg, initiatingMsg, roleMapping)) 
            {
                return *it;
            }
        }
    }

    throw std::runtime_error("Message does not trigger any transition in this state");

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
