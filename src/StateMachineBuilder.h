#ifndef _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINEBUILDER_H_
#define _RIMRES_FIPAACL_CONVMONITOR_STATEMACHINEBUILDER_H_

#include "StateMachine.h"
//#include "Transition.h"
#include "State.h"

#include <tinyxml/tinyxml.h>
#include <tinyxml/tinystr.h>

#include <vector>

namespace fipa {
    
namespace acl {
    
class StateMachineBuilder {
        
    private:
        std::vector<std::string> roles;
        std::vector<std::string> states;
        
    public:
        // attribute/node names used in the spec standard; most general ones such as "state", "transition" are hard-implemented
        static const std::string from;
        static const std::string to;
        static const std::string target;
        static const std::string id;
        static const std::string final;
        static const std::string performative;
        static const std::string initial;
    
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