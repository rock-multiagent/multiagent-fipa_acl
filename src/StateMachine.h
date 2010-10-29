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
        AgentID owner;
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
        friend class StateMachineTest;
        
    public:
        static const std::string NOT_UNDERSTOOD;
        static const std::string CONV_CANCELLED;
        static const std::string INITIAL;
        static const std::string INITIATOR;
        
        StateMachine();
        StateMachine(AgentID _owner);
        ~StateMachine();
        bool setInitialState(State*);
        bool setInitialState(std::string);
        bool setOwner(AgentID);
        int startMachine(ACLMessage msg);
        int consumeMessage(ACLMessage msg);
        //int initializeMachineFields(ACLMessage msg);
        
        
        
        State* getStateByName(std::string);
        State* getStateByName(State);
        void generateDefaultStates();
        void generateDefaultTransitions();
        bool addState(State);
        bool isConversationOver();
        bool isActive();
        void addRole(Role myrole);
        bool setRole(Role myrole,AgentID myagent);
        bool setRole(Role myrole,std::vector<AgentID> agents);
        bool checkIfAgentAssigned(AgentID);
        bool checkIfRoleExists(Role myrole);
        Role getAgentRole(AgentID ag);
        bool checkIfRoleSet(Role&);
        //void removeInterlocutor(AgentID);
        //void removeInterlocutor(std::vector<AgentID>);
        void updateAllAgentRoles();
        
        
    private:
        void initializeObjectFields();
        void removeInterlocutor(AgentID ag);
        void removeInterlocutor(std::vector<AgentID> agents);
        void loadAllTransitionParameters(ACLMessage &msg);
        void UpdateAllAgentRoles();
        int createAndStartNewCancelMetaProtocol(ACLMessage &msg);
        StateMachine generateCancelMetaProtocol(Role);
        void loadParameters();
        
        
    
};


} // end of acl
} // end of fipa

#endif