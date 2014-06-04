/**
 *
 * \file state.h
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief describes the structure and operation of a state, as part of a state machine
 */

#ifndef _FIPAACL_CONVMONITOR_STATE_H_
#define _FIPAACL_CONVMONITOR_STATE_H_

#include <map>
#include <vector>

#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/conversation_monitor/role.h>

namespace fipa {
namespace acl {

struct EmbeddedStateMachine;
class StateMachine;
class MessageArchive;
class Transition;
namespace default_transition {
    class NotUnderstood;
    class ConversationCancelling;
    class ConversationCancelSuccess;
    class ConversationCancelFailure;
    class GeneralFailure;
}

/**
 * Definition of a StateId
 */
typedef std::string StateId; 
    
/**
* \class State
* \brief implements a state of the StateMachine class
*/
class State 
{
    friend class Transition;
    friend class default_transition::NotUnderstood;
    friend class default_transition::ConversationCancelling;
    friend class default_transition::ConversationCancelSuccess;
    friend class default_transition::ConversationCancelFailure;
    friend class default_transition::GeneralFailure;
    friend class StateMachineReader;
    friend class StateMachine;

private:
    /** 
    * The identifier of the state
    */
    StateId mId;

    /**
    * Flag to indicate whether the state is a valid final state
    */
    bool mIsFinal;
    
    /**
    * List of outgoing transitions that belong to this state
    */
    std::vector<Transition> mTransitions;

    /** 
    * Embedded statemachines implement the subprotocol concept from the fipa speciffication as a state machine.
    * Each embedded state machine belongs to the state from which the subprotocol starts;
    * a state machine cannot exit a state in a valid
    * manner until all the sub-protocols of that state are in a valid final state
    */
    std::vector<EmbeddedStateMachine> mEmbeddedStateMachines;

    static std::vector<StateId> msDefaultStates;

protected:
    /**
      \brief setter method for the final field of the class
      When changing the final state flag make sure you either remove existing transitions or create
      the default transition if needed
      \param final Change status of state
    */
    void setFinal(bool final) { mIsFinal = final; }

public:
    /**
     * Default states
     */
    const static StateId UNDEFINED_ID;
    const static StateId NOT_UNDERSTOOD;
    const static StateId CONVERSATION_CANCELLING;
    const static StateId CONVERSATION_CANCEL_SUCCESS;
    const static StateId CONVERSATION_CANCEL_FAILURE;
    const static StateId GENERAL_FAILURE_STATE;

    /**
    * \brief empty constructor initializes default fields
    */
    State();

    /**
    *  \brief constructor taking the unique name(id) of the state as arguement
    *  \param id intended id for the state
    */
    State(const StateId& id);

    /**
     * Add a state transition
     * \return Transition with update source state field
     */
    Transition addTransition(const Transition& t);
   
    /**
    *  \brief Check whether the received message triggers a transition
    *  \return target of the transition 
    *  \throws runtime_error if the msg is invalid in the current state
    */
    const Transition& getTransition(const ACLMessage &msg, const MessageArchive& archive, const RoleMapping& roleMapping) const;

    /**
    *  \brief method that generates implicit generic transitions applicable to all states, that may or may not be speciffied in the 
    *  configuration file such as not-understood transition (if it is specified it is not doubled);
    *  this method is to be called after all states specified in the configuration have been added to the state machine,
    *  preferably after all generic transition have been added as well
    */
    void generateDefaultTransitions();

    /**
      \brief method that returns whether the state is a final state or not.
      In the case of a state with embedded state machines, this is a bit more coplex, see the implementation
      \return true if state is final, false otherwise
    */
    bool isFinal() const;

    /**
     * \brief Test if state belongs to the default state or not
     * \return true if state is default, false otherwise
     */
    bool isDefaultState() const;
    
    /**
      \brief setter method for the uid field of the state NOTE: maybe should be taken out and name only be allowed to be set on
      \brief construction, as if we rename the state later on the state machine might crash(surely will actually)
      \param id to set as id for the state
    */
    void setId(const StateId& id) { mId = id; }
    
    /**
      \brief getter method for the uid field of the class
    */
    std::string getId() const { return mId; }

    /**
     * Retrieve transitions associated with this state
     * \return list of transitions
     */
    const std::vector<Transition>& getTransitions() const { return mTransitions; }

    /**
     * Retrieve embedded statemachine.
     * \return list of embedded statemachines
     */
    const std::vector<EmbeddedStateMachine>& getEmbeddedStatemachines() const { return mEmbeddedStateMachines; }
    
    /**
     * Add an embedded state machine.
     */
    void addEmbeddedStateMachine(fipa::acl::EmbeddedStateMachine embeddedStateMachine);

    /**
     * Convert state to string representation
     */
    std::string toString() const;

};

class FinalState : public State
{
public:
    FinalState(const StateId& id);
};

class UndefinedState : public State
{
public:
    UndefinedState();
};

namespace default_state
{
    /**
     * Default final state for not understood
     */
    class NotUnderstood : public FinalState
    {
    public:
        NotUnderstood() : FinalState(State::NOT_UNDERSTOOD)
        {}
    };

    /**
     * Default state for conversation cancel
     */
    class ConversationCancelling : public State
    {
    public: 
        ConversationCancelling();
    };

    /**
     * Default final state for a successful conversation cancel
     */
    class ConversationCancelSuccess : public FinalState
    {
    public: 
        ConversationCancelSuccess() : FinalState(State::CONVERSATION_CANCEL_SUCCESS)
        {}
    };

    /**
     * Default final state for a failed conversation cancel
     */
    class ConversationCancelFailure : public FinalState
    {
    public: 
        ConversationCancelFailure() : FinalState(State::CONVERSATION_CANCEL_FAILURE)
        {}
    };

    class GeneralFailure : public FinalState
    {
    public: 
        GeneralFailure() : FinalState(State::GENERAL_FAILURE_STATE)
        {}
    };
}

} // end of acl
} // end of fipa

#endif
