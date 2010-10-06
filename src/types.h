#ifndef _RIMRES_FIPAACL_CONVMONITOR_TYPES_
#define _RIMRES_FIPAACL_CONVMONITOR_TYPES_

//#include <map>
#include <message-generator/AgentID.h>

namespace fipa {
namespace acl {
    
    typedef std::string Role;
    
    typedef struct {
        Role role;
        AgentID agent;
        bool check;
    } AgentMapping;
    

} // end of acl
} // end of fipa

#endif