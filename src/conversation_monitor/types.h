#ifndef _FIPAACL_CONVMONITOR_TYPES_
#define _FIPAACL_CONVMONITOR_TYPES_

#include <fipa_acl/bitefficient_message.h>

namespace fipa {
namespace acl {
    
    typedef std::string Role;
   
    // Role is the generic name of the actor
    // such as 'initiator' (whereas 'initiator')
    // is a keyword 
    // AgentMapping is assigned once the conversation
    // has started an the agents role has been identified
    // 
    // check allows to detect whether a role has already
    // been assigned (defaults to false)
    typedef struct _AgentMapping {
        Role role;
        AgentID agent;
        bool check;

        _AgentMapping() : role(), agent(), check(false)
        {
        }

    } AgentMapping;
    
    // Specific for subprotocol in order to match
    // roles across different statemachines, i.e.
    // from parent statemachine (master) to the 
    // enclosed one (resident) 
    typedef struct _RoleCorrelation {
        Role master;
        Role resident;
        // whether or not it has already been assigned
        // in the current statemachine (resident)
        bool check;

        _RoleCorrelation() : master(), resident(), check(false)
        {
        }
    } RoleCorrelation;
    

} // end of acl
} // end of fipa

#endif
