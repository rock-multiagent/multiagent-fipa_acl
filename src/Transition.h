#ifndef _RIMRES_FIPAACL_CONVMONITOR_TRANSITION_H_
#define _RIMRES_FIPAACL_CONVMONITOR_TRANSITION_H_

namespace fipa {
namespace acl {
    
class Transition 
{
    private:
        std::string from;
        std::string to;
        Performative expectedPerf;
        std::string nextStateName;
        //bool messageParity;
        
        StateMachine *machine;
        State *nextState,*owningState, *precedingState;
        std::vector<AgentAID> expectedSenders;
        std::vector<AgentAID> expectedRecepients;
        
        
        
    public:
        Transition();
        int consumeMessage(ACLMessage &msg);
        int validateMessage(ACLMessage &msg);
        
    private:
        void initializeFields();
        int loadTransitionParameters();
        
        void performWithStateExit(ACLMessage &msg);
        void performOnStateExit(ACLMessage &msg);
        
        bool checkAllExpectedSendersAccountedFor   (ACLMessage &msg);
        bool checkAllExpectedRecepientsAccountedFor(ACLMessage &msg);
        bool validateConvID   	(ACLMessage &msg);
        bool validateInReplyTo	(ACLMesasge &msg);
        bool validateLanguage 	(ACLMessage &msg);
        bool validateOntology 	(ACLMessage &msg);
        bool validateProtocol 	(ACLMessage &msg);
        bool validateReplyBy  	(ACLMessage &msg);
        bool validateEncoding 	(ACLMessage &msg);
        bool validatePerformative	(ACLMessage &msg);
        bool validateSender 		(ACLMessage &msg);
        bool validateRecepients 	(ACLMessage &msg);
        
}
    
} // end of acl
} // end of fipa

#endif