#ifndef FIPA_ACL_CONVERSATIONMONITOR_STATEMACHINE_FACTORY_H
#define FIPA_ACL_CONVERSATIONMONITOR_STATEMACHINE_FACTORY_H

#include <string>
#include <map>
#include <fipa_acl/conversation_monitor/statemachine.h>
#include <fipa_acl/conversation_monitor/state.h>

namespace fipa {
namespace acl {

class StateMachineReader;

class StateMachineFactory
{
    private:
        // Reader for state machine files
        static StateMachineReader msStateMachineReader;

        static std::string msResourceDir;

        // Marked when the function prepareProtocolFromResourceDir has already been called
        // Used for lazy initialization in getStateMachine
        static bool msPreparedResourceDir;
        
        static std::map<std::string, StateMachine> msStateMachines;

        /**
        * Instanciates all available machines from the resource directory
        */
        static void prepareProtocolsFromResourceDir();

public: 
        /**
        * Set the resource dir where to search for the protocol definitions
        */
        static void setProtocolResourceDir(const std::string& resourceDir);

        /**
         * Create a statemachine for a given protocol
         * \throws runtime_error if statem machine 
         */
        static StateMachine getStateMachine(const std::string& protocol);

};

} // end namespace acl
} // end namespace fipa
#endif //FIPA_ACL_CONVERSATIONMONITOR_STATEMACHINE_FACTORY_H
