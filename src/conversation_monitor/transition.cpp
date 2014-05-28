
#include "transition.h"
#include "state.h"
#include "statemachine.h"

#include <iostream>
#include <stdexcept>
#include <boost/regex.hpp>
#include <base/logging.h>

namespace fipa {
namespace acl {
    
Transition::Transition() 
    : mSenderRole()
    , mReceiverRole()
    , mPerformativeRegExp()
    , mSourceStateId()
    , mTargetStateId()
{
}

Transition::Transition(const Role& senderRole, const Role& receiverRole, const fipa::acl::ACLMessage::Performative& performative, const fipa::acl::StateId& sourceState, const fipa::acl::StateId& targetState)
    : mSenderRole(senderRole)
    , mReceiverRole(receiverRole)
    , mPerformativeRegExp(PerformativeTxt[performative])
    , mSourceStateId(sourceState)
    , mTargetStateId(targetState)
{
}


Transition::Transition(const Role& senderRole, const Role& receiverRole, const std::string& performativeRegExp, const StateId& sourceState, const StateId& targetState)
    : mSenderRole(senderRole)
    , mReceiverRole(receiverRole)
    , mPerformativeRegExp(performativeRegExp)
    , mSourceStateId(sourceState)
    , mTargetStateId(targetState)
{
}

bool Transition::triggers(const ACLMessage& msg, const ACLMessage& initiatingMsg, const RoleMapping& roleMapping) const
{
    // Set the standard validation flags
    validation::Flags flags = (validation::Flags) (validation::PERFORMATIVE | validation::SENDER | validation::RECEIVERS | validation::CONVERSATION_ID | validation::PROTOCOL);
    if( validateMessage(msg, initiatingMsg, roleMapping, flags) )
    {
	return true;
    } else {
        LOG_DEBUG("Message validation failed: message does not apply to transition");
    }
    return false;
}

bool Transition::validateMessage(const ACLMessage& msg, const ACLMessage& validatorMsg, const RoleMapping& roleMapping, validation::Flags flags) const
{
    if( flags == validation::NONE)
    {
        return true;
    }

    // Test performative against transition performative
    // not the validator message one
    if (validation::PERFORMATIVE & flags)
    {
        boost::regex peformativeRegex(mPerformativeRegExp);
        if(!regex_match(msg.getPerformative(), peformativeRegex))
        {
            LOG_DEBUG("Performative validation failed: was '%s' but expected: '%s'", msg.getPerformative().c_str(), mPerformativeRegExp.c_str()); 
            return false;
        }
    }

    if( validation::SENDER & flags)
    {
        AgentID senderAgent = msg.getSender(); 
        if(!roleMapping.isExpected(mSenderRole, senderAgent))
        {
                LOG_DEBUG("Sender validation failed: '%s' unexpected for role '%s'", senderAgent.getName().c_str(), mSenderRole.getId().c_str()); 
                return false;
        }
    }

    if(validation::RECEIVERS & flags)
    {
        if(!validateReceivers(msg, roleMapping))
        {
            LOG_DEBUG("Receivers validation failed"); 
            return false;
        }
    }

    if(validation::CONVERSATION_ID & flags)
    {
        if (msg.getConversationID() != validatorMsg.getConversationID())
        { 
            LOG_DEBUG("ConversationID validation failed: was '%s' but expected: '%s'", msg.getConversationID().c_str(), validatorMsg.getConversationID().c_str()); 
            return false; 
        }
    }

    if(validation::PROTOCOL & flags)
    {
        if (msg.getProtocol() != validatorMsg.getProtocol())
        { 
            LOG_DEBUG("Protocol validation failed: was '%s' but expected: '%s'", msg.getProtocol().c_str(), validatorMsg.getProtocol().c_str());
            return false;
        }
    }

    if(validation::ENCODING & flags)
    {
        if (msg.getEncoding() != validatorMsg.getEncoding())
        { 
            LOG_DEBUG("Encoding validation failed: was '%s' but expected: '%s'", msg.getEncoding().c_str(), validatorMsg.getEncoding().c_str()); 
            return false; 
        }
    }

    if(validation::LANGUAGE & flags)
    {
        if (msg.getLanguage() != validatorMsg.getLanguage())
        {
            LOG_DEBUG("Language validation failed: was '%s' but expected: '%s'", msg.getLanguage().c_str(), validatorMsg.getLanguage().c_str());
            return false;
        }
    }

    if(validation::ONTOLOGY & flags)
    {
        if (msg.getOntology() != validatorMsg.getOntology())
        {
            LOG_DEBUG("Ontology validation failed: was '%s' but expected: '%s'", msg.getOntology().c_str(), validatorMsg.getOntology().c_str());
            return false;
        }
    }

    if(validation::IN_REPLY_TO & flags)
    {
        if (msg.getInReplyTo() != validatorMsg.getInReplyTo())
        {
            LOG_DEBUG("InReplyTo validation failed: was '%s' but expected: '%s'", msg.getInReplyTo().c_str(), validatorMsg.getInReplyTo().c_str());
            return false;
        }
    }

    return true;
}

std::string Transition::toString() const
{
    std::stringstream transition;
    transition << "transition: sender role: " << mSenderRole.getId() << ", "; 
    transition << "receiver role: " << mReceiverRole.getId() << ", ";
    transition << "performative: " << mPerformativeRegExp << ", ";
    transition << "source state: " << mSourceStateId << ", ";
    transition << "target state: " << mTargetStateId;

    return transition.str();
}

bool Transition::validateReceivers(const ACLMessage& msg, const RoleMapping& roleMapping) const
{
    AgentIDList actualReceivers = msg.getAllReceivers();
    if(actualReceivers.empty())
    {
        std::string errorMsg = "No receivers set for this message: conversation id: " + msg.getConversationID() + " sender: " + msg.getSender().getName();
        LOG_ERROR("%s",errorMsg.c_str());
        throw std::runtime_error(errorMsg);
    }

    // Test whether the actual receivers are expected -- according to the current role to agent mapping
    AgentIDList::const_iterator ait = actualReceivers.begin(); 

    for(; ait != actualReceivers.end(); ++ait)
    {
        if(!roleMapping.isExpected(mReceiverRole, *ait))
        {
            return false;
        }
    }

    return true;
}

bool Transition::operator==(const Transition& other) const
{
    if(mSourceStateId != other.mSourceStateId)
    {
        return false;
    } else if (mTargetStateId != other.mTargetStateId)
    {
        return false;
    } else if (mSenderRole != other.mSenderRole)
    {
        return false;
    } else if (mReceiverRole != other.mReceiverRole)
    {
        return false;
    } else if (mPerformativeRegExp != other.mPerformativeRegExp)
    {
        return false;
    }

    return true;
}

// FIPA include definitions of some default protocol elements
// This library considers not-understood and cancel interaction-protocol as
// default ones and embeds them into the standard construction of the state machine
namespace default_transition
{

NotUnderstood::NotUnderstood(const Role& senderRole, const Role& receiverRole, const StateId& sourceState)
    : Transition(senderRole, receiverRole, ACLMessage::NOT_UNDERSTOOD, sourceState, State::NOT_UNDERSTOOD)
{
}

ConversationCancelling::ConversationCancelling(const Role& senderRole, const Role& receiverRole, const StateId& sourceState)
    : Transition(senderRole, receiverRole, ACLMessage::CANCEL, sourceState, State::CONVERSATION_CANCELLING)
{
}

// We loosen the role restriction here, to facilitate the integrating of the default states -- now we just need
// one state per direction (which works multidirectional)
// TODO: consider solving this issue using an embedded state machine
ConversationCancelSuccess::ConversationCancelSuccess()
    : Transition( Role(".*"), Role(".*"), ACLMessage::INFORM, State::CONVERSATION_CANCELLING, State::CONVERSATION_CANCEL_SUCCESS)
{
}

ConversationCancelFailure::ConversationCancelFailure()
    : Transition(Role(".*"), Role(".*"), ACLMessage::FAILURE, State::CONVERSATION_CANCELLING, State::CONVERSATION_CANCEL_FAILURE)
{
}

GeneralFailure::GeneralFailure(const StateId& sourceState)
    : Transition(Role(".*"), SelfRole(), ACLMessage::FAILURE, sourceState, State::GENERAL_FAILURE_STATE)
{}

}

} // end of acl
} // end of fipa
