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

//const State State::undefined()
//{
//    if(!mUndefinedState)
//    {
//        mUndefinedState = boost::shared_ptr<State>(new State(State::UNDEFINED_ID));
//    }
//
//    return *mUndefinedState;
//}

void State::addTransition(Transition& t)
{
    t.setSourceState(mId);
    mTransitions.push_back(t);
}

void State::generateDefaultTransitions()
{
        // Not adding any transition for final states
        if(isFinal())
            return; 

        std::vector<Transition>::iterator it = mTransitions.begin();
        for (; it != mTransitions.end();++it)
        {
	    // we don't generate a not-understood transition for not-understood message...
	    if ( it->getPerformative() == PerformativeTxt[ACLMessage::NOT_UNDERSTOOD] ) 
            {
	        continue;
            }
	 
            // Add a NOT_UNDERSTOOD transition 
	    Transition t; 
	    t.setPerformative(ACLMessage::NOT_UNDERSTOOD);
	    t.setSenderRole(it->getReceiverRole());
	    t.setReceiverRole(it->getSenderRole());
	   
            t.setSourceState(mId); 
	    t.setTargetState(State::NOT_UNDERSTOOD);
	    addTransition(t);	  
        }
}

int State::consumeMessage(const ACLMessage &msg)
{
    LOG_DEBUG("State::consumeMessage state %s", mId.c_str());

    // TODO: embedded statemachine

    std::vector<Transition>::iterator it = mTransitions.begin();
    for (; it != mTransitions.end(); ++it)
    {
        const ACLMessage& initiatingMsg = getLastInitiatorMessage();
        if (it->triggers(msg, initiatingMsg, mRoleMapping)) 
            return true;
    }
    return false;
}

std::vector<StateMachine> State::getEmbeddedStatemachines() const
{ 
    return mEmbeddedStateMachines;
}

const ACLMessage& State::getLastInitiatorMessage() const 
{
    throw std::runtime_error("NOT IMPLEMETED YET");
}

UndefinedState::UndefinedState() 
    : State(State::UNDEFINED_ID)
{}

} // end of acl
} // end of fipa
