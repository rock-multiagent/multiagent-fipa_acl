#ifndef _RIMRES_FIPAACL_CONVMONITOR_STATE_H_
#define _RIMRES_FIPAACL_CONVMONITOR_STATE_H_

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
        ~State();
        ACLMessage* searchArchiveBySenderReceiver(AgentAID&,AgentAID&);
        int consumeMessage(ACLMessage msg);
        void generateDefaultTransitions();
        bool checkAllAgentsAccountedFor();
        void loadInvolvedAgents();
        
    
};    

extern operator==(const State&, const State&);
extern operator==(const State&, const std::string&);
//extern operator==(const std::string&, const State&);

} // end of acl
} // end of fipa

#endif