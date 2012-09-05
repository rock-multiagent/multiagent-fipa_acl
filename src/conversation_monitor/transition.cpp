
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
    , mSourceState()
    , mTargetState()
{
}

Transition::Transition(const Role& senderRole, const Role& receiverRole, const ACLMessage::Performative& performative, const StateId& sourceState, const StateId& targetState)
    : mSenderRole(senderRole)
    , mReceiverRole(receiverRole)
    , mPerformative(performative)
    , mSourceState(sourceState)
    , mTargetState(targetState)
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
            return false;
        }
    }

    if( validation::SENDER & flags)
    {
        AgentID senderAgent = msg.getSender(); 
        if(!roleMapping.isExpected(mSenderRole, senderAgent))
        {
                return false;
        }
    }

    if(validation::RECEIVERS & flags)
    {
        if(!validateReceivers(msg, roleMapping))
        {
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

} // end of acl
} // end of fipa
