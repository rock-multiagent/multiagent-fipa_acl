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

    /**
     * Retrieve previous message of the initator, that triggers the emission on this message
     * \return last message the initiator sent
     */
    const ACLMessage& getLastInitiatorMessage() const;

protected:

    /**
     * Internal states
     */
    const static StateId NOT_UNDERSTOOD;

    const static StateId UNDEFINED_ID;

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
     */
    void addTransition(Transition &t);
   
    /**
    *  \brief prcesses the message received as parameter; sends it to each sub-protocol(subSM) until it is validated by one(if any)
    *  or else to each transition for processing until the message is validated by one of them(if any)
    *  \param msg current message that is being processed from the flow of messages
    *  \return 0 if successful, 1 otherwise
    */
    int consumeMessage(const ACLMessage &msg);

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
      \brief setter method for the final field of the class
      \param _final: bool argument to be set
    */
    void setFinal(bool final) { mIsFinal = final; }
    
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

class UndefinedState : public State
{
public:
    UndefinedState();
};

} // end of acl
} // end of fipa

#endif
