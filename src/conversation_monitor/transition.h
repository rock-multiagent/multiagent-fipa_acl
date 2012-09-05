/**
 * \file transition.h
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief describes the structure and operation of a transition of the state machine
 * 
 * \version 1.0
 */

#ifndef _FIPAACL_CONVMONITOR_TRANSITION_H_
#define _FIPAACL_CONVMONITOR_TRANSITION_H_

#include <fipa_acl/bitefficient_message.h>
#include <algorithm>
#include <fipa_acl/conversation_monitor/role.h>
#include <fipa_acl/conversation_monitor/state.h>

namespace fipa {
namespace acl {

namespace validation {
        enum Flags : uint32_t { NONE = 0x00, ALL = 0xFFFF, PERFORMATIVE=0x01, SENDER = 0x02, RECEIVERS = 0x04, CONVERSATION_ID = 0x08,
            PROTOCOL = 0x10, ENCODING = 0x20, LANGUAGE=0x40, ONTOLOGY = 0x80, IN_REPLY_TO=0x100 };
}

/**
 * A transition that can be associated with a state object
 */
class Transition 
{
    private:
        /** the role of the agent expected to be the sender of a message for this transition */
        Role mSenderRole;
        
        /** role of the agent expected to be the receiver of a message for this transition */
        Role mReceiverRole;
        
        /** performative of a message for this transition */
        ACLMessage::Performative mPerformative;

        // Source state where this transition starts from
        StateId mSourceState;
        
        // state that this transition ends in
        StateId mTargetState;

    public:

        /**
	  \brief empty constructor for the transition class; initializes fields
        */
        Transition();

        /**
         * Transition constructor
         */
        Transition(const Role& senderRole, const Role& receiverRole, const ACLMessage::Performative& performative, const StateId& sourceState, const StateId& targetState);
        
        /**
	  \brief check whether a certain message trigger the execution 
          of the transition
	  \param msg message to process
          \param initiatingMsg message that initiated the conversation or represents the direct predecessor of the current one
	  \return true if message triggers this transition, false otherwise
        */
        bool triggers(const ACLMessage &msg, const ACLMessage& initiatingMsg, const RoleMapping& roleMapping);
        
        /**
	  \brief method that validates all parameters of a message
	  \param msg message passed as parameter to be checked
	  \return true if message is valid false otherwise
        */
        bool validateMessage(const ACLMessage &msg, const ACLMessage& validatorMsg, const RoleMapping& roleMapping, validation::Flags flags = validation::ALL);

        /** 
         * \brief setter methods for various fields of the class 
         *
         **/
        void setPerformative(const ACLMessage::Performative& performative) { mPerformative = performative; }

        /**
         * Set the source state of this transition
         */
        void setSourceState(const StateId& stateId) { mSourceState = stateId; }

        /**
         * Set the target state of this transition
         */
        void setTargetState(const StateId& stateId) { mTargetState = stateId; }

        /**
         * Set the sender role
         */
        void setSenderRole(const Role& role) { mSenderRole = role; }

        /**
         * Set the receiver role
         */
        void setReceiverRole(const Role& role)  { mReceiverRole = role; }

        /**
         * Get the sender role
         */
        Role getSenderRole() const { return mSenderRole; }

        /**
         * Get the receiver role
         */
        Role getReceiverRole() const { return mReceiverRole; }

        /**
         * \brief getter methods for various fields of the class
         *
         **/
        ACLMessage::Performative getPerformative() const { return mPerformative; }

        /**
         * Get the name of the source state
         */
        StateId getSourceState() const { return mSourceState; }

        /**
         * Get the name of the target state
         */
        StateId getTargetState() const { return mTargetState; }

    private:
        
        /** \brief checks whether the receiver parameter of the message is valid(checks from expectedRecipients vector) */
        bool validateReceivers(const ACLMessage& msg, const RoleMapping& roleMapping);
};
    
} // end of acl
} // end of fipa

#endif
