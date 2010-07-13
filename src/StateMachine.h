#ifndef _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINE_H_
#define _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINE_H_

#include "types.h"
//#include "State.h"
#include <message-generator/types.h>
#include <message-generator/ACLMessage.h>

#include <vector>
//#include <multimap>

namespace fipa {
namespace acl {

   class State;
    
class StateMachine 
{
    private:
        std::vector<State> states;
        std::vector<AgentMapping> involvedAgents;
        AgentAID owner;
        State *currentState;
        bool active;
        bool conversationOver;
        
        std::vector<StateMachine> cancelMetaP;
        
        Ontology ontology;
        Language language;
        Protocol protocol;
        ConversationID convid;
        Encoding encoding;
        //ReplyBy replyBy;
        //ReplyWith replyWith;
        
        friend class Transition;
        
    public:
        static const std::string NOT_UNDERSTOOD;
        static const std::string CONV_CANCELLED;
        static const std::string INITIAL;
        static const std::string INITIATOR;
        
        StateMachine();
        StateMachine(AgentAID _owner);
        ~StateMachine();
        bool setInitialState(State*);
        bool setInitialState(std::string);
        bool setOwner(AgentAID);
        int startMachine(ACLMessage msg);
        int consumeMessage(ACLMessage msg);
        //int initializeMachineFields(ACLMessage msg);
        
        
        
        State* getStateByName(std::string);
        void generateDefaultStates();
        void generateDefaultTransitions();
        bool addState(State);
        bool isConversationOver();
        bool isActive();
        void addRole(Role myrole);
        bool setRole(Role myrole,AgentAID myagent);
        bool setRole(Role myrole,std::vector<AgentAID> agents);
        bool checkIfAgentAssigned(AgentAID);
        bool checkIfRoleExists(Role myrole);
        Role getAgentRole(AgentAID ag);
        bool checkIfRoleSet(Role&);
        //void removeInterlocutor(AgentAID);
        //void removeInterlocutor(std::vector<AgentAID>);
        void updateAllAgentRoles();
        
    private:
        void initializeObjectFields();
        void removeInterlocutor(AgentAID ag);
        void removeInterlocutor(std::vector<AgentAID> agents);
        void loadAllTransitionParameters(ACLMessage &msg);
        void UpdateAllAgentRoles();
        int createAndStartNewCancelMetaProtocol(ACLMessage &msg);
        StateMachine generateCancelMetaProtocol(Role);
        
        
    
};


} // end of acl
} // end of fipa

#endif