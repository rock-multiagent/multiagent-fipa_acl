#ifndef _RIMRES_FIPAACL_CONVMONITOR_STATE_H_
#define _RIMRES_FIPAACL_CONVMONITOR_STATE_H_

#include "StateMachine.h"
#include "Transition.h"

#include <map>

namespace fipa {
namespace acl {
    
class State 
{
    private:
        std::string uid;
        bool final;
                
        std::vector<Transition> transitions;
        std::vector<StateMachine> subSM;
        std::vector<ACLMessage> archive;
        std::map<AgentAID,bool> involvedAgents;
        
        
        friend class Transition;
        
    public:
        State();
        State(std::string _uid);
        ~State();
        ACLMessage* searchArchiveBySenderReceiver(AgentAID&,AgentAID&);
        void loadParameters();
        int consumeMessage(ACLMessage msg);
        void generateDefaultTransitions();
        bool checkAllAgentsAccountedFor();
        void loadInvolvedAgents();
        void tickInvolvedAgent(AgentAID&);
        void tickInvolvedAgent(std::vector<AgentAID>&);
        void loadParameters();
        void updateAllAgentRoles();
        void resetInvolvedAgentsTicks();
        void setAllPrecedingStates(State*);
        
        void addTransition(Transition &t);
        bool getFinal();
        void setFinal(bool _final);
        void setUID(std::string);
        std::string getUID();
    private:
        void updateInvolvedAgentsMap();
        
        
    
};    

extern operator==(const State&, const State&);
extern operator==(const State&, const std::string&);
//extern operator==(const std::string&, const State&);

} // end of acl
} // end of fipa

#endif