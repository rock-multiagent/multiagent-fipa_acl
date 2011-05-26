#ifndef _RIMRES_FIPAACL_CONVMONITOR_TYPES_
#define _RIMRES_FIPAACL_CONVMONITOR_TYPES_

#include <fipa_acl/bitefficient_message.h>

namespace fipa {
namespace acl {
    
    typedef std::string Role;
    
    typedef struct {
        Role role;
        AgentID agent;
        bool check;
    } AgentMapping;
    
    typedef struct {
        Role master;
        Role resident;
        bool check;
    } RoleCorrelation;
    

} // end of acl
} // end of fipa

#endif
