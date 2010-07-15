#ifndef _RIMRES_FIPAACL_CONVMONITOR_STATE_H_
#define _RIMRES_FIPAACL_CONVMONITOR_STATE_H_

#include <message-generator/ACLMessage.h>
//#include "StateMachine.h"
#include "Transition.h"


#include <map>

namespace fipa {
namespace acl {
    
class StateMachine;
    
class State 
{
    private:
        std::string uid;
        bool final;
                
        std::vector<Transition> transitions;
        std::vector<StateMachine> subSM;
        std::vector<ACLMessage> archive;
        std::map<AgentAID,bool> involvedAgents;
        
        StateMachine* owningMachine;
        
        
        friend class Transition;
        friend class StateMachineTest;
        
    public:
        State();
        State(std::string _uid);
        ~State();
        ACLMessage* searchArchiveBySenderReceiver(AgentAID,AgentAID);
        void addToArchive(ACLMessage &msg);
        
        int consumeMessage(ACLMessage &msg);
        void generateDefaultTransitions();
        bool checkAllAgentsAccountedFor();
        void loadInvolvedAgents();
        void tickInvolvedAgent(AgentAID);
        void tickInvolvedAgent(std::vector<AgentAID>);
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
extern bool operator<(const AgentAID&,const AgentAID&);

} // end of acl
} // end of fipa

#endif
