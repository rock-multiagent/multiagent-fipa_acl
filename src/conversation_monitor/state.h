/**
 *
 * \file state.h
 * \author Thomas Roehr, thomas.roehr@dfki.de
 * \brief describes the structure and operation of a state, as part of a state machine
 * 
 * \version 1.0
 */

#ifndef _FIPAACL_CONVMONITOR_STATE_H_
#define _FIPAACL_CONVMONITOR_STATE_H_

#include <map>
#include <vector>

#include <fipa_acl/message_generator/acl_message.h>
#include <fipa_acl/conversation_monitor/role.h>

namespace fipa {
namespace acl {

class StateMachine;
class Transition;

class MessageArchive
{
public:
    const ACLMessage getCorrespondingInitiatorMessage(const ACLMessage& msg) const
    {
        return ACLMessage();
    }

};

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

private:
    /** 
    * The identifier of the state
    */
    StateId mId;

    /**
    * \var final bool; flag to indicate whether the state is a valid final state
    */
    bool mIsFinal;
    
    /**
    * \var transitions: vector of transitions that belong to the current state
    */
    std::vector<Transition> mTransitions;

    /** 
    * \var subSM { (= "sub state machine"); implements the subprotocol concept from the fipa speciffication as a state machine
    * belonging to the state from which the subprotocol starts; a state machine cannot exit a state in a valid 
    * manner until all the sub-protocols of that state are in a valid final state}
    */
    std::vector<StateMachine> mEmbeddedStateMachines;

    /**
    * \var archive {vector of acl messages that stores all the messages that have been processed and validated by a any transition
    * of this state; needed to validate the in_reply_to parameter}
    */
    std::vector<ACLMessage> mMessageArchive;

    /**
     * The current role mapping
     */
    RoleMapping mRoleMapping;

protected:

    /**
     * Internal states
     */
    const static StateId UNDEFINED_ID;
    const static StateId NOT_UNDERSTOOD;


    /**
      \brief setter method for the final field of the class
      When changing the final state flag make sure you either remove existing transitions or create
      the default transition if needed
      \param final Change status of state  
    */
    void setFinal(bool final) { mIsFinal = final; }

public:
    /**
    * \brief empty constructor initializes default fields
    */
    State();

    /**
    *  \brief constructor taking the unique name(id) of the state as arguement
    *  \param _uid intended id for the state
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
    StateId transitionTarget(const ACLMessage &msg, const MessageArchive& archive);

    /**
    *  \brief method that generates implicit generic transitions applicable to all states, that may or may not be speciffied in the 
    *  configuration file such as not-understood transition (if it is specified it is not doubled);
    *  this method is to be called after all states specified in the configuration have been added to the state machine,
    *  preferably after all generic transition have been added as well
    */
    void generateDefaultTransitions();

    /**
      \brief method that returns whether the state is a final state or not
      \return true if state is final, false otherwise
    */
    bool isFinal() const { return mIsFinal; }
    
    
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
    std::vector<Transition> getTransitions() const { return mTransitions; }

    /**
     * Retrieve embedded statemachine 
     * \return list of embedded statemachines
     */
    std::vector<StateMachine> getEmbeddedStatemachines() const;
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

} // end of acl
} // end of fipa

#endif
