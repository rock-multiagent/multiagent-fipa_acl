#include "statemachine_factory.h"
#include <boost/filesystem/operations.hpp>
#include <base/logging.h>
#include "statemachine_reader.h"
#include "transition.h"

namespace fs = boost::filesystem;

namespace fipa {
namespace acl {

bool StateMachineFactory::msPreparedResourceDir = false;
std::string StateMachineFactory::msResourceDir = std::string("");
std::map<std::string, StateMachine> StateMachineFactory::msStateMachines;

StateMachineReader StateMachineFactory::msStateMachineReader;

void StateMachineFactory::setProtocolResourceDir(const std::string& resourceDir)
{
    fs::path protocolDir = fs::path(resourceDir);
    if(fs::is_directory(protocolDir))
    {
        msResourceDir = protocolDir.string();
    }
    LOG_INFO("Set protocol resource dir to: '%s' -- from '%s'", msResourceDir.c_str(), resourceDir.c_str());
}

void StateMachineFactory::prepareProtocolsFromResourceDir()
{
    fs::path protocolDir = fs::path(msResourceDir);
    LOG_INFO("Prepare protocols from: %s", protocolDir.string().c_str());
    if(fs::is_directory(protocolDir))
    {
        std::vector<fs::path> files;
        std::copy(fs::directory_iterator(protocolDir), fs::directory_iterator(), std::back_inserter(files));

        std::vector<fs::path>::iterator it = files.begin();
        for(;it != files.end(); ++it)
        {
            if(fs::is_regular_file(*it))
            {
                try {
                    // Extract protocol name
                    std::string protocolName = it->string();
                    size_t pos = protocolName.find_last_of("/");
                    protocolName.erase(0, pos+1);

                    // Loading the state machine from the given spec
                    StateMachine statemachine = msStateMachineReader.loadSpecification(it->string()); 
                    LOG_INFO("Register protocol %s", protocolName.c_str());                    
                    msStateMachines[protocolName] = statemachine;
                } catch(const std::runtime_error& e)
                {
                    LOG_ERROR("Error getting functional state machine: %s", e.what());
                }
            }
        }
    }

    msPreparedResourceDir = true;
}

StateMachine StateMachineFactory::getStateMachine(const std::string& protocol)
{
    if(!msPreparedResourceDir)
    {
        StateMachineFactory::prepareProtocolsFromResourceDir();
    }

    std::map<std::string, StateMachine>::iterator it = msStateMachines.find(protocol);

    if(it != msStateMachines.end())
    {
        return it->second;
    }

    LOG_WARN("StateMachine for protocol %s not found", protocol.c_str());
    throw std::runtime_error("State machine for requested protocol does not exist");
}

} // end namespace acl
} // end namespace fipa

