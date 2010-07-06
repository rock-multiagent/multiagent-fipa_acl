#ifndef _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINE_H_
#define _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINE_H_

#include "types.h"
#include "State.h"
#include <message-generator/types.h>
#include <message-generator/ACLMessage.h>

#include <vector>
#include <multimap>

namespace fipa {
namespace acl {

    typedef
    
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
        int startMachine(ACLMessage msg);
        int consumeMessage(ACLMessage msg);
        int initializeMachineFields(ACLMessage msg);
        int createAndStartNewCancelMetaP(ACLMessage &msg);
        int generateCancelMetaProtocol();
        
        Role getAgentRole(AgentAID &agent);
        State* getStateByName(std::string);
        
        bool checkConversationOver();
        void addRole(Role role);
        bool setRole(Role myrole,AgentAID myagent);
        bool setRole(Role myrole,std::vector<AgentAID> agents);
        bool checkIfAgentAssigned(AgentAID&);
        bool checkIfRoleAssigned(Role&);
        void removeInterlocutor(AgentAID&);
        void removeInterlocutor(std::vector<AgentAID>&);
    private:
        void initializeFields();
        void removeInterlocutor(AgentAID &agent);
        void removeInterlocutor(std::vector<AgentAID> &agents);
        void loadAllTransitionParameters(ACLMessage &msg);
        void UpdateAllAgentRoles();
        
        
    
};


} // end of acl
} // end of fipa

#endif