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
        State();
        State(std::string _uid);
        ~State();
        ACLMessage* searchArchiveBySenderReceiver(AgentID,AgentID);
        void addToArchive(ACLMessage &msg);
        
        int consumeMessage(ACLMessage &msg);
        void generateDefaultTransitions();
        bool checkAllAgentsAccountedFor();
        void loadInvolvedAgents();
        void tickInvolvedAgent(AgentID);
        void tickInvolvedAgent(std::vector<AgentID>);
        void loadParameters();
        void updateAllAgentRoles();
        void resetInvolvedAgentsTicks();
        void setAllPrecedingStates(State*);
        
        void addTransition(Transition &t);
        bool getFinal();
        void setFinal(bool _final);
        void setUID(std::string);
        std::string getUID() const;
        StateMachine* getOwningMachine();
        void setOwningMachine(StateMachine*);
    private:
        void updateInvolvedAgentsMap(Transition &it);
    
};    

//extern operator==(const State&, const State&);
extern bool operator==(const State&, const std::string&);
//extern bool operator==(const std::string&, const State&);
extern bool operator<(const AgentID&,const AgentID&);

} // end of acl
} // end of fipa

#endif
