#ifndef _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINEBUILDER_H_
#define _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINEBUILDER_H_

#include "StateMachine.h"
#include "Transition.h"
#include "State.h"

#include <vector>

namespace fipa {
namespace acl {
    
class StateMachineBuilder {
        
    private:
        std::vector<std::string> roles;
        std::vector<std::string> states;
        
    public:
        // attribute/node names used in the spec standard; most general ones such as "state", "transition" are hard-implemented
        static std::string from;
        static std::string to;
        static std::string target;
        static std::string id;
        static std::string final;
        static std::string performative;
    
    public:
        StateMachineBuilder();
        StateMachine loadSpecification(std::string);
        StateMachine parseStateMachineNode(TiXmlElement *sm);
        State parseStateNode(TiXmlElement *st);
        void addStates(TiXmlElement *st,StateMachine &sm);
        Transition parseTransitionNode(TiXmlElement *trans);
        void addInvolvedAgentsMap(StateMachine &sm);
    
    
};


} //end of acl
} //end of fipa

#endif