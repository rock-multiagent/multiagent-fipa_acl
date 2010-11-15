/**
 *
 * \file State.h
 * \author Mircea Cretu Stancu
 * \brief describes the structure and operation of a state, as part of a state machine
 * 
 * \version 1.0
 */

#ifndef _RIMRES_FIPAACL_CONVMONITOR_STATE_H_
#define _RIMRES_FIPAACL_CONVMONITOR_STATE_H_

#include <message-generator/ACLMessage.h>
//#include "StateMachine.h"
#include "Transition.h"


#include <map>

namespace fipa {
namespace acl {
    
    /**
        \brief forward declaration of the StateMachine class to resolve circular dependency
    */
class StateMachine;
    
/**
    \class State
    \brief implements a state of the StateMachine class
*/
class State 
{
    private:
        /** \var uid: {string; represents the unique identifier of a state needed as we sometimes have to access the states by name,
		   especially in the building phase of the state machine}
        */
        std::string uid;
        /** \var final: bool; flag to indicate whether the state is a valid final state */
        bool final;
        
        /** \var transitions: vector of transitions that belong to the current state */
        std::vector<Transition> transitions;
        /** \var subSM { (= "sub state machine"); implements the subprotocol concept from the fipa speciffication as a state machine
		    belonging to the state from which the subprotocol starts; a state machine cannot exit a state in a valid 
		    manner until all the sub-protocols of that state are in a valid final state}
        */
        std::vector<StateMachine> subSM;
        /**
	  \var archive {vector of acl messages that stores all the messages that have been processed and validated by a any transition
		      of this state; needed to validate the in_reply_to parameter}
        */
        std::vector<ACLMessage> archive;
        /**
	  \var involvedAgents {variable that keeps track of all AgentID instances of all transitions of this state; acts like 
			   a transition constraint: the stateMachine goes to the next state only if all agents that ware supposed
			   to exchange messages in this state have done so}
        */
        std::map<AgentID,bool> involvedAgents;
        /**
	  \var owningMachine: {pointer to the owning state machine of this state; needed mainly for the build phase}
        */
        StateMachine* owningMachine;
        
        /** \brief transition class declared as friend as it needs to access and/or modify fields of the state(i.e: archive,etc)*/
        friend class Transition;
        /** \brief StateMachineTest class declared as friend for the testing phase only; NOT in the final version*/
        friend class StateMachineTest;
        
    public:
        /**
	  \brief empty constructor initializes default fields
        */
        State();
        /**
	  \brief constructor taking the unique name(id) of the state as arguement
	  \param _uid intended id for the state
        */
        State(std::string _uid);
        ~State();
        /**
	  \brief method that searches through the archived message for the needed message in validation check of the in_reply_to 
	  \brief field of another message.
	  \param: first agentID parameter is the sender of the currently checked message, not of the message from archive 
		returned by this method
	  \param: second agentID parameter is the receiver of the currently checked message, not of the message from archive
		returned by this method
	  \return ACLMessage*; pointer to the found message or NULL if no message meets the requirements
        */
        ACLMessage* searchArchiveBySenderReceiver(AgentID,AgentID);
        /**
	  \brief method that adds a message to the state's message archive(message that has been previously validated)
	  \param msg: the message to be added to archive
        */
        void addToArchive(ACLMessage &msg);
        /**
	  \brief prcesses the message received as parameter; sends it to each sub-protocol(subSM) until it is validated by one(if any)
	  \brief or else to each transition for processing until the message is validated by one of them(if any)
	  \param msg: current message that is being processed from the flow of messages
	  \return 0 if successful, one otherwise
        */
        int consumeMessage(ACLMessage &msg);
        /**
	  \brief method that generates implicit generic transitions aplicable to all states, that may or may not be speciffied in the 
	  \brief configuration file such as not-understood transition(if it is seciffied it is not doubled);
	  \brief this method is to be called after all states specified in the configuration have been added to the state machine,
	  \brief preferably after all generic transition have been added as well
        */
        void generateDefaultTransitions();
        /**
	  \brief checks whether all agents supposed to interact through messages at this point of the conversation(i.e: state) have
	  \brief done so; acts like a constraint for moving out of the state
        */
        bool checkAllAgentsAccountedFor();
        //void loadInvolvedAgents();
        /**
	  \brief marks an agent from the involvedAgents map as accounted for
	  \param: agent to be ticked
        */
        void tickInvolvedAgent(AgentID);
        /**
	  \brief marks a vector of agents from the involvedAgents map as accounted for
	  \param: std::vector of agent to be ticked
        */
        void tickInvolvedAgent(std::vector<AgentID>);
        /**
	  \brief method that passes the loadParameters signal downwards to transitions; it is to called after all transitions and 
	  \brief states have been generated;
        */
        void loadParameters();
        /**
	  \brief method that passes the update roles signall downwards to transitions(is to be called whenever a role is changed
	  \brief added or removed); it also updates the related fields from the state itself(involvedAgents,..) 
        */
        void updateAllAgentRoles();
        /**
	  \brief resets all ticks from involvedAgents map of the state; it is called when a transition takes drives the state machine 
	  \brief to another state(so that it behaves correctly should it later return to this state)
        */
        void resetInvolvedAgentsTicks();
        /**
	  \brief method that sets the preceding state field of all transitions of the current state with the state given as argument
	  \brief it is called by the transition class when a transition takes the state machine from one state to another; it is needed
	  \brief because from the same state diferent transitions can take the state machine to diferent states
	  \param: state to be set as preceding state for all the transitions of the current state
        */
        void setAllPrecedingStates(State*);
        /**
	  \brief method that adds a transition to the current state
	  \param t: transition to be added, passed as reference
        */
        void addTransition(Transition &t);
        /**
	  \brief method that returns whether the state is a final state or not
	  \return true if state is final, false otherwise
        */
        bool getFinal();
        /**
	  \brief getter method for the final field of the class
	  \param _final: bool argument to be set
        */
        void setFinal(bool _final);
        /**
	  \brief setter method for the uid field of the state NOTE: maybe should be taken out and name only be allowed to be set on
	  \brief construction, as if we rename the state later on the state machine might crash(surely will actually)
	  \param: string to set as uid for the state
        */
        void setUID(std::string);
        /**
	  \brief getter method for the uid field of the class
        */
        std::string getUID() const;
        /** \brief getter method for the owningMachine field of the class */
        StateMachine* getOwningMachine();
        /** \brief setter method for the owningMachine field of the class */
        void setOwningMachine(StateMachine*);
    private:
        /**
	  \brief helper method that updates the involvedAgents map of the class based on the agents involved in a transition
        */
        void updateInvolvedAgentsMap(Transition &it);
    
};    

//extern operator==(const State&, const State&);
extern bool operator==(const State&, const std::string&);
//extern bool operator==(const std::string&, const State&);
extern bool operator<(const AgentID&,const AgentID&);

} // end of acl
} // end of fipa

#endif
