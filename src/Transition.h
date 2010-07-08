#ifndef _RIMRES_FIPAACL_CONVMONITOR_TRANSITION_H_
#define _RIMRES_FIPAACL_CONVMONITOR_TRANSITION_H_

//#include "StateMachine.h"
#include "types.h"
#include <message-generator/ACLMessage.h>
#include <message-generator/types.h>
#include <algorithm>

namespace fipa {
namespace acl {

    class StateMachine;
    class State;
    
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
        bool validateMessage(ACLMessage &msg);
        void loadParameters();
        void updateRoles();
        void setPrecedingState(State*);
                
        void setExpectedPerformative	(std::string);
        void setNextStateName		(std::string);
        void setFrom		(std::string);
        void setTo			(std::string);
        
        std::string getExpectedPerformative();
        std::string getNextStateName();
        std::string getFrom();
        std::string getTo();

    private:
        void initializeFields();
        int loadTransitionParameters();
        bool updateRoles(ACLMessage&);
        
        void performWithoutStateExit(ACLMessage &msg);
        void performOnStateExit(ACLMessage &msg);
        
        bool checkAllExpectedSendersAccountedFor   (ACLMessage &msg);
        bool checkAllExpectedRecepientsAccountedFor(ACLMessage &msg);
        bool validateConvID   	(ACLMessage &msg);
        bool validateInReplyTo	(ACLMessage &msg);
        bool validateLanguage 	(ACLMessage &msg);
        bool validateOntology 	(ACLMessage &msg);
        bool validateProtocol 	(ACLMessage &msg);
        bool validateReplyBy  	(ACLMessage &msg);
        bool validateEncoding 	(ACLMessage &msg);
        bool validatePerformative	(ACLMessage &msg);
        bool validateSender 		(ACLMessage &msg);
        bool validateRecepients 	(ACLMessage &msg);
        
        //void removeAllRecepientsBut(AgentAID&);
        void removeAllAgentsBut(AgentAID&,std::vector<AgentAID>&);
        
};
    
} // end of acl
} // end of fipa

#endif