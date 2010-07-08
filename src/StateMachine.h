#ifndef _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINE_H_
#define _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINE_H_

#include "types.h"
#include "State.h"
#include <message-generator/types.h>
#include <message-generator/ACLMessage.h>

#include <vector>
//#include <multimap>

namespace fipa {
namespace acl {

   
    
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
        
        StateMachine();
        ~StateMachine();
        bool setInitialState(State*);
        bool setInitialState(std::string);
        bool setOwner(AgentAID);
        int startMachine(ACLMessage msg);
        int consumeMessage(ACLMessage msg);
        //int initializeMachineFields(ACLMessage msg);
        
        
        
        State* getStateByName(std::string);
        
        bool isConversationOver();
        void addRole(Role role);
        bool setRole(Role myrole,AgentAID myagent);
        bool setRole(Role myrole,std::vector<AgentAID> agents);
        bool checkIfAgentAssigned(AgentAID&);
        bool checkIfRoleExists(Role &myrole);
        Role getAgentRole(AgentAID &agent);
        bool checkIfRoleSet(Role&);
        void removeInterlocutor(AgentAID);
        void removeInterlocutor(std::vector<AgentAID>);
        void updateAllAgentRoles();
        
    private:
        void initializeFields();
        void removeInterlocutor(AgentAID &agent);
        void removeInterlocutor(std::vector<AgentAID> &agents);
        void loadAllTransitionParameters(ACLMessage &msg);
        void UpdateAllAgentRoles();
        int createAndStartNewCancelMetaP(ACLMessage &msg);
        StateMachine generateCancelMetaProtocol(Role);
        
        
    
};


} // end of acl
} // end of fipa

#endif