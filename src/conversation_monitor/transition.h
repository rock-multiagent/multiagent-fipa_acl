/**
 * \file transition.h
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief describes the structure and operation of a transition of the state machine
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
        enum Flags { NONE = 0x00, ALL = 0xFFFF, PERFORMATIVE=0x01, SENDER = 0x02, RECEIVERS = 0x04, CONVERSATION_ID = 0x08,
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
        
        /** performative of a message for this transition, regular expression string */
        std::string mPerformativeRegExp;

        // Source state where this transition starts from
        StateId mSourceStateId;
        
        // state that this transition ends in
        StateId mTargetStateId;

    public:

        /**
	  \brief empty constructor for the transition class; initializes fields
        */
        Transition();
        
        /**
         * Transition constructor using an explicit Performative instead of a regular expression
         */
        Transition(const Role& senderRole, const Role& receiverRole, const fipa::acl::ACLMessage::Performative& performative, const fipa::acl::StateId& sourceState, const fipa::acl::StateId& targetState);

        /**
         * Transition constructor
         */
        Transition(const Role& senderRole, const Role& receiverRole, const std::string& performativeRegExp, const StateId& sourceState, const StateId& targetState);
        
        /**
	  \brief check whether a certain message trigger the execution 
          of the transition
	  \param msg message to process
          \param initiatingMsg message that initiated the conversation or represents the direct predecessor of the current one
          \param roleMapping Current role mapping
	  \return true if message triggers this transition, false otherwise
        */
        bool triggers(const ACLMessage &msg, const ACLMessage& initiatingMsg, const RoleMapping& roleMapping) const;
        
        /**
	  \brief method that validates all parameters of a message
	  \param msg message passed as parameter to be checked
          \param validatorMsg To validate conversation id, protocol, encoding, language, ontology and in_reply_to the validatorMsg is referred to
          \param roleMapping Current role mapping
          \param flags Flag which shall be considered during validation
	  \return true if message is valid false otherwise
        */
        bool validateMessage(const ACLMessage &msg, const ACLMessage& validatorMsg, const RoleMapping& roleMapping, validation::Flags flags = validation::ALL) const;

        /** 
         * \brief setter methods for various fields of the class 
         *
         **/
        void setPerformativeRegExp(const std::string& performativeRegExp) {  mPerformativeRegExp = performativeRegExp; }
        
        /** 
         * \brief setter methods for various fields of the class 
         *
         **/
        void setPerformative(const fipa::acl::ACLMessage::Performative& performative) {  mPerformativeRegExp = fipa::acl::PerformativeTxt[performative]; }

        /**
         * Set the source state of this transition
         */
        void setSourceState(const StateId& stateId) { mSourceStateId = stateId; }

        /**
         * Set the target state of this transition
         */
        void setTargetState(const StateId& stateId) { mTargetStateId = stateId; }

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
        std::string getPerformativeRegExp() const { return mPerformativeRegExp; }

        /**
         * Get the state id of the source state
         */
        StateId getSourceStateId() const { return mSourceStateId; }

        /**
         * Get the state id of the target state
         */
        StateId getTargetStateId() const { return mTargetStateId; }

        /**
         * Convert transition to string 
         */
        std::string toString() const;

        /**
         * Equals operator
         */
        bool operator==(const Transition& t) const;

    private:
        
        /** \brief checks whether the receiver parameter of the message is valid(checks from expectedRecipients vector) */
        bool validateReceivers(const ACLMessage& msg, const RoleMapping& roleMapping) const;
};

/**
 * Default transition 
 */
namespace default_transition 
{
    /**
     * Default transition for a not understood reply
     * into the not understood state
     */
    class NotUnderstood : public Transition
    {
    public:
        NotUnderstood(const Role& senderRole, const Role& receiverRole, const StateId& stateId);
    };

    /**
     * Default transition for starting to cancel a conversation
     */
    class ConversationCancelling : public Transition
    {
    public:
        ConversationCancelling(const Role& senderRole, const Role& receiverRole, const StateId& stateId);

    };

    /**
     * Default transition when a conversation has successfully been canceled
     */
    class ConversationCancelSuccess : public Transition
    {
    public:
        ConversationCancelSuccess();

    };

    /**
     * Transition into (terminal) failed state, when conversation cancelling has failed
     */
    class ConversationCancelFailure : public Transition
    {
    public:
        ConversationCancelFailure();

    };

    /**
     * Transition into (terminal) general failure state -- when a third party (which might
     * not be part of the actual conversation) notifies the system about an error
     * TODO: Safety, i.e. handling rejection of 'false' injected errors, e.g. by setting
     * a list of trusted agents (via pattern or similar), for now all (.*) agents
     */
    class GeneralFailure : public Transition
    {
    public:
        GeneralFailure(const StateId& sourceState);
    };
}

    
} // end of acl
} // end of fipa

#endif
