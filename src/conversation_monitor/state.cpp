#include "state.h"
#include "transition.h"
#include "role.h"
#include "message_archive.h"
#include "statemachine.h"

#include <boost/assign/list_of.hpp>
#include <boost/regex.hpp>
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
    // Not adding any transition for final states without substatemachines
    if(isFinal())
    {
        if(mEmbeddedStateMachines.empty())
        {
            return;
        }
    }
    else if(mTransitions.empty())
    {
        throw std::runtime_error("Invalid state: non final state without transition defined");
    }

    std::vector<Transition> transitions(mTransitions);

    std::vector<Transition>::const_iterator it = transitions.begin();
    for (; it != transitions.end();++it)
    {
        // we don't generate a not-understood transition for not-understood message...
        boost::regex peformativeRegex(it->getPerformativeRegExp());
        if(regex_match(PerformativeTxt[ACLMessage::NOT_UNDERSTOOD], peformativeRegex))
        {
            continue;
        } else {
            // Add the not understood transition for every matching transition
            default_transition::NotUnderstood transitionSender = default_transition::NotUnderstood(it->getSenderRole(), it->getReceiverRole(), mId);
            addTransition(*dynamic_cast<Transition*>(&transitionSender));

            default_transition::NotUnderstood transitionReceiver = default_transition::NotUnderstood(it->getReceiverRole(), it->getSenderRole(), mId);
            addTransition(*dynamic_cast<Transition*>(&transitionReceiver));
        }

        if(regex_match(PerformativeTxt[ACLMessage::CANCEL], peformativeRegex))
        {
            continue;
        } else {
            // Add the cancel transition -- bidirectional regarding receiver/sender role
            default_transition::ConversationCancelling transitionSender = default_transition::ConversationCancelling(it->getSenderRole(), it->getReceiverRole(), mId);
            addTransition(*dynamic_cast<Transition*>(&transitionSender));

            default_transition::ConversationCancelling transitionReceiver = default_transition::ConversationCancelling(it->getReceiverRole(), it->getSenderRole(), mId);
            addTransition(*dynamic_cast<Transition*>(&transitionReceiver));
        }

        if(regex_match(PerformativeTxt[ACLMessage::FAILURE], peformativeRegex))
        {
            continue;
        } else {
            // Add default failure transition -- which only applies for self as receiver
            default_transition::GeneralFailure transitionReceiver = default_transition::GeneralFailure(mId);
            addTransition(*dynamic_cast<Transition*>(&transitionReceiver));
        }
    }
    
    // Also add default states for each substatemachine that proxies
    std::vector<EmbeddedStateMachine>::const_iterator it0 = mEmbeddedStateMachines.begin();
    for (; it0 != mEmbeddedStateMachines.end();++it0)
    {
        if(!it0->proxiedTo.empty())
        {
            // Add the not understood transition for every matching transition
            default_transition::NotUnderstood transitionSender0 = default_transition::NotUnderstood(it0->fromRole, it0->proxiedToRole, mId);
            addTransition(*dynamic_cast<Transition*>(&transitionSender0));
            default_transition::NotUnderstood transitionReceiver0 = default_transition::NotUnderstood(it0->proxiedToRole, it0->fromRole, mId);
            addTransition(*dynamic_cast<Transition*>(&transitionReceiver0));
            
            // Add the cancel transition -- bidirectional regarding receiver/sender role
            default_transition::ConversationCancelling transitionSender1 = default_transition::ConversationCancelling(it0->fromRole, it0->proxiedToRole, mId);
            addTransition(*dynamic_cast<Transition*>(&transitionSender1));
            default_transition::ConversationCancelling transitionReceiver1 = default_transition::ConversationCancelling(it0->proxiedToRole, it0->fromRole, mId);
            addTransition(*dynamic_cast<Transition*>(&transitionReceiver1));
            
            // Add default failure transition -- which only applies for self as receiver
            default_transition::GeneralFailure transitionReceiver2 = default_transition::GeneralFailure(mId);
            addTransition(*dynamic_cast<Transition*>(&transitionReceiver2));
        }
    }
}

bool State::isDefaultState() const
{
    std::vector<StateId>::const_iterator cit = std::find(msDefaultStates.begin(), msDefaultStates.end(), mId);
    return cit != msDefaultStates.end();
}

void State::consumeSubStateMachineMessage(const ACLMessage& msg, const fipa::acl::StateMachine& stateMachine, const fipa::acl::RoleMapping& roleMapping, int numberOfSubConversations)
{
    LOG_INFO("State consumeSubStateMachineMessage");
    
    // Loop through all not-ended sub state machines
    for(std::vector<StateMachine>::iterator it0 = mSubStateMachines.begin(); it0 != mSubStateMachines.end(); it0++)
    {
        if(it0->inFinalState() || it0->inFailureState())
        {
            // StateMachine ended already
            continue;
        }
        // Test if this StateMachine is correct by maintaining a copy of it
        StateMachine copy = *it0;
        
        // Test the update
        try {
            LOG_DEBUG("State consumeSubStateMachineMessage trying an existing sub state machine");
            it0->consumeMessage(msg);
            // It worked
            return;
        } catch(const std::runtime_error& e)
        {
            LOG_DEBUG("State consumeSubStateMachineMessage Sub state machine incorrect: ", e.what());
            // The state machine was obviously not correct, we play back the copy
            *it0 = copy;
        }
    }
    
    LOG_DEBUG("State consumeSubStateMachineMessage trying to search for a fitting a embedded state machine");
    
    const EmbeddedStateMachine* embeddedStateMachinePtr = NULL;
    // We must be in a state that allows subProtocols
    std::string protocol = msg.getProtocol();
    
    // Search for an embedded state machine with the same protocol
    std::vector<EmbeddedStateMachine>::const_iterator it;
    for(it = mEmbeddedStateMachines.begin(); it != mEmbeddedStateMachines.end(); it++)
    {
        // Protocol must match Regex
        boost::regex peformativeRegex(it->name);
        if(regex_match(protocol, peformativeRegex))
        {           
            // Check that the sender role is correct
            // This throws if the mapping does not exist
            const AgentIDList l = roleMapping.getMapping().at(it->fromRole);
            AgentIDList::const_iterator lit = std::find(l.begin(), l.end(), msg.getSender());
            if(lit == l.end())
            {
                // Sender does not match
                continue;
            }
            
            // Check that the number of subconversations allows another one
            if(mSubStateMachines.size() >= numberOfSubConversations)
            {
                continue;
            }
            
            // This is the right ESM
            embeddedStateMachinePtr = &(*it);
            break;
        }
    }
    
    if(!embeddedStateMachinePtr)
    {
        // No fitting running or new embedded state machine found
        LOG_ERROR("State consumeSubStateMachineMessage: No fitting sub state machine found");
        throw std::runtime_error("State consumeSubStateMachineMessage: No fitting sub state machine found");
    }
    
    LOG_DEBUG("State consumeSubStateMachineMessage found a fiting embedded state machine and will try to create a sub state machine now");
    
    // Construct a new state machine with mapped sender role
    if(!protocol.empty())
    {
        // Copy the given state machine
        StateMachine subStateMachine = stateMachine;
        subStateMachine.setSelf(msg.getSender());
        
        // update the message state machine
        try {
            LOG_DEBUG("State consumeSubStateMachineMessage sub state machine initialized, trying to consume message");
            subStateMachine.consumeMessage(msg);
        } catch(const std::runtime_error& e)
        {
            // Also constructing and using a new one did not work for that message.
            std::string errorMsg = "Substatemachine creation failed: ";
            errorMsg += " -- " + std::string(e.what());
            throw std::runtime_error(errorMsg);
        }
        
        // If that was successful, save the actual protocol and number of subconversations in the embedded state machine
        LOG_DEBUG("State consumeSubStateMachineMessage new sub state machine consumed message");
        mSubStateMachines.push_back(subStateMachine);
        embeddedStateMachinePtr->actualProtocol = protocol;
        embeddedStateMachinePtr->numberOfSubConversations = numberOfSubConversations;
    } else {
        LOG_ERROR("Protocol not set");
        throw std::runtime_error("Protocol not set");
    }
    
    LOG_DEBUG("State consumeSubStateMachineMessage successfully created new sub state machine");
}

const Transition& State::getTransition(const ACLMessage &msg, const MessageArchive& archive, const RoleMapping& roleMapping) const
{
    std::vector<Transition>::const_iterator it = mTransitions.begin();
    for (; it != mTransitions.end(); ++it)
    {
        // TODO: better use the directly corresponding one
        // but this should be ok for now
        if(!archive.hasMessages())
        {
            // Initiating message, i.e. validation should only apply to performative
            boost::regex peformativeRegex(it->getPerformativeRegExp());
            if(regex_match(msg.getPerformative(), peformativeRegex))
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

const Transition& State::getSubstateMachineProxiedTransition(const ACLMessage& msg, const MessageArchive& archive, const RoleMapping& roleMapping)
{
    if(archive.hasMessages())
    {
        const ACLMessage& initiatingMsg = archive.getInitiatingMessage();
        // If state has substatemachine(s) && substatemachine(s) proxied_to not empty && actual_protocol != inform && response not already received:
        // Genereate Transition on-the-fly, if posssible
        std::vector<EmbeddedStateMachine>::iterator it0 = mEmbeddedStateMachines.begin();
        for (; it0 != mEmbeddedStateMachines.end(); ++it0)
        {
            LOG_DEBUG("State getSubstateMachineProxiedTransition checking if a transition needs to be generated");
            // FIXME there can be other protocols that do not expect any responses
            if(!it0->proxiedTo.empty() && it0->actualProtocol != "inform" && !it0->receivedProxiedReply )
            {
                LOG_DEBUG("State getSubstateMachineProxiedTransition generating a transition");
                // Generate a transition (any performative, not leaving the state)
                Transition transition (it0->fromRole, it0->proxiedToRole, ".*", getId(), getId());
                // And see if it triggers
                if (transition.triggers(msg, initiatingMsg, roleMapping)) 
                {
                    // Save that a proxied reply was received
                    it0->receivedProxiedReply = true;
                    mSubstateMachineProxiedTransitions.push_back(transition);
                    LOG_DEBUG("State getSubstateMachineProxiedTransition transition triggered");
                    // We cannot use the local var to return as a reference
                    return mSubstateMachineProxiedTransitions.back();
                }
            }
        }
    }

    throw std::runtime_error("Message does not trigger any transition in this state");
}

bool State::isFinished() const
{
    if(!isFinal())
    {
        return false;
    }
    
    // Check all subprotocols
    std::vector<StateMachine>::const_iterator it;
    for(it = mSubStateMachines.begin(); it != mSubStateMachines.end(); it++)
    {
        if(!it->inFinalState() && !it->inFailureState())
        {
            LOG_DEBUG("State not finished (subconversation still running)");
            return false;
        }
    } 
    
    // When there are embedded state machines, they all must have forwarded a proxied reply, if this was
    // necessary in the first place
    std::vector<EmbeddedStateMachine>::const_iterator it0 = mEmbeddedStateMachines.begin();
    for (; it0 != mEmbeddedStateMachines.end(); ++it0)
    {
        // They must also all have started enough sub state machines
        // Check that enough subprotocols have been started
        if(it0->numberOfSubConversations != mSubStateMachines.size())
        {
            LOG_DEBUG("State not finished (subconversation still running)");
            return false;
        }
        
        // FIXME there can be other protocols that do not expect any responses
        if(!it0->proxiedTo.empty() && it0->actualProtocol != "inform" && !it0->receivedProxiedReply )
        {
            LOG_DEBUG("State not finished (proxied response missing)");
            return false;
        }
    }
    
    return true;
}


void State::addEmbeddedStateMachine(EmbeddedStateMachine embeddedStateMachine)
{
    mEmbeddedStateMachines.push_back(embeddedStateMachine);
}

std::string State::toString() const
{
    std::stringstream state;
    state << "state id: '" << mId << "', final: '" << mIsFinal << "'\n";
    for(std::vector<Transition>::const_iterator it = mTransitions.begin(); it != mTransitions.end(); ++it)
    {
        state << "\t" << it->toString() << "\n";
    }
    
    for(std::vector<EmbeddedStateMachine>::const_iterator it = mEmbeddedStateMachines.begin(); it != mEmbeddedStateMachines.end(); ++it)
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
