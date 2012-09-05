#include "state.h"
#include "transition.h"
#include "role.h"
#include "statemachine.h"

#include <iostream>
#include <stdexcept>
#include <base/logging.h>

namespace fipa {
namespace acl {

const StateId State::NOT_UNDERSTOOD = "__internal_state:not_understood__";
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

void State::addTransition(Transition& t)
{
    t.setSourceState(mId);
    mTransitions.push_back(t);
}

void State::generateDefaultTransitions()
{
        // Not adding any transition for final states
        if(isFinal())
        {
            return; 
        }

        std::vector<Transition>::iterator it = mTransitions.begin();
        for (; it != mTransitions.end();++it)
        {
	    // we don't generate a not-understood transition for not-understood message...
	    if ( it->getPerformative() == PerformativeTxt[ACLMessage::NOT_UNDERSTOOD] ) 
            {
	        continue;
            }
            Transition defaultTransition = Transition::not_understood(it->getSenderRole(), it->getReceiverRole(), mId);
            addTransition(defaultTransition);
        }
}

bool State::consumeMessage(const ACLMessage &msg, const MessageArchive& archive)
{
    // TODO: consider embedded statemachines
    //if(!mEmbeddedStateMachines.empty())
    //{
    //    // iterator and try to consume message
    //    //
    //    // when using state, make sure all embedded State machines are in final state 
    //    // before leaving the current state
    //}

    std::vector<Transition>::iterator it = mTransitions.begin();
    for (; it != mTransitions.end(); ++it)
    {
        const ACLMessage& initiatingMsg = archive.getCorrespondingInitiatorMessage(msg);
        if (it->triggers(msg, initiatingMsg, mRoleMapping)) 
            return true;
    }
    return false;
}

std::vector<StateMachine> State::getEmbeddedStatemachines() const
{ 
    return mEmbeddedStateMachines;
}

FinalState::FinalState(const StateId& id)
    : State(id)
{
    setFinal(true);
}

UndefinedState::UndefinedState() 
    : State(State::UNDEFINED_ID)
{}

} // end of acl
} // end of fipa
