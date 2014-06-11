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
#include <boost/regex.hpp>
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
        it->second.generateDefaultTransitions();
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

        default_state::GeneralFailure generalFailure;
        addState(*dynamic_cast<State*>(&generalFailure));

    } catch (const std::runtime_error& e)
    {
        LOG_FATAL("%s", e.what());
        // The states should be unqiue
        assert(false);
    }
}

const State& StateMachine::getCurrentState() const
{
    if(mCurrentStateId.empty())
    {
        throw std::runtime_error("Statemachine has not been properly initialized: current state not set");
    } else {
        std::map<StateId, State>::const_iterator it = mStates.find(mCurrentStateId);
        if(it == mStates.end())
        {
            throw std::runtime_error("Statemachine has not been properly initialized: current state not found");
        }
        return it->second;
    }
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

void StateMachine::consumeSubStateMachineMessage(const ACLMessage& msg, const StateMachine& stateMachine, int numberOfSubConversations)
{
    LOG_DEBUG("StateMachine consumeSubStateMachineMessage");
    
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
            LOG_DEBUG("StateMachine consumeSubStateMachineMessage trying an existing sub state machine");
            it0->consumeMessage(msg);
            // It worked
            return;
        } catch(const std::runtime_error& e)
        {
            LOG_DEBUG("StateMachine consumeSubStateMachineMessage Sub state machine incorrect: ", e.what());
            // The state machine was obviously not correct, we play back the copy
            *it0 = copy;
        }
    }
    
    LOG_DEBUG("StateMachine consumeSubStateMachineMessage trying to search for a fitting a embedded state machine");
    
    EmbeddedStateMachine* embeddedStateMachinePtr = NULL;
    // We must be in a state that allows subProtocols
    std::string protocol = msg.getProtocol();
    
    // Search for an embedded state machine with the same protocol
    std::vector<EmbeddedStateMachine>::iterator it;
    for(it = mEmbeddedStateMachines.begin(); it != mEmbeddedStateMachines.end(); it++)
    {
        // Protocol must match Regex
        boost::regex peformativeRegex(it->name);
        if(regex_match(protocol, peformativeRegex))
        {           
            // Check that the sender role is correct
            // This throws if the mapping does not exist
            const AgentIDList l = mRoleMapping.getMapping().at(it->fromRole);
            AgentIDList::const_iterator lit = std::find(l.begin(), l.end(), msg.getSender());
            if(lit == l.end())
            {
                // Sender does not match
                continue;
            }
            
            // Check that we are in its starting state
            if(getCurrentStateId() != it->startState)
            {
                continue;
            }
            
            // Check that the number of subconversations allows another one
            if(((int) mSubStateMachines.size()) >= numberOfSubConversations)
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
        LOG_ERROR("StateMachine consumeSubStateMachineMessage: No fitting sub state machine found");
        throw std::runtime_error("State consumeSubStateMachineMessage: No fitting sub state machine found");
    }
    
    LOG_DEBUG("StateMachine consumeSubStateMachineMessage found a fiting embedded state machine and will try to create a sub state machine now");
    
    // Construct a new state machine with mapped sender role
    if(!protocol.empty())
    {
        // Copy the given state machine
        StateMachine subStateMachine = stateMachine;
        subStateMachine.setSelf(msg.getSender());
        
        // update the message state machine
        try {
            LOG_DEBUG("StateMachine consumeSubStateMachineMessage sub state machine initialized, trying to consume message");
            subStateMachine.consumeMessage(msg);
        } catch(const std::runtime_error& e)
        {
            // Also constructing and using a new one did not work for that message.
            std::string errorMsg = "Substatemachine creation failed: ";
            errorMsg += " -- " + std::string(e.what());
            throw std::runtime_error(errorMsg);
        }
        
        // If that was successful, save the actual protocol and number of subconversations in the embedded state machine
        LOG_DEBUG("StateMachine consumeSubStateMachineMessage new sub state machine consumed message");
        mSubStateMachines.push_back(subStateMachine);
        embeddedStateMachinePtr->actualProtocol = protocol;
        embeddedStateMachinePtr->numberOfSubConversations = numberOfSubConversations;
    } else {
        LOG_ERROR("Protocol not set");
        throw std::runtime_error("Protocol not set");
    }
    
    LOG_DEBUG("StateMachine consumeSubStateMachineMessage successfully created new sub state machine");
}

void StateMachine::consumeMessage(const ACLMessage& msg)
{
    LOG_DEBUG("StateMachine consumeMessage");
    
    try
    {
        const State& currentState = getCurrentState();
        const Transition& transition = currentState.getTransition(msg, mMessageArchive, mRoleMapping);
        updateRoleMapping(msg, transition);
        mMessageArchive.addMessage(msg);

        // Perform transition
        mCurrentStateId = transition.getTargetStateId();
    }
    catch(const std::exception& e)
    {
        LOG_DEBUG("StateMachine consumeMessage trying substatemachine proxied transition");
        // Retry with substatemachineproxied transition
        const Transition& transition = getSubstateMachineProxiedTransition(msg, mMessageArchive, mRoleMapping);
        updateRoleMapping(msg, transition);
        mMessageArchive.addMessage(msg);

        // Perform transition
        mCurrentStateId = transition.getTargetStateId();
    }
}

const Transition& StateMachine::getSubstateMachineProxiedTransition(const ACLMessage& msg, const MessageArchive& archive, const RoleMapping& roleMapping)
{
    if(archive.hasMessages())
    {
        const ACLMessage& initiatingMsg = archive.getInitiatingMessage();
        // If there are substatemachine(s) && substatemachine(s) proxied_to not empty && actual_protocol != inform && response not already received:
        // Genereate Transition on-the-fly, if posssible
        std::vector<EmbeddedStateMachine>::iterator it0 = mEmbeddedStateMachines.begin();
        for (; it0 != mEmbeddedStateMachines.end(); ++it0)
        {
            LOG_DEBUG("StateMachine getSubstateMachineProxiedTransition checking if a transition needs to be generated");
            // FIXME there can be other protocols that do not expect any responses
            if(!it0->proxiedTo.empty() && it0->actualProtocol != "inform" && !it0->receivedProxiedReply )
            {
                LOG_DEBUG("StateMachine getSubstateMachineProxiedTransition generating a transition");
                // Generate a transition (any performative, not leaving the state)
                Transition transition (it0->fromRole, it0->proxiedToRole, ".*", getCurrentStateId(), getCurrentStateId());
                // And see if it triggers
                if (transition.triggers(msg, initiatingMsg, roleMapping)) 
                {
                    // Save that a proxied reply was received
                    it0->receivedProxiedReply = true;
                    mSubstateMachineProxiedTransitions.push_back(transition);
                    LOG_DEBUG("StateMachine getSubstateMachineProxiedTransition transition triggered");
                    // We cannot use the local var to return as a reference
                    return mSubstateMachineProxiedTransitions.back();
                }
            }
        }
    }

    throw std::runtime_error("Message does not trigger any transition in this state");
}

void StateMachine::addEmbeddedStateMachine(EmbeddedStateMachine embeddedStateMachine)
{
    mEmbeddedStateMachines.push_back(embeddedStateMachine);
}

bool StateMachine::inFinalState() const
{
    const State& currentState = getCurrentState();
    if(!currentState.isFinal())
    {
        return false;
    }
    
    // Check all subprotocols
    std::vector<StateMachine>::const_iterator it;
    for(it = mSubStateMachines.begin(); it != mSubStateMachines.end(); it++)
    {
        if(!it->inFinalState() && !it->inFailureState())
        {
            LOG_DEBUG("Statemachine not finished (subconversation still running)");
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
        if(it0->numberOfSubConversations != ((int) mSubStateMachines.size()))
        {
            LOG_DEBUG("Statemachine not finished (subconversation still running)");
            return false;
        }
        
        // FIXME there can be other protocols that do not expect any responses
        if(!it0->proxiedTo.empty() && it0->actualProtocol != "inform" && !it0->receivedProxiedReply )
        {
            LOG_DEBUG("Statemachine not finished (proxied response missing)");
            return false;
        }
    }
    
    return true;
}

bool StateMachine::inFailureState() const
{
    return mCurrentStateId == State::GENERAL_FAILURE_STATE || mCurrentStateId == State::CONVERSATION_CANCEL_FAILURE;
}

bool StateMachine::cancelled() const
{
    return mCurrentStateId == State::CONVERSATION_CANCEL_SUCCESS;
}

std::string StateMachine::toString() const
{
    std::stringstream statemachine;
    statemachine << "Statemachine: (" << mProtocol << ")\n";
    std::map<StateId, State>::const_iterator it = mStates.begin();
    for(; it != mStates.end(); ++it)
    {
        statemachine << it->second.toString();
    }
    
    for(std::vector<EmbeddedStateMachine>::const_iterator it = mEmbeddedStateMachines.begin(); it != mEmbeddedStateMachines.end(); ++it)
    {
        statemachine << it->toString() << "\n";
    }
    
    return statemachine.str();
}

// EmbeddedStateMachine
std::string EmbeddedStateMachine::toString() const
{
    std::stringstream str;
    str << "embedded state machine: protocol: '" << name
        << "', from: '" << from 
        << "', startState: '" << startState 
        << "', proxied_to: '" << proxiedTo << "'\n";
    
    return str.str();
}

} // end of acl
} // end of fipa
