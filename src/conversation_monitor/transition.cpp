
#include "transition.h"
#include "state.h"

#include <iostream>
#include <stdexcept>
#include <boost/regex.hpp>
#include <base/logging.h>

namespace fipa {
namespace acl {
    
Transition::Transition() 
    : mSenderRole()
    , mReceiverRole()
    , mPerformative()
    , mSourceStateId()
    , mTargetStateId()
{
}

Transition::Transition(const Role& senderRole, const Role& receiverRole, const ACLMessage::Performative& performative, const StateId& sourceState, const StateId& targetState)
    : mSenderRole(senderRole)
    , mReceiverRole(receiverRole)
    , mPerformative(performative)
    , mSourceStateId(sourceState)
    , mTargetStateId(targetState)
{
}

bool Transition::triggers(const ACLMessage &msg, const ACLMessage& initiatingMsg, const RoleMapping& roleMapping)
{
    if( validateMessage(msg, initiatingMsg, roleMapping, validation::ALL) )
    {
	return true;
    } else {
        LOG_DEBUG("Message validation failed: message does not apply to transition");
    }
    return false;
}

bool Transition::validateMessage(const ACLMessage &msg, const ACLMessage& validatorMsg, const RoleMapping& roleMapping, validation::Flags flags)
{
    if( flags == validation::NONE)
    {
        return true;
    }

    if (validation::PERFORMATIVE & flags)
    {
        if( msg.getPerformative() != validatorMsg.getPerformative())
        {
            LOG_DEBUG("Performative validation failed"); 
            return false;
        }
    }

    if( validation::SENDER & flags)
    {
        AgentID senderAgent = msg.getSender(); 
        if(!roleMapping.isExpected(mSenderRole, senderAgent))
        {
                LOG_DEBUG("Sender validation failed"); 
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
            LOG_DEBUG("ConversationID validation failed"); 
            return false; 
        }
    }

    if(validation::PROTOCOL & flags)
    {
        if (msg.getProtocol() != validatorMsg.getProtocol())
        { 
            LOG_DEBUG("Protocol validation failed");
            return false;
        }
    }

    if(validation::ENCODING & flags)
    {
        if (msg.getEncoding() != validatorMsg.getEncoding())
        { 
            LOG_DEBUG("Encoding validation failed"); 
            return false; 
        }
    }

    if(validation::LANGUAGE & flags)
    {
        if (msg.getLanguage() != validatorMsg.getLanguage())
        {
            LOG_DEBUG("Language validation failed");
            return false;
        }
    }

    if(validation::ONTOLOGY & flags)
    {
        if (msg.getOntology() != validatorMsg.getOntology())
        {
            LOG_DEBUG("Ontology validation failed");
            return false;
        }
    }

    if(validation::IN_REPLY_TO & flags)
    {
        if (msg.getInReplyTo() != validatorMsg.getInReplyTo())
        {
            LOG_DEBUG("InReplyTo validation failed");
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
    transition << "performative: " << PerformativeTxt[mPerformative] << ", ";
    transition << "source state: " << mSourceStateId << ", ";
    transition << "target state: " << mTargetStateId;

    return transition.str();
}

bool Transition::validateReceivers(const ACLMessage& msg, const RoleMapping& roleMapping)
{
    AgentIDList actualReceivers = msg.getAllReceivers();
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
    } else if (mPerformative != other.mPerformative)
    {
        return false;
    }

    return true;
}

Transition Transition::not_understood(const Role& senderRole, const Role& receiverRole, const StateId& sourceState)
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

}

} // end of acl
} // end of fipa
